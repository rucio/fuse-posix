//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#include <utils.h>
#include <string>
#include <string.h>
#include <iostream>


std::string to_string(char* contents, size_t size){
    std::string s;
    for (size_t i = 0; i < size; i++) {
        s = s + contents[i];
    }
    return s;
}

// Returns true if and only if path is root path
bool is_root_path(const char *path){
  return (strcmp(path, "/") == 0);
}



// This function returns -1 if path contains no token
// 0 if path is root
// POSIX format depth in other cases
int path_depth(const char *path, const char token){
  // If path is root return depth=0 immediately
  if(strcmp(path, &token) == 0) {
    printf("At root\n");
    return 0;
  }
  // Otherwise compute depth
  else {
    // Count token occurrences
    auto s = path;
    size_t i = 0;
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
    return i;
  }
}

// This function returns true is the depth is 1 (e.g. /scope1 or /scope1/)
bool is_main_scope(const char *path){
  return path_depth(path) == 1;
}
