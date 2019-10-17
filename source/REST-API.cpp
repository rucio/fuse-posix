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

void rucio_get_auth_token_userpass(){
  struct curl_slist *headers = NULL;

  auto xRucioAccount = "X-Rucio-Account: "+rucio_account_name;
  auto xRucioUsername = "X-Rucio-Username: "+rucio_user_name;
  auto xRucioPwd = "X-Rucio-Password: "+rucio_password;

  headers= curl_slist_append(headers, xRucioAccount.c_str());
  headers= curl_slist_append(headers, xRucioUsername.c_str());
  headers= curl_slist_append(headers, xRucioPwd.c_str());

  auto curl_res = GET(rucio_server_url+"/auth/userpass", headers, true);

  curl_slist_free_all(headers);

  std::string token;

  for(const auto& line : curl_res.payload){
    if (line.find(rucio_token_prefix) != std::string::npos) {
      token = line;
      token.erase(0, rucio_token_prefix_size);
    }
  }

  token = (strlen(token.c_str())>0) ? token : ">>>---invalid-token---<<<";

  rucio_conn_token = token;
}

std::vector<std::string> rucio_list_scopes(){
  auto curl_res = GET(rucio_server_url+"/scopes");
  return curl_res.payload;
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
