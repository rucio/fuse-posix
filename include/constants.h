//
// Created by Gabriele Gaetano Fronzé on 2019-10-18.
//

#ifndef RUCIO_FUSE_POSIX_CONSTANTS_H
#define RUCIO_FUSE_POSIX_CONSTANTS_H

#include <string>
#include <string.h>
#include <cstddef>

// FS structure constants
static const std::string rucio_root_path = "/rucio";

// Token parsing constants
static const std::string rucio_token_prefix = "X-Rucio-Auth-Token: ";
static const size_t rucio_token_prefix_size = strlen(rucio_token_prefix.c_str());
static const std::string rucio_invalid_token = ">>>---invalid-token---<<<";

static const std::string rucio_token_duration_prefix = "X-Rucio-Auth-Token-Expires: ";
static const size_t rucio_token_duration_prefix_size = strlen(rucio_token_duration_prefix.c_str());
static const std::string rucio_default_exp = "Thu, 1 Jan 1970 01:02:03 UTC";

#endif //RUCIO_FUSE_POSIX_CONSTANTS_H
