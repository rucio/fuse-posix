//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#ifndef RUCIO_FUSE_FUSE_OP_H
#define RUCIO_FUSE_FUSE_OP_H

#include <stddef.h>
#include <stdbool.h>
#include "REST-API.h"

// Returns true if and only if path is root path
bool is_root_path(const char *path);

// This function returns -1 if path contains no token
// 0 if path is root
// POSIX format depth in other cases
size_t path_depth(const char *path, char token = '/');

// This function returns true is the depth is 1 (e.g. /scope1 or /scope1/)
bool is_main_scope(const char *path);

static int rucio_getattr (const char *path, struct stat *st){
  printf("rucio_getattr called");
  if (is_root_path(path)){
    auto scopes = rucio_list_scopes();
    // TODO: Handle the scopes...
  } else {
    if (is_main_scope(path)){
      auto dids = rucio_list_dids(""); //TODO: extract the scope from the path
      //TODO: Handle the dids
    } else if (rucio_is_container(path)){
      auto container_dids = rucio_list_container_dids("",""); //TODO: extract the scope and the name from path
    } else {
      //TODO: crete the did inode
    }
  }
  return 0;
}
//static int rucio_readdir (const char *path, void *buff, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
//static int rucio_read (const char *path, char *buff, size_t size, off_t offset, struct fuse_file_info *fi);

#endif //RUCIO_FUSE_FUSE_OP_H
