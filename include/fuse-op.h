//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#ifndef RUCIO_FUSE_FUSE_OP_H
#define RUCIO_FUSE_FUSE_OP_H

#include <stddef.h>
#include <stdbool.h>
#include <REST-API.h>
#include <utils.h>

// This is where the rucio-compliant logic branching happens
static int rucio_getattr (const char *path, struct stat *st){
  printf("rucio_getattr called");
  // If at root list the available servers
  if (is_root_path(path)){
    auto servers = rucio_list_servers();
    //TODO: Render the available servers as directories
  } else {
    std::string server_short_name = rucio_server_from_path(path);
    // If at a server mountpoint just list the available scopes
    if (is_server_mountpoint(path)) {
      auto scopes = rucio_list_scopes(server_short_name);
      // TODO: Handle the server scopes
    }
      // If is one of the top level scopes list the dids inside
      if (is_main_scope(path)) {
        auto dids = rucio_list_dids("", server_short_name); //TODO: extract the scope from the path
        //TODO: Handle the dids
      // Otherwise, if a container (or a dataset), list its dids
      } else if (rucio_is_container(path)) {
        auto container_dids = rucio_list_container_dids("", "", server_short_name); //TODO: extract the scope and the name from path
        //TODO: Handle dids taking care of loops!
      // If it's a file just create the inode
      } else {
        //TODO: create the did inode
      }
    }
  }
  return 0;
}
//static int rucio_readdir (const char *path, void *buff, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
//static int rucio_read (const char *path, char *buff, size_t size, off_t offset, struct fuse_file_info *fi);

#endif //RUCIO_FUSE_FUSE_OP_H
