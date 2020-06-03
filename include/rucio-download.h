//
// Created by Gabriele Gaetano Fronzé on 2020-06-03.
//

#ifndef RUCIO_FUSE_POSIX_RUCIO_DOWNLOAD_H
#define RUCIO_FUSE_POSIX_RUCIO_DOWNLOAD_H

#include <fastlog.h>
#include "download-cache.h"

using namespace fastlog;

#define FILE_NOT_FOUND 42
#define MAX_ATTEMPTS 3
#define TOO_MANY_ATTEMPTS 314

int rucio_download_wrapper(const std::string& tmp_path, const std::string& cache_path, const std::string& did){
  //TODO: using rucio download directly, prevents from being able to connect to multiple rucio servers at once

  std::string command = "rucio download --dir " + tmp_path + " " + did;
  system(command.data());

  fastlog(DEBUG,"Checking downloaded file...");
  FILE* file = fopen(tmp_path.data(), "rb");

  if(not file){
    fastlog(ERROR, "Failed file download! Passing over...");
    return FILE_NOT_FOUND;
  } else {
    fastlog(DEBUG, "Download OK! Renaming temporary file...");
    std::rename(tmp_path.data(), cache_path.data());
  }

  fastlog(DEBUG,"Adding to cache file downloaded at %s.", cache_path.data());
  rucio_download_cache.add_file(cache_path, file);

  return 0;
}

struct rucio_download_info{
    std::string ftmp_path, fcache_path, fdid;
    int freturn_code = 0;
    uint fattempt = 0;
    bool fdownloaded = false;

    rucio_download_info(const std::string& cache_path, std::string did) :
      fcache_path(cache_path),
      ftmp_path(cache_path+".download"),
      fdid(std::move(did)){}

    std::string print(){
      if(fdownloaded){
        return "Did " + fdid + " downloaded at " + fcache_path;
      } else {
        return "Did " + fdid + " download FAILED!";
      }
    }
};

rucio_download_info* rucio_download_wrapper(rucio_download_info& info){
  if (info.fattempt <= MAX_ATTEMPTS) {
    info.fattempt++;
    info.freturn_code = rucio_download_wrapper(info.ftmp_path, info.fcache_path, info.fdid);
    info.fdownloaded = (info.freturn_code != FILE_NOT_FOUND);
  } else {
    info.freturn_code = TOO_MANY_ATTEMPTS;
    info.fdownloaded = false;
  }
  return &info;
}

#endif //RUCIO_FUSE_POSIX_RUCIO_DOWNLOAD_H
