//
// Created by Gabriele Gaetano Fronzé on 2019-10-17.
//

#include <REST-API.h>
#include <globals.h>
#include <iostream>

int main(){

  rucio_get_auth_token_userpass();
  printf("Got the following token: %s\n", rucio_conn_token.c_str());

  return 0;
}
