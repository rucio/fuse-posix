//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#ifndef RUCIO_FUSE_CONNNECTION_PARAMETERS_H
#define RUCIO_FUSE_CONNNECTION_PARAMETERS_H

#include <curl/curl.h>
#include <string>
#include <string.h>
#include <time.h>

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

// Shared connection parameters
extern connection_parameters rucio_connection_parameters;

// Token parsing constants
static const std::string rucio_token_prefix = "X-Rucio-Auth-Token: ";
static const size_t rucio_token_prefix_size = strlen(rucio_token_prefix.c_str());
static const std::string rucio_invalid_token = ">>>---invalid-token---<<<";

static const std::string rucio_token_duration_prefix = "X-Rucio-Auth-Token-Expires: ";
static const size_t rucio_token_duration_prefix_size = strlen(rucio_token_duration_prefix.c_str());
static const std::string rucio_default_exp = "Thu, 1 Jan 1970 01:02:03 UTC";

// Token info struct definition
struct token_info{
  std::string conn_token = rucio_invalid_token;
  tm conn_token_exp;
  time_t conn_token_exp_epoch = 0;
};

// Shared token parameters
extern token_info rucio_token_info;

#endif //RUCIO_FUSE_CONNNECTION_PARAMETERS_H
