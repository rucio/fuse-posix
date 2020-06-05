//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#ifndef RUCIO_FUSE_CONNNECTION_PARAMETERS_H
#define RUCIO_FUSE_CONNNECTION_PARAMETERS_H

#include <constants.h>
#include <curl/curl.h>
#include <string>
#include <string.h>
#include <time.h>
#include <unordered_map>
#include <vector>

// Connection parameters struct definition
struct connection_parameters{
  std::string server_url;
  std::string account_name;
  std::string user_name;
  std::string password;

  connection_parameters(std::string server_url,
                        std::string account_name,
                        std::string user_name,
                        std::string password):
                        server_url(server_url),
                        account_name(account_name),
                        user_name(user_name),
                        password(password){}
};

// Token info struct definition
struct token_info{
  std::string conn_token = rucio_invalid_token;
  tm conn_token_exp;
  time_t conn_token_exp_epoch = 0;
};

// Rucio server descriptor
struct rucio_server{
  connection_parameters rucio_conn_params;
  token_info rucio_token_info;
  std::string config_file_path;

  rucio_server():rucio_conn_params("","","",""), rucio_token_info(){};

  rucio_server(std::string server_url,
               std::string account_name,
               std::string user_name,
               std::string password):
               rucio_conn_params(server_url,
                                 account_name,
                                 user_name,
                                 password),
               rucio_token_info(){}

  connection_parameters* get_params(){ return &rucio_conn_params; };
  token_info* get_token(){ return &rucio_token_info; };
};

// Shared token parameters
extern std::unordered_map<std::string, rucio_server> rucio_server_map;
extern std::vector<std::string> rucio_server_names;

// Utility functions
bool key_exists(std::string key);

connection_parameters* get_server_params(std::string server_name);

std::string* get_server_config(std::string server_name);

token_info* get_server_token(std::string server_name);

void parse_settings_json();

void parse_settings_cfg();

#endif //RUCIO_FUSE_CONNNECTION_PARAMETERS_H
