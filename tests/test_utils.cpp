//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#include <utils.h>
#include <cassert>
#include <iostream>

int main(){
  printf("\nTesting is_root_path:\n");
  std::cout << "--- Testing: /rucio -> " << is_root_path("/rucio") << " expected true\n";
  std::cout << "--- Testing: /rucio/ -> " << is_root_path("/rucio/") << " expected true\n";
  std::cout << "--- Testing: /rucio/a -> " << is_root_path("/rucio/a") << " expected false\n";
  std::cout << "--- Testing: /rucio/a/ -> " << is_root_path("/rucio/a/") << " expected false\n";
  std::cout << "--- Testing: /rucio/a/b -> " << is_root_path("/rucio/a/b") << " expected false\n";

  printf("\nTesting path_depth:\n");
  std::cout << "--- Testing: /rucio/ -> " << path_depth("/rucio/",'/') << " expected 0\n";
  std::cout << "--- Testing: /rucio/a -> " << path_depth("/rucio/a",'/') << " expected 1\n";
  std::cout << "--- Testing: /rucio/a/ -> " << path_depth("/rucio/a/",'/') << " expected 1\n";
  std::cout << "--- Testing: /rucio/a/b -> " << path_depth("/rucio/a/b",'/') << " expected 2\n";
  std::cout << "--- Testing: /rucio/a/b/c/d/e/f -> " << path_depth("/rucio/a/b/c/d/e/f",'/') << " expected 6\n";
  std::cout << "--- Testing: /rucio/a/b/c/d/e/f/ -> " << path_depth("/rucio/a/b/c/d/e/f/",'/') << " expected 6\n";

  printf("\nTesting is_server_mountpoint:\n");
  std::cout << "--- Testing: /rucio/server1 -> " << is_server_mountpoint("/rucio/server1") << " expected true\n";
  std::cout << "--- Testing: /rucio/server1/ -> " << is_server_mountpoint("/rucio/server1/") << " expected true\n";
  std::cout << "--- Testing: /rucio -> " << is_server_mountpoint("/rucio/") << " expected false\n";
  std::cout << "--- Testing: /rucio/server1/scope1 -> " << is_server_mountpoint("/rucio/server1/scope1") << " expected false\n";
  std::cout << "--- Testing: /rucio/server1/scope1/ -> " << is_server_mountpoint("/rucio/server1/scope1/") << " expected false\n";
  std::cout << "--- Testing: /rucio/server1/scope1/name1 -> " << is_server_mountpoint("/rucio/server1/scope1/name1") << " expected false\n";

  printf("\nTesting is_main_scope:\n");
  std::cout << "--- Testing: /rucio/server1 -> " << is_main_scope("/rucio/server1") << " expected false\n";
  std::cout << "--- Testing: /rucio/server1/ -> " << is_main_scope("/rucio/server1/") << " expected false\n";
  std::cout << "--- Testing: /rucio -> " << is_main_scope("/rucio/") << " expected false\n";
  std::cout << "--- Testing: /rucio/server1/scope1 -> " << is_main_scope("/rucio/server1/scope1") << " expected true\n";
  std::cout << "--- Testing: /rucio/server1/scope1/ -> " << is_main_scope("/rucio/server1/scope1/") << " expected true\n";
  std::cout << "--- Testing: /rucio/server1/scope1/name1 -> " << is_main_scope("/rucio/server1/scope1/name1") << " expected false\n";

  printf("\nTesting tokenize_python_list:\n");
  std::vector<std::string> expected_scopes = {"a","b","c","d"};
  std::vector<std::string> scopes;
  tokenize_python_list(R"(["a","b","c","d"])", scopes);
  size_t i = 0;
  for(auto const& scope : scopes)
    std::cout << "--- " << scope << " expected " << expected_scopes[i++] << std::endl;

  printf("\nTesting structurize_did:\n");
  std::string test_did = R"({"scope": "user.root", "type": "FILE", "name": "test-19102019.txt", "parent": null, "level": 0})";
  std::vector<rucio_did> didv;
  structurize_did(test_did, didv);
  std::cout << "--- " << didv[0].scope << std::endl;
  std::cout << "--- " << didv[0].type << std::endl;
  std::cout << "--- " << didv[0].name << std::endl;
  std::cout << "--- " << didv[0].parent << std::endl;
  std::cout << "--- " << didv[0].level << std::endl;

  printf("\nTesting string manipulation:\n");
  std::string test_string = "/rucio/server/scope/container/dataset/filename/";
  std::cout << "--- Test string: ";
  std::cout << test_string << "->";
  remove_trailing_token(test_string);
  std::cout << test_string << std::endl;

  std::cout << "--- Server name: " << extract_server_name(test_string) << std::endl;
  std::cout << "--- Scope: " << extract_scope(test_string) << std::endl;
  std::cout << "--- File name: " << extract_name(test_string) << std::endl;
  std::cout << "--- DiD name: " << get_did(test_string) << std::endl;


  return 0;
}