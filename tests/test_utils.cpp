//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#include <utils.h>
#include <cassert>
#include <iostream>

int main(){
  printf("\nTesting is_root_path:\n");
  std::cout << "--- Testing: / -> " << is_root_path("/") << " expected true\n";
  std::cout << "--- Testing: / -> " << is_root_path("/") << " expected true\n";
  std::cout << "--- Testing: /a -> " << is_root_path("/a") << " expected false\n";
  std::cout << "--- Testing: /a/ -> " << is_root_path("/a/") << " expected false\n";
  std::cout << "--- Testing: /a/b -> " << is_root_path("/a/b") << " expected false\n";

  printf("\nTesting path_depth:\n");
  std::cout << "--- Testing: / -> " << path_depth("/",'/') << " expected 0\n";
  std::cout << "--- Testing: /a -> " << path_depth("/a",'/') << " expected 1\n";
  std::cout << "--- Testing: /a/ -> " << path_depth("/a/",'/') << " expected 1\n";
  std::cout << "--- Testing: /a/b -> " << path_depth("/a/b",'/') << " expected 2\n";
  std::cout << "--- Testing: /a/b/c/d/e/f -> " << path_depth("/a/b/c/d/e/f",'/') << " expected 6\n";
  std::cout << "--- Testing: /a/b/c/d/e/f/ -> " << path_depth("/a/b/c/d/e/f/",'/') << " expected 6\n";

  printf("\nTesting is_server_mountpoint:\n");
  std::cout << "--- Testing: /server1 -> " << is_server_mountpoint("/server1") << " expected true\n";
  std::cout << "--- Testing: /server1/ -> " << is_server_mountpoint("/server1/") << " expected true\n";
  std::cout << "--- Testing: / -> " << is_server_mountpoint("/") << " expected false\n";
  std::cout << "--- Testing: /server1/scope1 -> " << is_server_mountpoint("/server1/scope1") << " expected false\n";
  std::cout << "--- Testing: /server1/scope1/ -> " << is_server_mountpoint("/server1/scope1/") << " expected false\n";
  std::cout << "--- Testing: /server1/scope1/name1 -> " << is_server_mountpoint("/server1/scope1/name1") << " expected false\n";

  printf("\nTesting is_main_scope:\n");
  std::cout << "--- Testing: /server1 -> " << is_main_scope("/server1") << " expected false\n";
  std::cout << "--- Testing: /server1/ -> " << is_main_scope("/server1/") << " expected false\n";
  std::cout << "--- Testing: / -> " << is_main_scope("/") << " expected false\n";
  std::cout << "--- Testing: /server1/scope1 -> " << is_main_scope("/server1/scope1") << " expected true\n";
  std::cout << "--- Testing: /server1/scope1/ -> " << is_main_scope("/server1/scope1/") << " expected true\n";
  std::cout << "--- Testing: /server1/scope1/name1 -> " << is_main_scope("/server1/scope1/name1") << " expected false\n";

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
  std::string test_string = "/server/scope/container/dataset/filename/";
  std::cout << "--- Test string: ";
  std::cout << test_string << "->";
  remove_trailing_token(test_string);
  std::cout << test_string << std::endl;

  std::cout << "--- Server name: " << extract_server_name(test_string) << std::endl;
  std::cout << "--- Scope: " << extract_scope(test_string) << std::endl;
  std::cout << "--- File name: " << extract_name(test_string) << std::endl;
  std::cout << "--- DiD name: " << get_did(test_string) << std::endl;

  printf("\nTesting did string concatenation:\n");
  std::string test_dids_string = R"({"adler32": "f907738a", "name": "H-H1_HOFT_C00-1126346752-4096.gwf", "bytes": 458416223, "scope": "ER8", "type": "FILE", "md5": "8b50ad882ea5813f2b8b17dccfba2d26"}
{"adler32": "0a6ec5ac", "name": "H-H1_HOFT_C00-1126350848-4096.gwf", "bytes": 45)";
  std::string test_dids_string2 = R"(8425803, "scope": "ER8", "type": "FILE", "md5": "f1d70565a33fc075feb62f4a5c2e3b9f"}
{"adler32": "b76bd851", "name": "H-H1_HOFT_C00-1126354944-4096.gwf", "bytes": 456326199, "scope": "ER8", "type": "FILE", "md5": "088509c189b893dedf397eadf79cb980"})";
  std::vector<std::string> did_strings_vect;
  split_dids(test_dids_string, did_strings_vect);
  split_dids(test_dids_string2, did_strings_vect);

  return 0;
}