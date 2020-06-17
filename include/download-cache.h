//
// Created by Gabriele Gaetano Fronzé on 2020-06-03.
//

#ifndef RUCIO_FUSE_POSIX_DOWNLOAD_CACHE_H
#define RUCIO_FUSE_POSIX_DOWNLOAD_CACHE_H

#include <unordered_map>

// This struct implements a did to local cache path mapping.
// It also keeps open files pointer alive and performs cleanup at end of execution.
struct file_cache {
    std::unordered_map<std::string, FILE*> cache;

    //TODO-or-NOT: here we can populate the cache at startup parsing all files in the cache root.
    file_cache() = default;

    ~file_cache(){
      for(auto &file : cache){
        if(file.second){
          fclose(file.second);
        }
      }
    }

    bool is_cached(const std::string& key){
      return cache.find(key) != cache.end();
    }

    FILE* get_file(const std::string& key){
      return (is_cached(key))?cache[key]:nullptr;
    }

    bool add_file(const std::string& key, FILE* file){
      cache[key] = file;
      return true;
    }

    bool add_file(std::string key){
      cache[key] = fopen(key.data(), "rb");
      return true;
    }
};

static file_cache rucio_download_cache;

#endif //RUCIO_FUSE_POSIX_DOWNLOAD_CACHE_H
