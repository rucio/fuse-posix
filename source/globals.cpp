//
// Created by Gabriele Gaetano Fronzé on 2019-10-17.
//

#include <globals.h>

std::unordered_map<std::string, rucio_server> rucio_server_map = {
                                                                  {
                                                                    "rucio-server-torino",
                                                                    rucio_server("https://rucio-server",
                                                                                 "root",
                                                                                 "ddmlab",
                                                                                 "secret")
                                                                  },
                                                                  {
                                                                    "rucio-server-ligo",
                                                                    rucio_server("https://rucio-server",
                                                                                 "root",
                                                                                 "ddmlab",
                                                                                 "secret")
                                                                  }
                                                                 };

bool key_exists(std::string key){
  return rucio_server_map.count(key)>0;
}

connection_parameters* get_server_params(std::string server_name){
  return (key_exists(server_name)) ? rucio_server_map[server_name].get_params() : nullptr;
}

token_info* get_server_token(std::string server_name){
  return (key_exists(server_name)) ? rucio_server_map[server_name].get_token() : nullptr;
}