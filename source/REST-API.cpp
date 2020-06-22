/*
Created by Gabriele Gaetano Fronzé on 2019-10-16.
Authors:
- Gabriele Gaetano Fronzé <gabriele.fronze at to.infn.it>, 2019
- Vivek Nigam <viveknigam.nigam3@gmail.com>, 2020
*/

#include <REST-API.h>
#include <curl-REST.h>
#include <globals.h>
#include <sha1.hpp>
#include <utils.h>
#include <iostream>
#include <time.h>

using namespace fastlog;

bool rucio_ping(const std::string& short_server_name){
  auto conn_params = get_server_params(short_server_name);

  auto curl_res = GET(conn_params->server_url+"/ping", conn_params->ca_path);
  return curl_res.res == CURLE_OK;
}

bool rucio_validate_server(const std::string& short_server_name){
  auto conn_params = get_server_params(short_server_name);

  if(not rucio_ping(short_server_name)){
    fastlog(ERROR, "Server %s unreachable via network.", conn_params->server_url.data());
    return false;
  }

  if(rucio_get_auth_token(short_server_name) != TOKEN_OK){
    fastlog(ERROR, "Cannot validate server %s auth settings.", conn_params->server_url.data());
    return false;
  }

  return true;
}

int rucio_get_auth_token(const std::string& short_server_name){

  auto conn_params = get_server_params(short_server_name);

  switch (conn_params->rucio_auth_mode){
    case auth_mode::userpass: return rucio_get_auth_token_userpass(short_server_name);
    case auth_mode::x509: return rucio_get_auth_token_x509(short_server_name);
    default: return TOKEN_ERROR;
  }
}

int rucio_get_auth_token_userpass(const std::string& short_server_name){

  struct curl_slist *headers = nullptr;

  auto conn_params = get_server_params(short_server_name);

  if(not conn_params){
    fastlog(ERROR,"Server %s not found. Aborting!", short_server_name.data());
    return SERVER_NOT_LOADED;
  }

  auto xRucioAccount = "X-Rucio-Account: "+conn_params->account_name;
  auto xRucioUsername = "X-Rucio-Username: "+conn_params->user_name;
  auto xRucioPwd = "X-Rucio-Password: "+conn_params->password;

  headers= curl_slist_append(headers, xRucioAccount.c_str());
  headers= curl_slist_append(headers, xRucioUsername.c_str());
  headers= curl_slist_append(headers, xRucioPwd.c_str());

  auto curl_res = GET(conn_params->server_url+"/auth/userpass", conn_params->ca_path, headers, true);
  if(curl_res.res != CURLE_OK){
    fastlog(ERROR, "Token: Curl error. Abort.");
    return CURL_ERROR;
  }

  curl_slist_free_all(headers);

  std::string token;
  std::string expire_time_string;

  for(auto& line : curl_res.payload){
    if (line.find(rucio_token_exception_prefix) != std::string::npos) {
      fastlog(ERROR, "Wrong authentication parameters for server %s!",short_server_name.data());
      return CANNOT_AUTH;
    }

    if (line.find(rucio_token_prefix) != std::string::npos) {
      token = line;
      token.erase(0, rucio_token_prefix_size);
    }

    if (line.find(rucio_token_duration_prefix) != std::string::npos) {
      expire_time_string = line;
      expire_time_string.erase(0, rucio_token_duration_prefix_size);
    }
  }

  auto token_info = get_server_token(short_server_name);

  if(not token_info){
    fastlog(ERROR,"Server %s didn't provide token. Aborting!", short_server_name.data());
    return TOKEN_ERROR;
  }

  token_info->conn_token = (strlen(token.c_str())>0) ? token : rucio_invalid_token;

  expire_time_string = (strlen(expire_time_string.c_str())>0) ? expire_time_string : rucio_default_exp;
  strptime(expire_time_string.data(), "%a, %d %b %Y %H:%M:%S",&token_info->conn_token_exp);
  char UTC[] = {'U','T','C'};
  token_info->conn_token_exp.tm_zone = UTC;
  token_info->conn_token_exp_epoch = mktime(&token_info->conn_token_exp) - timezone;

  return TOKEN_OK;
}

int rucio_get_auth_token_x509(const std::string& short_server_name){

  struct curl_slist *headers = nullptr;

  auto conn_params = get_server_params(short_server_name);

  if(not conn_params){
    fastlog(ERROR,"Server %s not found. Aborting!", short_server_name.data());
    return SERVER_NOT_LOADED;
  }

  auto xRucioAccount = "X-Rucio-Account: "+conn_params->account_name;

  curlx509Bundle* bundle = get_server_SSL_bundle(short_server_name);

  headers = curl_slist_append(headers, xRucioAccount.c_str());

  auto curl_res = GET_x509(conn_params->server_url + "/auth/x509", *bundle, headers, true);
  if(curl_res.res != CURLE_OK){
    fastlog(ERROR, "Token x509: Curl error. Abort.");
    return CURL_ERROR;
  }


  curl_slist_free_all(headers);

  std::string token;
  std::string expire_time_string;

  for(auto& line : curl_res.payload){
    if (line.find(rucio_token_exception_prefix) != std::string::npos) {
      fastlog(ERROR, "Wrong authentication parameters for server %s! Aborting!",short_server_name.data());
      return CANNOT_AUTH;
    }

    if (line.find(rucio_token_prefix) != std::string::npos) {
      token = line;
      token.erase(0, rucio_token_prefix_size);
    }

    if (line.find(rucio_token_duration_prefix) != std::string::npos) {
      expire_time_string = line;
      expire_time_string.erase(0, rucio_token_duration_prefix_size);
    }
  }

  auto token_info = get_server_token(short_server_name);

  if(not token_info){
    fastlog(ERROR,"Server %s didn't provide token. Aborting!", short_server_name.data());
    return TOKEN_ERROR;
  }

  token_info->conn_token = (strlen(token.c_str())>0) ? token : rucio_invalid_token;

  expire_time_string = (strlen(expire_time_string.c_str())>0) ? expire_time_string : rucio_default_exp;
  strptime(expire_time_string.data(), "%a, %d %b %Y %H:%M:%S",&token_info->conn_token_exp);
  char UTC[] = {'U','T','C'};
  token_info->conn_token_exp.tm_zone = UTC;
  token_info->conn_token_exp_epoch = mktime(&token_info->conn_token_exp) - timezone;

  return TOKEN_OK;
}

bool rucio_is_token_valid(const std::string& short_server_name){
  auto token_info = get_server_token(short_server_name);

  if(not token_info){
    fastlog(ERROR,"Server %s not found. Aborting!", short_server_name.data());
    return false;
  }

  return difftime(token_info->conn_token_exp_epoch, time(nullptr)) >= 0;
}

const std::vector<std::string>& rucio_list_servers(){
  return rucio_server_names;
}

std::vector<std::string> rucio_list_scopes(const std::string& short_server_name){
  auto found = scopes_cache.find(short_server_name);
  if(found == scopes_cache.end()) {
    auto conn_params = get_server_params(short_server_name);
    auto token_info = get_server_token(short_server_name);

    if (not token_info || not conn_params) {
      fastlog(ERROR,"Server %s not found. Aborting!", short_server_name.data());
      return {};
    }

    if (not rucio_is_token_valid(short_server_name)) rucio_get_auth_token(short_server_name);

    auto xRucioToken = "X-Rucio-Auth-Token: " + token_info->conn_token;

    struct curl_slist *headers = nullptr;

    headers = curl_slist_append(headers, xRucioToken.c_str());

    auto curl_res = GET(conn_params->server_url + "/scopes/", conn_params->ca_path, headers);
    if(curl_res.res != CURLE_OK){
      fastlog(ERROR, "Scopes: Curl error. Abort.");
      return {};
    }


    curl_slist_free_all(headers);

    std::vector<std::string> scopes;

    for (auto &line : curl_res.payload) {
      tokenize_python_list(line, scopes);
    }

    scopes_cache[short_server_name] = std::move(scopes);
    return scopes_cache[short_server_name];
  } else {
    fastlog(DEBUG,"USING CACHE");
    return found->second;
  }
}

curl_slist* get_auth_headers(const std::string& short_server_name){
  auto conn_params = get_server_params(short_server_name);
  auto token_info = get_server_token(short_server_name);

  if(not token_info || not conn_params){
    fastlog(ERROR,"Server %s not found. Aborting!", short_server_name.data());
    return nullptr;
  }

  if(not rucio_is_token_valid(short_server_name)) rucio_get_auth_token(short_server_name);

  auto xRucioToken = "X-Rucio-Auth-Token: "+token_info->conn_token;

  struct curl_slist *headers = nullptr;

  headers= curl_slist_append(headers, xRucioToken.c_str());

  return headers;
}

std::vector<rucio_did> rucio_list_dids(const std::string& scope, const std::string& short_server_name){
  auto conn_params = get_server_params(short_server_name);
  auto key = short_server_name+scope;
  auto found = dids_cache.find(key);
  if(found == dids_cache.end()) {
    auto headers = get_auth_headers(short_server_name);

    if (not headers) {
      fastlog(ERROR,"Server %s not found. Aborting!", short_server_name.data());
      return {};
    }

    auto curl_res = GET(conn_params->server_url + "/dids/" + scope + "/", conn_params->ca_path, headers);
    if(curl_res.res != CURLE_OK){
      fastlog(ERROR, "Dids: Curl error. Abort.");
      return {};
    }


    curl_slist_free_all(headers);

    std::vector<rucio_did> dids;

    for (auto &line : curl_res.payload) {
      structurize_did(line, dids);
    }

    for(const auto& did : dids){
      is_container_cache[short_server_name+scope+did.name] = did.type != rucio_data_type::rucio_file;
    }

    dids_cache[key] = std::move(dids);
    return dids_cache[key];
  } else {
    fastlog(DEBUG,"USING CACHE");
    return found->second;
  }
}

std::vector<rucio_did> rucio_list_container_dids(const std::string& scope, const std::string& container_name, const std::string& short_server_name){
  auto conn_params = get_server_params(short_server_name);
  auto key = short_server_name+scope+container_name;
  auto found = container_dids_cache.find(key);
  if(found == container_dids_cache.end()) {

    auto headers = get_auth_headers(short_server_name);

    if (not headers) {
      fastlog(ERROR,"Server %s not found. Aborting!", short_server_name.data());
      return {};
    }

    auto curl_res = GET(
            conn_params->server_url + "/dids/" + scope + "/" + container_name + "/dids",
            conn_params->ca_path,
            headers);
    if(curl_res.res != CURLE_OK){
      fastlog(ERROR, "Container: Curl error. Abort.");
      return {};
    }


    curl_slist_free_all(headers);

    std::vector<rucio_did> dids;

    for (auto &line : curl_res.payload) {
      structurize_container_did(line, dids);
    }

    for(const auto& did : dids){
      is_container_cache[short_server_name+scope+did.name] = did.type != rucio_data_type::rucio_file;
      fastlog(DEBUG,"%s:%s:%s -> %s",short_server_name.data(), scope.data(), did.name.data(),
              (is_container_cache[short_server_name+scope+did.name])?"true":"false");
    }

    container_dids_cache[key] = std::move(dids);
    return container_dids_cache[key];
  } else {
    fastlog(DEBUG,"USING CACHE");
    return found->second;
  }
}

bool rucio_is_container(const rucio_did& did){
  return did.type != rucio_data_type::rucio_file;
}

bool rucio_is_container(const std::string& path){
  auto short_server_name = extract_server_name(path);
  auto conn_params = get_server_params(short_server_name);
  auto scope = extract_scope(path);
  auto name = extract_name(path);
  auto found = is_container_cache.find(short_server_name+scope+name);

  if(found == is_container_cache.end()) {
    auto headers = get_auth_headers(short_server_name);

    if (not headers) {
      fastlog(ERROR,"Server %s not found. Aborting!", short_server_name.data());
      return {};
    }

    auto curl_res = GET(conn_params->server_url + "/dids/" + scope + "/" + name,
                        conn_params->ca_path,
                        headers);
    if(curl_res.res != CURLE_OK){
      fastlog(ERROR, "IsCont: Curl error. Abort.");
      return false;
    }


    curl_slist_free_all(headers);

    is_container_cache[path] = curl_res.payload.front().find(R"("CONTAINER",)") != std::string::npos;
    is_container_cache[path] |= curl_res.payload.front().find(R"("DATASET",)") != std::string::npos;
    return is_container_cache[path];
  } else {
    fastlog(DEBUG,"USING CACHE");
    fastlog(DEBUG,"%s:%s:%s -> %s",short_server_name.data(), scope.data(), name.data(),
              (is_container_cache[short_server_name+scope+name])?"true":"false");
    return found->second;
  }
}

bool rucio_is_file(const std::string& path){
  auto short_server_name = extract_server_name(path);
  auto conn_params = get_server_params(short_server_name);
  auto scope = extract_scope(path);
  auto name = extract_name(path);
  auto found = is_file_cache.find(short_server_name+scope+name);

  if(found == is_container_cache.end()) {
    auto headers = get_auth_headers(short_server_name);

    if (not headers) {
      fastlog(ERROR,"Server %s not found. Aborting!", short_server_name.data());
      return {};
    }

    auto curl_res = GET(conn_params->server_url + "/dids/" + scope + "/" + name,
                        conn_params->ca_path,
                        headers);
    if(curl_res.res != CURLE_OK){
      fastlog(ERROR, "IsFile: Curl error. Abort. %s", (conn_params->server_url + "/dids/" + scope + "/" + name).data());
      return false;
    }


    curl_slist_free_all(headers);

    is_file_cache[path] = curl_res.payload.front().find(R"("FILE",)") != std::string::npos;
    return is_file_cache[path];
  } else {
    fastlog(DEBUG,"USING CACHE");
    fastlog(DEBUG,"%s:%s:%s -> %s",short_server_name.data(), scope.data(), name.data(),
            (is_file_cache[short_server_name+scope+name])?"true":"false");
    return found->second;
  }
}

size_t rucio_get_size(const std::string& path){
  auto cache_found = file_size_cache.find(path);

  if(cache_found != file_size_cache.end()){
    return cache_found->second;
  }

  auto short_server_name = extract_server_name(path);
  auto conn_params = get_server_params(short_server_name);
  auto scope = extract_scope(path);
  auto name = extract_name(path);

  auto headers = get_auth_headers(short_server_name);

  if (not headers) {
    fastlog(ERROR,"Server %s not found. Aborting!", short_server_name.data());
    return {};
  }

  auto curl_res = GET(conn_params->server_url + "/dids/" + scope + "/" + name,
                      conn_params->ca_path,
                      headers);
  if(curl_res.res != CURLE_OK){
    fastlog(ERROR, "Size: Curl error. Abort.");
    return 0;
  }


  for(auto const& payload : curl_res.payload){
    auto found = payload.find(rucio_bytes_metadata);
    if (found != std::string::npos) {
      auto pos = payload.find(rucio_bytes_metadata);
      auto pos2 = payload.find(',', pos + rucio_bytes_metadata_length - 1);
      auto size_bytes = payload.substr(pos + rucio_bytes_metadata_length, pos2 - pos - rucio_bytes_metadata_length);
      fastlog(INFO, "File size is %s", size_bytes.data());
      auto size_i = std::stoi(size_bytes);
      file_size_cache.emplace(path, size_i);
      return size_i;
    }
  }
  return -1;
}

std::vector<std::string> rucio_get_replicas_metalinks(const std::string& path){
  auto short_server_name = extract_server_name(path);
  auto conn_params = get_server_params(short_server_name);
  auto scope = extract_scope(path);
  auto name = extract_name(path);

  auto headers = get_auth_headers(short_server_name);

  if (not headers) {
    fastlog(ERROR,"Server %s not found. Aborting!", short_server_name.data());
    return {};
  }
  headers= curl_slist_append(headers, "HTTP_ACCEPT: metalink4+xml");

  auto curl_res = GET(conn_params->server_url + "/replicas/" + scope + "/" + name,
                      conn_params->ca_path,
                      headers);
  if(curl_res.res != CURLE_OK){
    fastlog(ERROR, "Meta: Curl error. Abort.");
    return {};
  }


  std::string merged_response;

  for(const auto& line : curl_res.payload){
    fastlog(DEBUG, "%s", line.data());
    merged_response.append(line);
  }

  fastlog(DEBUG, "\n\nMerged:\n%s", merged_response.data());

  std::string identifier = R"("rses": {)";
  auto rses_position = merged_response.find(identifier);
  auto rses_end = merged_response.find('}', rses_position+1);
  auto rses = std::string(merged_response.begin() + rses_position + identifier.length(), merged_response.begin() + rses_end);

  fastlog(DEBUG, "\n\nRSES:\n%s\n\n", rses.data());

  auto beg_pfn = rses.find('[', 0);
  auto end_pfn = rses.find(']', beg_pfn + 1);

  std::cout << beg_pfn << std::endl;
  std::cout << end_pfn << std::endl;

  std::vector<std::string> pfns;

  while(beg_pfn != std::string::npos && end_pfn != std::string::npos){
    pfns.emplace_back(std::string(rses.begin() + beg_pfn + 2, rses.begin() + end_pfn - 1));

    beg_pfn = rses.find('[', end_pfn + 1);
    end_pfn = rses.find(']', beg_pfn + 1);

    fastlog(DEBUG, "---> %s", pfns.back().data());
  }

  return std::move(pfns);
}

