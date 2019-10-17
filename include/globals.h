//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#ifndef RUCIO_FUSE_CONNNECTION_PARAMETERS_H
#define RUCIO_FUSE_CONNNECTION_PARAMETERS_H

#include <curl/curl.h>
#include <string>
#include <string.h>
#include <chrono>

extern std::string rucio_server_url;

extern std::string rucio_account_name;
extern std::string rucio_user_name;
extern std::string rucio_password;

static const std::string rucio_token_prefix = "X-Rucio-Auth-Token: ";
static const size_t rucio_token_prefix_size = strlen(rucio_token_prefix.c_str());
static const std::string rucio_invalid_token = ">>>---invalid-token---<<<";

extern std::string rucio_conn_token;
extern std::chrono::steady_clock::time_point rucio_conn_token_exp;

#endif //RUCIO_FUSE_CONNNECTION_PARAMETERS_H
