//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#ifndef RUCIO_FUSE_CONNNECTION_PARAMETERS_H
#define RUCIO_FUSE_CONNNECTION_PARAMETERS_H

#include <curl/curl.h>
#include <string>
#include <chrono>

static std::string rucio_server_url = "https://rucio-server";

static std::string rucio_account_name = "root";
static std::string rucio_user_name = "dmlab";
static std::string rucio_password = "secret";

static std::string rucio_conn_token_header = "";
std::chrono::steady_clock::time_point rucio_conn_token_exp;

#endif //RUCIO_FUSE_CONNNECTION_PARAMETERS_H
