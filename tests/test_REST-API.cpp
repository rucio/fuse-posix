//
// Created by Gabriele Gaetano Fronzé on 2019-10-17.
//

#include <REST-API.h>

int main(){

  for(auto const& line : rucio_get_auth_token_userpass()) printf("%s\n", line.c_str());

  return 0;
}
