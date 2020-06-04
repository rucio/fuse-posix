//
// Created by Gabriele Gaetano Fronzé on 2020-06-03.
//

#ifndef RUCIO_FUSE_POSIX_RUCIO_DOWNLOAD_H
#define RUCIO_FUSE_POSIX_RUCIO_DOWNLOAD_H

#include <fastlog.h>
#include "download-cache.h"
#include "constants.h"

using namespace fastlog;

#define FILE_NOT_FOUND 42
#define MAX_ATTEMPTS 3
#define TOO_MANY_ATTEMPTS 314

int rucio_download_wrapper(const std::string& did, const std::string& scope){
  //TODO: using rucio download directly, prevents from being able to connect to multiple rucio servers at once

  auto cache_path = rucio_cache_path + "/" + scope;
  auto file_path = cache_path + "/" + did;
  FILE* file = fopen(file_path.data(), "rb");

  if (file){
    fclose(file);
    fastlog(INFO,"File %s already there!",file_path.data());
    return 0;
  }

  fastlog(DEBUG,"Downloading at %s...",cache_path.data());

  std::string command = "rucio --verbose download --dir " + cache_path  + " " + did;
  system(command.data());

  fastlog(DEBUG,"Checking downloaded file...");
  file = fopen(file_path.data(), "rb");

  if(not file){
    fastlog(ERROR, "Failed file download! Passing over...");
    return FILE_NOT_FOUND;
  } else {
    fastlog(DEBUG, "Download OK! Renaming temporary file...");
  }

  fastlog(DEBUG,"Adding to cache file downloaded at %s.", file_path.data());
  rucio_download_cache.add_file(file_path, file);

  return 0;
}

struct rucio_download_info{
    std::string fdid;
    std::string::size_type fpos;
    int freturn_code = 0;
    unsigned int fattempt = 0;
    bool fdownloaded = false;

    explicit rucio_download_info(std::string did) :
      fdid(std::move(did)){
      fpos = fdid.find_first_of(':');
    }

    std::string print(){
      if(fdownloaded){
        return "Did " + fdid + " downloaded at " + rucio_cache_path + "/" + fdid ;
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

    std::string full_path(){
      return rucio_cache_path + "/" + this->scopename() + "/" + this->filename();
    }
};

rucio_download_info* rucio_download_wrapper(rucio_download_info& info){
  if (info.fattempt <= MAX_ATTEMPTS) {
    info.fattempt++;
    info.freturn_code = rucio_download_wrapper(info.fdid, info.scopename());
    info.fdownloaded = (info.freturn_code != FILE_NOT_FOUND);
  } else {
    info.freturn_code = TOO_MANY_ATTEMPTS;
    info.fdownloaded = false;
  }
  return &info;
}

#endif //RUCIO_FUSE_POSIX_RUCIO_DOWNLOAD_H
