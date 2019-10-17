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
#include <sstream>
#include <algorithm>

void rucio_get_auth_token_userpass(){
  struct curl_slist *headers = nullptr;

  auto xRucioAccount = "X-Rucio-Account: "+rucio_account_name;
  auto xRucioUsername = "X-Rucio-Username: "+rucio_user_name;
  auto xRucioPwd = "X-Rucio-Password: "+rucio_password;

  headers= curl_slist_append(headers, xRucioAccount.c_str());
  headers= curl_slist_append(headers, xRucioUsername.c_str());
  headers= curl_slist_append(headers, xRucioPwd.c_str());

  auto curl_res = GET(rucio_server_url+"/auth/userpass", headers, true);

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

  rucio_conn_token = (strlen(token.c_str())>0) ? token : rucio_invalid_token;

  expire_time_string = (strlen(expire_time_string.c_str())>0) ? expire_time_string : rucio_default_exp;
  std::cout << expire_time_string << std::endl;
  strptime(expire_time_string.c_str(),"%a, %d %b %Y %H:%M:%S",&rucio_conn_token_exp);
  rucio_conn_token_exp_epoch = mktime(&rucio_conn_token_exp);
}

bool rucio_is_token_valid(){
  return rucio_conn_token_exp_epoch >= time(0);
}

std::vector<std::string> rucio_list_scopes(){
  if(not rucio_is_token_valid()) rucio_get_auth_token_userpass();

  struct curl_slist *headers = nullptr;

  auto xRucioToken = "X-Rucio-Auth-Token: "+rucio_conn_token;

  headers= curl_slist_append(headers, xRucioToken.c_str());

  auto curl_res = GET(rucio_server_url+"/scopes/", headers);

  curl_slist_free_all(headers);

  std::vector<std::string> scopes;
  std::string scope;

  for(auto& line : curl_res.payload){
    line.erase(0,1);
    line.erase(line.size() -1);
    std::stringstream stream(line.c_str());
    while(getline(stream, scope, ',')){
      scope.erase(std::remove(scope.begin(), scope.end(), '"'), scope.end());
      scope.erase(std::remove(scope.begin(), scope.end(), ' '), scope.end());
      scopes.emplace_back(scope);
    }
  }

  return scopes;
}

std::vector<std::string> rucio_list_dids(const std::string& scope){
  auto curl_res = GET(rucio_server_url+"/dids/"+scope);
  return curl_res.payload;
}

std::vector<std::string> rucio_list_container_dids(const std::string& scope, const std::string& container_name){
  auto curl_res = GET(rucio_server_url+"/dids/"+scope+"/"+container_name); //????? Just guessing, not right!
  return curl_res.payload;
}

bool rucio_is_container(const std::string& path){
  return false;
}
