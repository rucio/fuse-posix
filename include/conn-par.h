//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#ifndef RUCIO_FUSE_CONNNECTION_PARAMETERS_H
#define RUCIO_FUSE_CONNNECTION_PARAMETERS_H

#include <curl/curl.h>
#include <string>
#include <chrono>

static std::string rucio_server_url = "";
static int rucio_server_port = 80;

static std::string rucio_account_name = "";
static std::string rucio_user_name = "";
static std::string rucio_password = "";

static std::string rucio_conn_token_header = "";
std::chrono::steady_clock::time_point rucio_conn_token_exp;

#endif //RUCIO_FUSE_CONNNECTION_PARAMETERS_H
