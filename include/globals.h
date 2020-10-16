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
#include <utility>
#include "curl-REST.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enumerator to define supported enum types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum auth_mode{
    userpass,
    x509,
    none
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Methods to retrieve selected auth method from settings string
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
auth_mode get_auth_mode(const std::string& settings_line);
std::string get_auth_name(auth_mode mode);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Connection parameters struct definition
// Contains all the parameteres defining how to contact a rucio server
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct connection_parameters{
  std::string server_url;
  std::string account_name;
  std::string user_name;
  std::string password;
  auth_mode rucio_auth_mode;
  std::string ca_path;

  connection_parameters(std::string server_url,
                        std::string account_name,
                        std::string user_name,
                        std::string password,
                        std::string ca_path,
                        const std::string& auth_method_line = "userpass"):
                        server_url(std::move(server_url)),
                        account_name(std::move(account_name)),
                        user_name(std::move(user_name)),
                        password(std::move(password)),
                        ca_path(std::move(ca_path)),
                        rucio_auth_mode(get_auth_mode(auth_method_line)){}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Token info struct definition
// Contains token string for REST calls and toke expire information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct token_info{
  std::string conn_token = rucio_invalid_token;
  tm conn_token_exp;
  time_t conn_token_exp_epoch = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rucio server descriptor
// Packs together all the aboves structs
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct rucio_server{
  connection_parameters rucio_conn_params;
  token_info rucio_token_info;
  std::string config_file_path;

  rucio_server():rucio_conn_params("","","","",""), rucio_token_info(){};

  rucio_server(std::string server_url,
               std::string account_name,
               std::string user_name,
               std::string password,
               std::string ca_path,
               const std::string& auth_mode):
               rucio_conn_params(std::move(server_url),
                                 std::move(account_name),
                                 std::move(user_name),
                                 std::move(password),
                                 std::move(ca_path),
                                 auth_mode),
               rucio_token_info(){}

  connection_parameters* get_params(){ return &rucio_conn_params; };
  token_info* get_token(){ return &rucio_token_info; };
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Server descriptors cache
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern std::unordered_map<std::string, rucio_server> rucio_server_map;
extern std::vector<std::string> rucio_server_names;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Server and scope existance utilities
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool server_exists(const std::string &key);
bool scope_exists(const std::string &server_name, const std::string &scope);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Methods to get server configs and params. Wrapped around the caches to protect against non existing servers.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
connection_parameters*  get_server_params(const std::string& server_name);
std::string*  get_server_config(const std::string& server_name);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Methods to retrieve authentication information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
curlx509Bundle* get_server_SSL_bundle(const std::string& server_name);
token_info* get_server_token(const std::string& server_name);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Startup methods. Configuration file parser and permission checker.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void parse_settings_cfg(std::string ruciofs_settings_root = "./rucio-settings");
bool check_permissions(const std::string& mountpoint_path);

#endif //RUCIO_FUSE_CONNNECTION_PARAMETERS_H
