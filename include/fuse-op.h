//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#ifndef RUCIO_FUSE_FUSE_OP_H
#define RUCIO_FUSE_FUSE_OP_H

#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <REST-API.h>
#include <utils.h>
#include <string.h>
#include <iostream>

static int rucio_getattr (const char *path, struct stat *st){
//  std::cout << "rucio_getattr called\n";
//  std::cout << "Handling this path: " << path <<std::endl;

  st->st_uid = getuid();
	st->st_gid = getgid();
	st->st_atime = time( nullptr );
	st->st_mtime = time( nullptr );

	if ( is_root_path(path) )
	{
//	  std::cout << "handling root path\n";
	  auto servers = rucio_list_servers();

		st->st_mode = S_IFDIR | 0755;
		st->st_nlink = 2 + servers.size();
	}
	else
	{
	  if (is_server_mountpoint(path)) {
//	    std::cout << "handling server path\n";
	    std::string server_short_name = extract_server_name(path);
	    auto scopes = rucio_list_scopes(server_short_name);

      st->st_mode = S_IFDIR | 0755;
		  st->st_nlink = 2 + scopes.size();
	  } else {
//	    std::cout << "handling scope path\n";
      // If is one of the top level scopes list the dids inside
      if (is_main_scope(path)) {
//        std::cout << "handling scope path\n";
        std::string server_short_name = extract_server_name(path);
        auto dids = rucio_list_dids(extract_scope(path), server_short_name);

        st->st_mode = S_IFDIR | 0755;
		    st->st_nlink = 2 + dids.size();
      // Otherwise, if a container (or a dataset), list its dids
      } else if (rucio_is_container(path)) {
        std::cout << "handling container path\n";
        std::string server_short_name = extract_server_name(path);
        auto container_dids = rucio_list_container_dids(extract_scope(path), extract_name(path), server_short_name);

        std::cout << "DiDs: " << container_dids.size() << std::endl;
        auto nFiles = std::count_if(container_dids.begin(),container_dids.end(),[](const rucio_did& did){ return did.type == rucio_data_type::rucio_file; });
        std::cout << "Files: " << nFiles << std::endl;
        std::cout << "Containers/datasets: " << container_dids.size() - nFiles <<std::endl;

        st->st_mode = S_IFDIR | 0755;
		    st->st_nlink = 2 + container_dids.size() - nFiles;
      // If it's a file just create the inode
      } else {
        std::cout << "handling file path\n";
        st->st_mode = S_IFREG | 0644;
        st->st_nlink = 1;
        st->st_size = 1024;
      }
    }
	}

  return 0;
}

static int rucio_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
//  std::cout << "rucio_readdir called\n";
//  std::cout << "Handling this path: " << path <<std::endl;

  filler(buffer, ".", nullptr, 0 );
  filler(buffer, "..", nullptr, 0 );

  if ( is_root_path(path) )	{
//    std::cout << "handling root path\n";
    auto servers = rucio_list_servers();

    for(auto const& server : servers){
      filler(buffer, server.data(), nullptr, 0 );
    }
	}	else {
	  std::string server_short_name = extract_server_name(path);

	  if (is_server_mountpoint(path)) {
//	    std::cout << "handling server path\n";
	    auto scopes = rucio_list_scopes(server_short_name);

      for(auto const& scope : scopes){
        filler(buffer, scope.data(), nullptr, 0 );
      }
	  } else {
      // If is one of the top level scopes list the dids inside
      if (is_main_scope(path)) {
//        std::cout << "handling scope path\n";
        auto dids = rucio_list_dids(extract_scope(path), server_short_name);

        for(auto const& did : dids){
          filler(buffer, did.name.data(), nullptr, 0 );
        }
      // Otherwise, if a container (or a dataset), list its dids
      } else if (rucio_is_container(path)) {
        std::cout << "handling container path\n";
        auto container_dids = rucio_list_container_dids(extract_scope(path), extract_name(path), server_short_name);

        //TODO: Handle dids taking care of loops!
        for(auto const& did : container_dids){
          std::cout << did.name << std::endl;
          filler(buffer, did.name.data(), nullptr, 0 );
        }
      }
    }
	}
  return 0;
}

static int rucio_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi)
{
//  std::cout << "rucio_read called\n";
//  std::cout << "Handling this path: " << path <<std::endl;

  if(not is_server_mountpoint(path) and not is_main_scope(path) and not rucio_is_container(path)){
    //TODO: download file through Rucio
    char *fileContent = "Dummy file content placeholder";
    memcpy(buffer, fileContent + offset, size);
	  return strlen( fileContent ) - offset;
  }

  return -1;
}

#endif //RUCIO_FUSE_FUSE_OP_H
