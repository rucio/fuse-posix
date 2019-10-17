//
// Created by Gabriele Gaetano Fronzé on 2019-10-17.
//

#include <REST-API.h>
#include <iostream>

int main(){

  for(auto const& line : rucio_get_auth_token_userpass()) printf("%s", line.c_str());
  std::cout << std::endl;
  return 0;
}
