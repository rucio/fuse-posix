//
// Created by Gabriele Gaetano Fronzé on 2019-10-17.
//

#include <REST-API.h>
#include <globals.h>
#include <iostream>

int main(){

  rucio_get_auth_token_userpass();
  printf("Got the following token: %s\n", rucio_conn_token.c_str());
  char buffer[100];
  strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S %Z",&rucio_conn_token_exp);
  std::cout << buffer << std::endl;

  if(rucio_conn_token_exp_epoch < time(0)){
    std::cout << "Token not valid!" << std::endl;
  }

  return 0;
}
