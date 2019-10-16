//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#ifndef RUCIO_FUSE_REST_API_H
#define RUCIO_FUSE_REST_API_H

#include <vector>
#include <string>

std::vector<std::string> rucio_list_scopes();

std::vector<std::string> rucio_list_dids(const std::string& scope);

std::vector<std::string> rucio_list_container_dids(const std::string& scope, const std::string& container_name);

void rucio_create_inode();

#endif //RUCIO_FUSE_REST_API_H
