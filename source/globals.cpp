//
// Created by Gabriele Gaetano Fronzé on 2019-10-17.
//

#include <globals.h>

std::string rucio_server_url = "https://rucio-server";

std::string rucio_account_name = "root";
std::string rucio_user_name = "ddmlab";
std::string rucio_password = "secret";

std::string rucio_conn_token = rucio_invalid_token;
std::chrono::steady_clock::time_point rucio_conn_token_exp;