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

void rucio_get_auth_token_userpass(std::string short_server_name){
  struct curl_slist *headers = nullptr;

  auto conn_params = get_server_params(short_server_name);

  if(not conn_params){
    printf("Server not found. Aborting!");
    return;
  }

  auto xRucioAccount = "X-Rucio-Account: "+conn_params->account_name;
  auto xRucioUsername = "X-Rucio-Username: "+conn_params->user_name;
  auto xRucioPwd = "X-Rucio-Password: "+conn_params->password;

  headers= curl_slist_append(headers, xRucioAccount.c_str());
  headers= curl_slist_append(headers, xRucioUsername.c_str());
  headers= curl_slist_append(headers, xRucioPwd.c_str());

  auto curl_res = GET(conn_params->server_url+"/auth/userpass", headers, true);

  curl_slist_free_all(headers);

  std::string token = "";
  std::string expire_time_string = "";

  for(auto& line : curl_res.payload){
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
    printf("Server not found. Aborting!");
    return;
  }

  token_info->conn_token = (strlen(token.c_str())>0) ? token : rucio_invalid_token;

  expire_time_string = (strlen(expire_time_string.c_str())>0) ? expire_time_string : rucio_default_exp;
  strptime(expire_time_string.c_str(),"%a, %d %b %Y %H:%M:%S",&token_info->conn_token_exp);
  token_info->conn_token_exp_epoch = mktime(&token_info->conn_token_exp);
}

bool rucio_is_token_valid(std::string short_server_name){
  auto token_info = get_server_token(short_server_name);

  if(not token_info){
    printf("Server not found. Aborting!");
    return false;
  }

  return token_info->conn_token_exp_epoch >= time(0);
}

std::string rucio_server_from_path(const std::string& path){
  //TODO: extract path from string
  return "";
}

std::vector<std::string> rucio_list_servers(){
  std::vector<std::string> servers;
  servers.reserve(rucio_server_map.size());

  for(auto const& server: rucio_server_map)
      servers.push_back(server.first);

  return servers;
}

std::vector<std::string> rucio_list_scopes(std::string short_server_name){
  auto conn_params = get_server_params(short_server_name);
  auto token_info = get_server_token(short_server_name);

  if(not token_info || not conn_params){
    printf("Server not found. Aborting!");
    return {""};
  }

  if(not rucio_is_token_valid(short_server_name)) rucio_get_auth_token_userpass(short_server_name);

  auto xRucioToken = "X-Rucio-Auth-Token: "+token_info->conn_token;

  struct curl_slist *headers = nullptr;

  headers= curl_slist_append(headers, xRucioToken.c_str());

  auto curl_res = GET(conn_params->server_url+"/scopes/", headers);

  curl_slist_free_all(headers);

  std::vector<std::string> scopes;

  for(auto& line : curl_res.payload){
    tokenize_python_list(line, scopes);
  }

  return scopes;
}

std::vector<rucio_did> rucio_list_dids(const std::string& scope, std::string short_server_name){
  auto conn_params = get_server_params(short_server_name);
  auto token_info = get_server_token(short_server_name);


  if(not token_info || not conn_params){
    printf("Server not found. Aborting!");
    return {};
  }

  if(not rucio_is_token_valid(short_server_name)) rucio_get_auth_token_userpass(short_server_name);

  auto xRucioToken = "X-Rucio-Auth-Token: "+token_info->conn_token;

  struct curl_slist *headers = nullptr;

  headers= curl_slist_append(headers, xRucioToken.c_str());

  auto curl_res = GET(conn_params->server_url+"/dids/"+scope+"/", headers);

  curl_slist_free_all(headers);

  std::vector<rucio_did> dids;

  for(auto& line : curl_res.payload){
    structurize_did(line, dids);
  }

  return dids;
}

std::vector<std::string> rucio_list_container_dids(const std::string& scope, const std::string& container_name, std::string short_server_name){
  auto conn_params = get_server_params(short_server_name);

  if(not conn_params){
    printf("Server not found. Aborting!");
    return {""};
  }

  auto curl_res = GET(conn_params->server_url+"/dids/"+scope+"/"+container_name+"/"); //????? Just guessing, not right!
  return curl_res.payload;
}

bool rucio_is_container(const std::string& path){
  return false;
}
