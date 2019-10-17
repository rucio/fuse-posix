//
// Created by Gabriele Gaetano Fronzé on 2019-10-17.
//

#include <globals.h>

std::string rucio_server_url = "https://rucio-server";

std::string rucio_account_name = "root";
std::string rucio_user_name = "ddmlab";
std::string rucio_password = "secret";

const std::string rucio_token_prefix = "X-Rucio-Auth-Token: ";
const size_t rucio_token_prefix_size = strlen(rucio_token_prefix.c_str());

std::string rucio_conn_token = "";
std::chrono::steady_clock::time_point rucio_conn_token_exp;