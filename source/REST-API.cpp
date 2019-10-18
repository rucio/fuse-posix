//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#include <REST-API.h>
#include <curl-REST.h>
#include <globals.h>
#include <vector>
#include <string>
#include <sha1.hpp>
#include <utils.h>
#include <iostream>
#include <time.h>

void rucio_get_auth_token_userpass(){
  struct curl_slist *headers = nullptr;

  auto xRucioAccount = "X-Rucio-Account: "+rucio_connection_parameters.account_name;
  auto xRucioUsername = "X-Rucio-Username: "+rucio_connection_parameters.user_name;
  auto xRucioPwd = "X-Rucio-Password: "+rucio_connection_parameters.password;

  headers= curl_slist_append(headers, xRucioAccount.c_str());
  headers= curl_slist_append(headers, xRucioUsername.c_str());
  headers= curl_slist_append(headers, xRucioPwd.c_str());

  auto curl_res = GET(rucio_connection_parameters.server_url+"/auth/userpass", headers, true);

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

  rucio_token_info.conn_token = (strlen(token.c_str())>0) ? token : rucio_invalid_token;

  expire_time_string = (strlen(expire_time_string.c_str())>0) ? expire_time_string : rucio_default_exp;
  strptime(expire_time_string.c_str(),"%a, %d %b %Y %H:%M:%S",&rucio_token_info.conn_token_exp);
  rucio_token_info.conn_token_exp_epoch = mktime(&rucio_token_info.conn_token_exp);
}

bool rucio_is_token_valid(){
  return rucio_token_info.conn_token_exp_epoch >= time(0);
}

std::vector<std::string> rucio_list_scopes(){
  if(not rucio_is_token_valid()) rucio_get_auth_token_userpass();

  struct curl_slist *headers = nullptr;

  auto xRucioToken = "X-Rucio-Auth-Token: "+rucio_token_info.conn_token;

  headers= curl_slist_append(headers, xRucioToken.c_str());

  auto curl_res = GET(rucio_connection_parameters.server_url+"/scopes/", headers);

  curl_slist_free_all(headers);

  std::vector<std::string> scopes;

  for(auto& line : curl_res.payload){
    tokenize_python_list(line, scopes);
  }

  return scopes;
}

std::vector<std::string> rucio_list_dids(const std::string& scope){
  auto curl_res = GET(rucio_connection_parameters.server_url+"/dids/"+scope+"/");
  return curl_res.payload;
}

std::vector<std::string> rucio_list_container_dids(const std::string& scope, const std::string& container_name){
  auto curl_res = GET(rucio_connection_parameters.server_url+"/dids/"+scope+"/"+container_name+"/"); //????? Just guessing, not right!
  return curl_res.payload;
}

bool rucio_is_container(const std::string& path){
  return false;
}
