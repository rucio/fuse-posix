//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#import <utils.h>
#import <cassert>
#include <iostream>

int main(){
  printf("\nTesting is_root_path:\n");
  std::cout << is_root_path("/") << " expected true\n";
  std::cout << is_root_path("/scope1") << " expected false\n";
  std::cout << is_root_path("/scope1/") << " expected false\n";

  printf("\nTesting is_main_scope:\n");
  std::cout << is_main_scope("/scope1") << " expected true\n";
  std::cout << is_main_scope("/scope1/") << " expected true\n";
  std::cout << is_main_scope("/") << " expected false\n";
  std::cout << is_main_scope("/scope1/name1") << " expected false\n";

  printf("\nTesting path_depth:\n");
  std::cout << path_depth("/",'/') << " expected 0\n";
  std::cout << path_depth("/a",'/') << " expected 1\n";
  std::cout << path_depth("/a/",'/') << " expected 1\n";
  std::cout << path_depth("/a/b",'/') << " expected 2\n";
  std::cout << path_depth("/a/b/c/d/e/f",'/') << " expected 6\n";

  return 0;
}