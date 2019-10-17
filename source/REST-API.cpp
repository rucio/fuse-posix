//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#include <REST-API.h>
#include <curl-REST.h>
#include <conn-par.h>
#include <vector>
#include <string>
#include <sha1.hpp>
#include <utils.h>

std::string rucio_get_auth_token_userpass(){
  struct curl_slist *headers = NULL;
  char base64[SHA1_BASE64_SIZE];

  auto xRucioAccount = "X-Rucio-Account: "+rucio_account_name;
  auto xRucioUsername = "X-Rucio-Username: "+rucio_user_name;

  sha1(rucio_password.c_str()).print_base64(base64);
  auto xRucioPwd = "X-Rucio-Password: "+to_string(base64, SHA1_BASE64_SIZE);

  headers= curl_slist_append(headers, xRucioAccount.c_str());
  headers= curl_slist_append(headers, xRucioUsername.c_str());
  headers= curl_slist_append(headers, xRucioPwd.c_str());

  auto curl_res = GET(rucio_server_url+"/auth/userpass", headers);
  return "";
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
