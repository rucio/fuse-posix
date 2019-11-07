//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#ifndef RUCIO_FUSE_REST_API_H
#define RUCIO_FUSE_REST_API_H

#include <vector>
#include <string>
#include <map>
#include <utils.h>
#include <unordered_map>
#include <fastlog.h>

void rucio_get_auth_token_userpass(const std::string& short_server_name);
bool rucio_is_token_valid(const std::string& short_server_name);

static std::vector<std::string> servers_cache;
std::vector<std::string> rucio_list_servers();

static std::unordered_map<std::string, std::vector<std::string>> scopes_cache;
std::vector<std::string> rucio_list_scopes(const std::string& short_server_name);

static std::unordered_map<std::string, bool> is_container_cache;
static std::unordered_map<std::string, std::vector<rucio_did>> dids_cache;
std::vector<rucio_did> rucio_list_dids(const std::string& scope, const std::string& short_server_name);

static std::unordered_map<std::string, std::vector<rucio_did>> container_dids_cache;
std::vector<rucio_did> rucio_list_container_dids(const std::string& scope, const std::string& container_name, const std::string& short_server_name);

// Returns true if did is container of dataset, false otherwise (for files)
bool rucio_is_container(const rucio_did& did);
bool rucio_is_container(const std::string& path);

void rucio_create_inode();

#endif //RUCIO_FUSE_REST_API_H
