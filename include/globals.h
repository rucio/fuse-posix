#include <utility>

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
// Contains all the parameteres defining how to contact a rucio server
struct connection_parameters{
  std::string server_url;
  std::string account_name;
  std::string user_name;
  std::string password;

  connection_parameters(std::string server_url,
                        std::string account_name,
                        std::string user_name,
                        std::string password):
                        server_url(std::move(server_url)),
                        account_name(std::move(account_name)),
                        user_name(std::move(user_name)),
                        password(std::move(password)){}
};

// Token info struct definition
// Contains token string for REST calls and toke expire information
struct token_info{
  std::string conn_token = rucio_invalid_token;
  tm conn_token_exp;
  time_t conn_token_exp_epoch = 0;
};

// Rucio server descriptor
// Packs together all the aboves structs
struct rucio_server{
  connection_parameters rucio_conn_params;
  token_info rucio_token_info;
  std::string config_file_path;

  rucio_server():rucio_conn_params("","","",""), rucio_token_info(){};

  rucio_server(std::string server_url,
               std::string account_name,
               std::string user_name,
               std::string password):
               rucio_conn_params(std::move(server_url),
                                 std::move(account_name),
                                 std::move(user_name),
                                 std::move(password)),
               rucio_token_info(){}

  connection_parameters* get_params(){ return &rucio_conn_params; };
  token_info* get_token(){ return &rucio_token_info; };
};

// Shared server descriptors
extern std::unordered_map<std::string, rucio_server> rucio_server_map;
extern std::vector<std::string> rucio_server_names;

// Utility functions
bool server_exists(const std::string &key);
connection_parameters* get_server_params(const std::string& server_name);
std::string* get_server_config(const std::string& server_name);
token_info* get_server_token(const std::string& server_name);

// Startup functions
void parse_settings_cfg();
bool check_permissions(const std::string& mountpoint_path);

#endif //RUCIO_FUSE_CONNNECTION_PARAMETERS_H
