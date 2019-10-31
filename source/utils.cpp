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

void remove_trailing_token(std::string& path, std::string token){
  if(path.length() - 1 > 0) {
    if (path.substr(path.length() - 1) == token) {
      path.pop_back();
    }
  }
}

std::string remove_substring(const std::string& path, const std::string& subs){
  auto path_copy = path;

  // Search for the rucio root path with trailing "/"
	size_t pos = path_copy.find(subs);

	if (pos != std::string::npos)
	{
    // Erase root path from string
		path_copy.erase(pos, subs.length());
	} else return "";

	return path_copy;
}

std::string remove_root_path(const std::string& path){
  return remove_substring(path, rucio_root_path+'/');
}

std::string extract_server_name(const std::string& path){
  auto path_copy = path;
  remove_trailing_token(path_copy);

	// Find position of first "/"
	size_t pos = path_copy.find('/');

  if (pos != std::string::npos)
	{
    // Erase everything after the first "/"
		path_copy.erase(pos, path_copy.length());
	} else return "";

  return std::move(path_copy);
}

std::string extract_scope(const std::string& path){
  auto path_copy = path;
  remove_trailing_token(path_copy);
  path_copy = remove_substring(path_copy, extract_server_name(path)+'/');

  size_t pos = path_copy.find('/');

  if (pos != std::string::npos)
	{
    // Erase everything after the first "/"
		path_copy.erase(pos, path_copy.length());
	} else return "";

  return std::move(path_copy);
}

std::string extract_name(const std::string& path){
  auto path_copy = path;
  remove_trailing_token(path_copy);

  size_t pos = path_copy.find_last_of('/');

  if (pos != std::string::npos)
	{
    // Erase everything after the first "/"
		path_copy.erase(0, pos+1);
	} else return "";

  return std::move(path_copy);
}

std::string get_did(const std::string& path){
  return extract_scope(path)+":"+extract_name(path);
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
