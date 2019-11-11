//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#include <REST-API.h>
#include <curl-REST.h>
#include <globals.h>
#include <sha1.hpp>
#include <utils.h>
#include <iostream>
#include <time.h>

using namespace fastlog;

bool rucio_ping(const std::string& server_url){
  auto curl_res = GET(server_url+"/ping");
  return curl_res.res == CURLE_OK;
}

void rucio_get_auth_token_userpass(const std::string& short_server_name){
  auto srv = get_server(short_server_name);

  if (not srv) {
    fastlog(ERROR, "Server %s not found. Aborting!", short_server_name.data());
    return;
  }

  struct curl_slist *headers = nullptr;
  auto conn_params = &(srv->rucio_conn_params);

  curlRet curl_res;

  switch (srv->auth)
  {
    case auth_method::userpass:{
      auto xRucioAccount = "X-Rucio-Account: " + conn_params->account_name;
      auto xRucioUsername = "X-Rucio-Username: " + conn_params->user_name;
      auto xRucioPwd = "X-Rucio-Password: " + conn_params->password;

      headers = curl_slist_append(headers, xRucioAccount.c_str());
      headers = curl_slist_append(headers, xRucioUsername.c_str());
      headers = curl_slist_append(headers, xRucioPwd.c_str());

      curl_res = GET(conn_params->server_url + "/auth/userpass", headers, true);

      break;
    }
//    case auth_method::x509proxy:
//    case auth_method::x509:{
//      auto xRucioAccount = "X-Rucio-Account: " + conn_params->account_name;
//      curlx509Bundle bundle;
//
//      //TODO: fill the bundle
//      switch(srv->auth){
//        case auth_method::x509proxy: bundle;
//        case auth_method::x509: bundle;
//      }
//
//      headers = curl_slist_append(headers, xRucioAccount.c_str());
//
//      curl_res = GET_x509(conn_params->server_url + "/auth/x509", bundle, headers, true);
//
//      break;
//    }
    default:{
      fastlog(ERROR, "Unsupported auth method!");
      return;
    }
  }

  curl_slist_free_all(headers);

  std::string token;
  std::string expire_time_string;

  for (auto &line : curl_res.payload) {
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

  if (not token_info) {
    fastlog(ERROR, "Server %s not found. Aborting!", short_server_name.data());
    return;
  }

  token_info->conn_token = (strlen(token.c_str()) > 0) ? token : rucio_invalid_token;

  expire_time_string = (strlen(expire_time_string.c_str()) > 0) ? expire_time_string : rucio_default_exp;
  strptime(expire_time_string.c_str(), "%a, %d %b %Y %H:%M:%S", &token_info->conn_token_exp);
  token_info->conn_token_exp_epoch = mktime(&token_info->conn_token_exp);
}

bool rucio_is_token_valid(const std::string& short_server_name){
  auto token_info = get_server_token(short_server_name);

  if(not token_info){
    fastlog(ERROR,"Server %s not found. Aborting!", short_server_name.data());
    return false;
  }

  return token_info->conn_token_exp_epoch >= time(nullptr);
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

    if (not rucio_is_token_valid(short_server_name)) rucio_get_auth_token_userpass(short_server_name);

    auto xRucioToken = "X-Rucio-Auth-Token: " + token_info->conn_token;

    struct curl_slist *headers = nullptr;

    headers = curl_slist_append(headers, xRucioToken.c_str());

    auto curl_res = GET(conn_params->server_url + "/scopes/", headers);

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

  if(not rucio_is_token_valid(short_server_name)) rucio_get_auth_token_userpass(short_server_name);

  auto xRucioToken = "X-Rucio-Auth-Token: "+token_info->conn_token;

  struct curl_slist *headers = nullptr;

  headers= curl_slist_append(headers, xRucioToken.c_str());

  return headers;
}

std::vector<rucio_did> rucio_list_dids(const std::string& scope, const std::string& short_server_name){
  auto key = short_server_name+scope;
  auto found = dids_cache.find(key);
  if(found == dids_cache.end()) {
    auto headers = get_auth_headers(short_server_name);

    if (not headers) {
      fastlog(ERROR,"Server %s not found. Aborting!", short_server_name.data());
      return {};
    }

    auto curl_res = GET(get_server_params(short_server_name)->server_url + "/dids/" + scope + "/", headers);

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
  auto key = short_server_name+scope+container_name;
  auto found = container_dids_cache.find(key);
  if(found == container_dids_cache.end()) {

    auto headers = get_auth_headers(short_server_name);

    if (not headers) {
      fastlog(ERROR,"Server %s not found. Aborting!", short_server_name.data());
      return {};
    }

    auto curl_res = GET(
            get_server_params(short_server_name)->server_url + "/dids/" + scope + "/" + container_name + "/dids",
            headers);

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
  auto scope = extract_scope(path);
  auto name = extract_name(path);
  auto found = is_container_cache.find(short_server_name+scope+name);

  if(found == is_container_cache.end()) {
    auto headers = get_auth_headers(short_server_name);

    if (not headers) {
      fastlog(ERROR,"Server %s not found. Aborting!", short_server_name.data());
      return {};
    }

    auto curl_res = GET(get_server_params(short_server_name)->server_url + "/dids/" + scope + "/" + name, headers);

    curl_slist_free_all(headers);

    is_container_cache[path] = curl_res.payload.front().find(R"("type": "FILE",)") == std::string::npos;
    return is_container_cache[path];
  } else {
    fastlog(DEBUG,"USING CACHE");
    fastlog(DEBUG,"%s:%s:%s -> %s",short_server_name.data(), scope.data(), name.data(),
              (is_container_cache[short_server_name+scope+name])?"true":"false");
    return found->second;
  }
}
