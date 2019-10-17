//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#ifndef RUCIO_FUSE_REST_API_H
#define RUCIO_FUSE_REST_API_H

#include <vector>
#include <string>

void rucio_get_auth_token_userpass();

bool rucio_is_token_valid();

std::vector<std::string> rucio_list_scopes();

std::vector<std::string> rucio_list_dids(const std::string& scope);

std::vector<std::string> rucio_list_container_dids(const std::string& scope, const std::string& container_name);

// Returns true if did is container of dataset, false otherwise (for files)
bool rucio_is_container(const std::string& path);

void rucio_create_inode();

#endif //RUCIO_FUSE_REST_API_H
