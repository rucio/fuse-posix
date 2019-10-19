//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#ifndef RUCIO_FUSE_UTILS_H
#define RUCIO_FUSE_UTILS_H

#include <string>
#include <vector>
#include <array>
#include <map>
#include <sstream>
#include <algorithm>

std::string to_string(char* contents, size_t size);

// Returns true if and only if path is root path
bool is_root_path(const char *path);

// This function returns -1 if path contains no token
// 0 if path is root
// POSIX format depth in other cases
int path_depth(const char *path, char token = '/');

// Returns true if the path is in the form /rucio/server1 or /rucio/server1/
bool is_server_mountpoint(const char *path);

// This function returns true is the depth is 2 (e.g. /rucio/server1/scope1 or /rucio/server1/scope1/)
bool is_main_scope(const char *path);

template<class T>
void tokenize_python_list(std::string list, std::vector<T>& target, char separator = ',', std::array<char,2> unwanted_chars = {'"', ' '}){
  std::string list_copy = list;
  std::string element;

  list_copy.erase(0, 1);
  list_copy.erase(list_copy.size() - 1);
  std::stringstream stream(list_copy.c_str());
  while (getline(stream, element, separator)) {
    for(const auto& ch : unwanted_chars){
      element.erase(std::remove(element.begin(), element.end(), ch), element.end());
    }
    target.emplace_back((T)element);
  }
}

enum rucio_data_type{
  rucio_file,
  rucio_container,
  rucio_dataset
};

struct rucio_did{
  std::string scope;
  rucio_data_type type;
  std::string name;
  std::string parent;
  int level;
};

void structurize_did(const std::string& did_str, std::vector<rucio_did>& target);
#endif //RUCIO_FUSE_UTILS_H
