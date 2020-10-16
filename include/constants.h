/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Copyright European Organization for Nuclear Research (CERN)
Licensed under the Apache License, Version 2.0 (the "License");
You may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
Authors:
 - Gabriele Gaetano Fronzé, <gfronze@cern.ch>, 2019-2020
/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef RUCIO_FUSE_POSIX_CONSTANTS_H
#define RUCIO_FUSE_POSIX_CONSTANTS_H

#include <string>
#include <string.h>
#include <cstddef>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Constants related to data placement and mountpoint
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const std::string rucio_root_path = "/";
static const std::string rucio_cache_path = "/ruciofs-cache";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Constants related to Rucio API calls and responses parsing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const std::string rucio_token_prefix = "X-Rucio-Auth-Token: ";
static const std::string rucio_token_exception_prefix = "ExceptionMessage:";
static const size_t rucio_token_prefix_size = strlen(rucio_token_prefix.data());
static const std::string rucio_invalid_token = ">>>---invalid-token---<<<";

static const std::string rucio_token_duration_prefix = "X-Rucio-Auth-Token-Expires: ";
static const size_t rucio_token_duration_prefix_size = strlen(rucio_token_duration_prefix.data());
static const std::string rucio_default_exp = "Thu, 1 Jan 1970 01:02:03 UTC";

static const std::string rucio_bytes_metadata = "\"bytes\": ";
static const size_t rucio_bytes_metadata_length =  rucio_bytes_metadata.length();

#endif //RUCIO_FUSE_POSIX_CONSTANTS_H
