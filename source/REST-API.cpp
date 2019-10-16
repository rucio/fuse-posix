//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#include <REST-API.h>
#include <curl-REST.h>
#include <conn-par.h>
#include <vector>
#include <string>

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
