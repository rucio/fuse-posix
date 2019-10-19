//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#include <utils.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <constants.h>


std::string to_string(char* contents, size_t size){
    std::string s;
    for (size_t i = 0; i < size; i++) {
        s = s + contents[i];
    }
    return s;
}

// Returns true if and only if path is root path
bool is_root_path(const char *path){
  return (strcmp(path, rucio_root_path.c_str()) == 0 || strcmp(path, (rucio_root_path+"/").c_str()) == 0);
}



// This function returns -1 if path contains no token
// 0 if path is root
// POSIX format depth in other cases
int path_depth(const char *path, const char token){
  // If path is root return depth=0 immediately
  if(is_root_path(path)) {
    return 0;
  }
  // Otherwise compute depth
  else {
    // Count token occurrences
    auto s = path;
    int16_t i = 0;
    size_t size = 0;
    for (i = 0; s[i];){
      size++;
      if(s[i] == token) i++;
      else s++;
    }

    // Remove one to count if just the last char is equal to token (spurious token)
    // '/scope1' and '/scope1/' should behave the same
    if(path[size-1] == token) i--;
    // Returns depth
    return i-1;
  }
}

bool is_server_mountpoint(const char *path){
  return path_depth(path) == 1;
};

// This function returns true is the depth is 1 (e.g. /scope1 or /scope1/)
bool is_main_scope(const char *path){
  return path_depth(path) == 2;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string key_or_value;
    while(std::getline(ss, key_or_value, delim)) {
      elems.emplace_back(key_or_value);
    }
    return elems;
}

void structurize_did(const std::string& did_str,
                     std::vector<rucio_did>& target){
  std::string list_copy = did_str;

  for(const auto& ch : {' ','}','{','"'}){
    list_copy.erase(std::remove(list_copy.begin(), list_copy.end(), ch), list_copy.end());
  }

  std::replace(list_copy.begin(), list_copy.end(), ':', ' ');
  std::replace(list_copy.begin(), list_copy.end(), ',', ' ');

  auto key_values = split(list_copy, ' ');
  rucio_did did;

  did.scope = key_values[1];

  if(key_values[3] == "FILE"){
    did.type = rucio_data_type::rucio_file;
  }else if(key_values[3] == "CONTAINER"){
    did.type = rucio_data_type::rucio_container;
  }else if(key_values[3] == "DATASET"){
    did.type = rucio_data_type::rucio_dataset;
  }

  did.name = key_values[5];
  did.parent = key_values[7];
  did.level = std::atoi(key_values[9].c_str());

  target.emplace_back(did);
}
