/*
Created by Gabriele Gaetano Fronzé on 2019-10-16.
Authors:
- Gabriele Gaetano Fronzé <gabriele.fronze at to.infn.it>, 2019
- Vivek Nigam <viveknigam.nigam3@gmail.com>, 2020
*/

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

//Returns true if file is hidden.
bool is_hidden(const std::string &path);

//Returns true if file is Mac system file.
bool is_mac_specific(const std::string &path);

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

void remove_trailing_token(std::string& path, std::string token = "/");
void remove_leading_token(std::string& path, std::string token = "/");
std::string remove_substring(const std::string& path, const std::string& subs);
std::string remove_root_path(const std::string& path);
std::string extract_server_name(const std::string& path);
std::string extract_scope(const std::string& path);
std::string extract_name(const std::string& path);
std::string get_did(const std::string& path);

void split_dids(const std::string &line, std::vector<std::string>& did_strings);

static std::string did_string_remainder;
void coherentize_dids(std::string &did_string);

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
void structurize_container_did(const std::string& did_str, std::vector<rucio_did>& target);
#endif //RUCIO_FUSE_UTILS_H
