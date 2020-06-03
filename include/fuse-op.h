/*
Created by Gabriele Gaetano Fronzé on 2019-10-16.
Authors:
- Gabriele Gaetano Fronzé <gabriele.fronze at to.infn.it>, 2019
- Vivek Nigam <viveknigam.nigam3@gmail.com>, 2020
*/

#ifndef RUCIO_FUSE_POSIX_OP_H
#define RUCIO_FUSE_POSIX_OP_H

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <REST-API.h>
#include <utils.h>
#include <string.h>
#include <iostream>
#include <fastlog.h>

#include "constants.h"
#include "download-cache.h"
#include "rucio-download.h"

using namespace fastlog;

static int rucio_getattr (const char *path, struct stat *st){
  fastlog(DEBUG,"rucio_getattr called");
  fastlog(INFO,"Handling this path: %s", path);

  st->st_uid = getuid();
	st->st_gid = getgid();
	st->st_atime = time( nullptr );
	st->st_mtime = time( nullptr );

  if (is_hidden(path)) return 0;
	// If it is the root path list the connected servers
	if ( is_root_path(path) ) {
	  fastlog(DEBUG,"handling root path");
	  auto servers = rucio_list_servers();

		st->st_mode = S_IFDIR | 0755;
		st->st_nlink = 2 + servers.size();
  // If it is a server mountpoint render the inner scopes
	} else if (is_server_mountpoint(path)) {
    fastlog(DEBUG,"handling server path");
    std::string server_short_name = extract_server_name(path);
    auto scopes = rucio_list_scopes(server_short_name);

    st->st_mode = S_IFDIR | 0755;
    st->st_nlink = 2 + scopes.size();
  // If is one of the top level scopes list the dids inside
  } else if (is_main_scope(path)) {
    fastlog(DEBUG,"handling scope path");
    std::string server_short_name = extract_server_name(path);
    auto dids = rucio_list_dids(extract_scope(path), server_short_name);

    st->st_mode = S_IFDIR | 0755;
    st->st_nlink = 2 + dids.size();
  // Otherwise, if a container (or a dataset), list its dids
  } else if (rucio_is_container(path)) {
    fastlog(DEBUG,"handling container path");
    std::string server_short_name = extract_server_name(path);
    auto container_dids = rucio_list_container_dids(extract_scope(path), extract_name(path), server_short_name);

    fastlog(DEBUG,"DiDs: %d", container_dids.size());
    auto nFiles = std::count_if(container_dids.begin(),container_dids.end(),[](const rucio_did& did){ return did.type == rucio_data_type::rucio_file; });
    fastlog(DEBUG,"Files: %d", nFiles);
    fastlog(DEBUG,"Containers/datasets: %d", container_dids.size() - nFiles);

    st->st_mode = S_IFDIR | 0755;
    st->st_nlink = 2 + container_dids.size() - nFiles;
  // If it's a file just create the inode
  } else {
    fastlog(DEBUG,"handling file path");
    st->st_mode = S_IFREG | 0644;
    st->st_nlink = 1;
    st->st_size = 1024;
  }

	return 0;
}

static int rucio_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
  fastlog(DEBUG,"rucio_readdir called");
  fastlog(INFO,"Handling this path: %s", path);

  filler(buffer, ".", nullptr, 0 );
  filler(buffer, "..", nullptr, 0 );

  if (is_hidden(path)) return 0;
  
  if ( is_root_path(path) )	{
    fastlog(DEBUG,"handling root path");
    auto servers = rucio_list_servers();

    for(auto const& server : servers){
      filler(buffer, server.data(), nullptr, 0 );
    }

    return 0;
	}	else {
	  std::string server_short_name = extract_server_name(path);

	  if (is_server_mountpoint(path)) {
	    fastlog(DEBUG,"handling server path");
	    auto scopes = rucio_list_scopes(server_short_name);

      for(auto const& scope : scopes){
        filler(buffer, scope.data(), nullptr, 0 );
      }

      return 0;
	  } else {
      // If is one of the top level scopes list the dids inside
      if (is_main_scope(path)) {
        fastlog(DEBUG,"handling scope path");
        auto dids = rucio_list_dids(extract_scope(path), server_short_name);

        for(auto const& did : dids){
          filler(buffer, did.name.data(), nullptr, 0 );
        }

        return 0;
      // Otherwise, if a container (or a dataset), list its dids
      } else if (rucio_is_container(path)) {
        fastlog(DEBUG,"handling container path");
        auto container_dids = rucio_list_container_dids(extract_scope(path), extract_name(path), server_short_name);

        fastlog(DEBUG,"DiDs: %d", container_dids.size());
        auto nFiles = std::count_if(container_dids.begin(),container_dids.end(),[](const rucio_did& did){ return did.type == rucio_data_type::rucio_file; });
        fastlog(DEBUG,"Files: %d", nFiles);
        fastlog(DEBUG,"Containers/datasets: %d", container_dids.size() - nFiles);

        //TODO: Handle dids taking care of loops!
        for(auto const& did : container_dids){
          filler(buffer, did.name.data(), nullptr, 0 );
        }

        return 0;
      }
    }
	}
  return 0;
}

static int rucio_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi)
{
  fastlog(DEBUG,"rucio_read called");
  fastlog(INFO,"Handling this path: %s", path);

  if(not is_server_mountpoint(path) and not is_main_scope(path) and not rucio_is_container(path)){
    auto server_name = extract_server_name(path);
    auto did = get_did(path);
    std::string cache_root = rucio_cache_path + "/" + server_name;
    std::string cache_path = cache_root + "/" + did;
    std::string tmp_path = cache_path + ".download";

    if(not rucio_download_cache.is_cached(cache_path)) {
      fastlog(DEBUG,"File %s @ %s is not cached. Downloading...", did.data(), server_name.data());

      auto return_code = rucio_download_wrapper(tmp_path, cache_path, did);

      if (return_code == FILE_NOT_FOUND){
        return -ENOENT;
      }
    } else {
      fastlog(DEBUG,"File %s @ %s found in cache!", did.data(), server_name.data());
    }

    fastlog(DEBUG,"Getting file...");
    FILE* file = rucio_download_cache.get_file(cache_path);

    fastlog(DEBUG,"Seeking file...");
    fseek(file, offset, SEEK_SET);

    memcpy(buffer, file, size);
  }

  return -1;
}

#endif //RUCIO_FUSE_POSIX_OP_H
