/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Copyright European Organization for Nuclear Research (CERN)
Licensed under the Apache License, Version 2.0 (the "License");
You may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
Authors:
 - Gabriele Gaetano Fronzé, <gfronze@cern.ch>, 2019-2020
 - Vivek Nigam <viveknigam.nigam3@gmail.com>, 2020
/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
#include <algorithm>

#include "constants.h"
#include "globals.h"
#include "download-cache.h"
#include "rucio-download.h"
#include "download-pipeline.h"

using namespace fastlog;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is the method which renders the posix namespace and set the file/dir permissions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int rucio_getattr (const char *path, struct stat *st){
  if (is_hidden(path)) return 0;

  if (is_mac_specific(path)) return 0;

  fastlog(DEBUG,"rucio_getattr called");
  fastlog(DEBUG,"Handling this path: %s", path);

  st->st_uid = getuid();
	st->st_gid = getgid();
	st->st_atime = time( nullptr );
	st->st_mtime = time( nullptr );

	// If it is the root path list the connected servers
	if ( is_root_path(path) ) {
	  fastlog(DEBUG,"handling root path");
		st->st_mode = S_IFDIR | 0755;
		st->st_nlink = 2 + rucio_list_servers().size();

  // If it is a server mountpoint render the inner scopes
	} else if (is_server_mountpoint(path)) {
    fastlog(DEBUG,"handling server path");
    std::string server_short_name = extract_server_name(path);

    if(not server_exists(server_short_name)){
      fastlog(DEBUG, "Server %s doesn't exist.", server_short_name.data());
      return -ENOENT;
    }

    auto scopes = rucio_list_scopes(server_short_name);
    st->st_mode = S_IFDIR | 0755;
    st->st_nlink = 2 + scopes.size();

  // If is one of the top level scopes list the dids inside
  } else if (is_main_scope(path)) {
    fastlog(DEBUG,"handling scope path");
    std::string server_short_name = extract_server_name(path);
    std::string scope = extract_scope(path);

    if(not scope_exists(server_short_name, scope)){
      fastlog(DEBUG, "Scope %s at server %s doesn't exist.", scope.data(), server_short_name.data());
      return -ENOENT;
    }

    auto dids = rucio_list_dids(scope, server_short_name);
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
    st->st_size = rucio_get_size(path);
  }
	return 0;
}

static int finalize_filler(void *buffer, fuse_fill_dir_t filler){
  filler(buffer, ".", nullptr, 0 );
  filler(buffer, "..", nullptr, 0 );
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is the method called when using cd
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int rucio_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
  fastlog(DEBUG,"rucio_readdir called");
  fastlog(INFO,"Handling this path: %s", path);

  if (is_hidden(path)) return 0;

  // At root path just render all the servers
  if (is_root_path(path))	{
    fastlog(DEBUG,"handling root path");
    auto servers = rucio_list_servers();

    for(auto const& server : servers){
      filler(buffer, server.data(), nullptr, 0 );
    }
    return finalize_filler(buffer, filler);

	}	else {
	  std::string server_short_name = extract_server_name(path);

	  // At server mountpoint render all the scopes
	  if (is_server_mountpoint(path)) {
	    fastlog(DEBUG,"handling server path");

	    auto scopes = rucio_list_scopes(server_short_name);

      for(auto const& scope : scopes){
        filler(buffer, scope.data(), nullptr, 0 );
      }
      return finalize_filler(buffer, filler);

	  } else {
      // If is one of the top level scopes list the dids inside
      if (is_main_scope(path)) {
        fastlog(DEBUG,"handling scope path");
        auto dids = rucio_list_dids(extract_scope(path), server_short_name);

        for(auto const& did : dids){
          filler(buffer, did.name.data(), nullptr, 0 );
        }
        return finalize_filler(buffer, filler);

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
        return finalize_filler(buffer, filler);
      }
    }
	}
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This method is called when a file entity is opened
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int rucio_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi)
{
  fastlog(DEBUG,"rucio_read called");
  fastlog(INFO,"Handling this path: %s", path);

  // If path is not a directory hendle the file
  if(not is_server_mountpoint(path) and not is_main_scope(path) and not rucio_is_container(path)){
    auto server_name = extract_server_name(path);
    auto did = get_did(path);
    std::string cache_root = rucio_cache_path + "/" + server_name + "/" + extract_scope(path);
    std::string cache_path = cache_root + "/" + extract_name(path);

    // Check if file has been downloaded already and cached
    if(not rucio_download_cache.is_cached(cache_path)) {

      auto ctx = fuse_get_context();

      // If file is downloading avoid enqueue-ing it again
      if(is_downloading(path)){
        fastlog(INFO, "File %s @ %s is not cached and already downloading!", did.data(), server_name.data());
//        printToPID(ctx->pid, "\nFile "+did+" @ "+server_name+" is not cached and already downloading!\n");
        return -EINPROGRESS;

      // Otherwise download it
      } else {
        fastlog(DEBUG, "File %s @ %s is not cached. Downloading...", did.data(), server_name.data());
//        printToPID(ctx->pid, "\nFile "+did+" @ "+server_name+" is not cached. Download started...\n");
        // If not downloaded yet, download file appending its infos to the download jobs queue
        rucio_download_pipeline.append_new_download(rucio_download_info(did, path));
        set_downloading(path);
        // Notify the file is not there (yet)
        return -EAGAIN;
      }
    } else {
      fastlog(DEBUG,"File %s @ %s found in cache!", did.data(), server_name.data());
      set_downloaded(path);
    }

    // Getting the file from cache and retrieving its size
    // TODO: cache file sizes!
    fastlog(DEBUG,"Getting file...");
    FILE* file = rucio_download_cache.get_file(cache_path);
    auto file_size = rucio_get_size(path);

    fastlog(DEBUG,"Seeking file...");

    // Avoid going over end of file
    if (offset > file_size) return 0;

    // Apply offset to file and fread into buffer the requested number of bytes
    fseek(file, offset, SEEK_SET);
    fread(buffer, sizeof(char), size, file);
    return std::min(size , (unsigned long)(file_size - offset));
  }
  return -1;
}

#endif //RUCIO_FUSE_POSIX_OP_H
