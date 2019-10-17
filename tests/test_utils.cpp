//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#include <utils.h>
#include <cassert>
#include <iostream>

int main(){
  printf("\nTesting is_root_path:\n");
  std::cout << "--- Testing: / -> " << is_root_path("/") << " expected true\n";
  std::cout << "--- Testing: /scope1 -> " << is_root_path("/scope1") << " expected false\n";
  std::cout << "--- Testing: /scope1/ -> " << is_root_path("/scope1/") << " expected false\n";

  printf("\nTesting is_main_scope:\n");
  std::cout << "--- Testing: /scope1 -> " << is_main_scope("/scope1") << " expected true\n";
  std::cout << "--- Testing: /scope1/ -> " << is_main_scope("/scope1/") << " expected true\n";
  std::cout << "--- Testing: / -> " << is_main_scope("/") << " expected false\n";
  std::cout << "--- Testing: /scope1/name1 -> " << is_main_scope("/scope1/name1") << " expected false\n";

  printf("\nTesting path_depth:\n");
  std::cout << "--- Testing: / -> " << path_depth("/",'/') << " expected 0\n";
  std::cout << "--- Testing: /a -> " << path_depth("/a",'/') << " expected 1\n";
  std::cout << "--- Testing: /a/ -> " << path_depth("/a/",'/') << " expected 1\n";
  std::cout << "--- Testing: /a/b -> " << path_depth("/a/b",'/') << " expected 2\n";
  std::cout << "--- Testing: /a/b/c/d/e/f -> " << path_depth("/a/b/c/d/e/f",'/') << " expected 6\n";
  std::cout << "--- Testing: /a/b/c/d/e/f/ -> " << path_depth("/a/b/c/d/e/f/",'/') << " expected 6\n";

  return 0;
}