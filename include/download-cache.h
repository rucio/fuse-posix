//
// Created by Gabriele Gaetano Fronzé on 2020-06-03.
//

#ifndef RUCIO_FUSE_POSIX_DOWNLOAD_CACHE_H
#define RUCIO_FUSE_POSIX_DOWNLOAD_CACHE_H

struct file_cache {
    std::unordered_map<std::string, FILE*> cache;

    file_cache(){
      //TODO: here we can populate the cache at startup parsing all files in the cache root.
    }

    ~file_cache(){
      for(auto &file : cache){
        fclose(file.second);
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
file_cache rucio_download_cache;

#endif //RUCIO_FUSE_POSIX_DOWNLOAD_CACHE_H
