//
// Created by Gabriele Gaetano Fronzé on 2020-06-03.
//

#ifndef RUCIO_FUSE_POSIX_RUCIO_DOWNLOAD_H
#define RUCIO_FUSE_POSIX_RUCIO_DOWNLOAD_H

#include <fastlog.h>
#include "download-cache.h"
#include "constants.h"
#include "utils.h"

using namespace fastlog;

// Error return codes definition
#define FILE_NOT_FOUND 42
#define MAX_ATTEMPTS 3
#define TOO_MANY_ATTEMPTS 314
#define SETTINGS_NOT_FOUND 1717

// Wrapper around rucio download called as bash command
int rucio_download_wrapper(const std::string& server_name, const std::string& scope, const std::string& name){
  auto server_config_file = get_server_params(server_name)->ca_path;
  auto cache_path = rucio_cache_path + "/" + server_name + "/" + scope;
  auto file_path = cache_path + "/" + name;
  FILE* file = fopen(file_path.data(), "rb");

  // If file is found populate the cache with the local entry
  if (file){
    fastlog(INFO,"File %s already there!",file_path.data());

  // Otherwise download the file using rucio and populate the cache
  } else {
    fastlog(DEBUG, "Downloading at %s...", cache_path.data());
    FILE *settings = fopen(server_config_file.data(), "r");

    // Abort with correct exit code if the server config is not found
    if (not settings) {
      fastlog(ERROR, "Server config file not found at %s. Aborting!", server_config_file.data());
      fclose(settings);
      return SETTINGS_NOT_FOUND;
    }

    fclose(settings);

    std::string did = scope + ":" + name;
    std::string command = "rucio --verbose --config " + server_config_file + " download --no-subdir --dir " + cache_path + " " + did;
    fastlog(DEBUG, "Executing: %s", command.data());
    fastlog(DEBUG, "Downloading to: %s", file_path.data());
    system(command.data());

    fastlog(DEBUG, "Checking downloaded file...");
    file = fopen(file_path.data(), "rb");

    // Check downloaded file and use it to populate the cache
    if (not file) {
      fastlog(ERROR, "Failed file download! Passing over...");
      return FILE_NOT_FOUND;
    } else {
      fastlog(DEBUG, "Download OK!");
    }

    fastlog(DEBUG, "Adding to cache file downloaded at %s", file_path.data());
  }

  rucio_download_cache.add_file(file_path, file);
  return 0;
}

// Utility struct to serve as enqueue-able download request descriptor
struct rucio_download_info{
    std::string fserver_name;
    std::string* fserver_config;
    std::string fdid;
    std::string::size_type fpos;
    int freturn_code = 0;
    unsigned int fattempt = 0;
    bool fdownloaded = false;

    explicit rucio_download_info(std::string did, const std::string& path) :
      fdid(std::move(did)),
      fserver_name(extract_server_name(path)){
      fpos = fdid.find_first_of(':');
      fserver_config = &rucio_server_map[fserver_name].config_file_path;
      fastlog(DEBUG, "Download info added with server name %s and settings at %s", fserver_name.data(), fserver_config->data());
    }

    std::string print(){
      if(fdownloaded){
        return "Did " + fdid + " downloaded at " + full_cache_path();
      } else {
        return "Did " + fdid + " download FAILED!";
      }
    }

    std::string scopename(){
      return fdid.substr(0, fpos);
    }

    std::string filename(){
      return fdid.substr(fpos+1);
    }

    std::string full_cache_path(){
      return rucio_cache_path + "/" + this->fserver_name + "/" + this->scopename() + "/" + this->filename();
    }
};

// Alternate rucio download wrapper which ingests a rucio download info object. Useful for download pipeline use.
rucio_download_info* rucio_download_wrapper(rucio_download_info& info){
  if (info.fattempt <= MAX_ATTEMPTS and info.freturn_code != SETTINGS_NOT_FOUND) {
    info.fattempt++;
    info.freturn_code = rucio_download_wrapper(info.fserver_name, info.scopename(), info.filename());
    info.fdownloaded = (info.freturn_code != FILE_NOT_FOUND and info.freturn_code != SETTINGS_NOT_FOUND);
  } else {
    info.freturn_code = TOO_MANY_ATTEMPTS;
    info.fdownloaded = false;
  }
  return &info;
}

#endif //RUCIO_FUSE_POSIX_RUCIO_DOWNLOAD_H
