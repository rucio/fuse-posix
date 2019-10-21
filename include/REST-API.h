//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#ifndef RUCIO_FUSE_REST_API_H
#define RUCIO_FUSE_REST_API_H

#include <vector>
#include <string>
#include <map>
#include <utils.h>

void rucio_get_auth_token_userpass(const std::string& short_server_name);

bool rucio_is_token_valid(const std::string& short_server_name);

std::string rucio_server_from_path(const std::string& path);

std::vector<std::string> rucio_list_servers();

std::vector<std::string> rucio_list_scopes(const std::string& short_server_name);

std::vector<rucio_did> rucio_list_dids(const std::string& scope, const std::string& short_server_name);

std::vector<rucio_did> rucio_list_container_dids(const std::string& scope, const std::string& container_name, const std::string& short_server_name);

// Returns true if did is container of dataset, false otherwise (for files)
bool rucio_is_container(const rucio_did& did);

void rucio_create_inode();

#endif //RUCIO_FUSE_REST_API_H
