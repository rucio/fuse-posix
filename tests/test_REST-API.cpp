//
// Created by Gabriele Gaetano Fronzé on 2019-10-17.
//

#include <REST-API.h>
#include <globals.h>
#include <iostream>

int main(){
  {
    rucio_get_auth_token_userpass();
    printf("Got the following token: %s\n", rucio_conn_token.c_str());

    if (not rucio_is_token_valid) {
      std::cout << "Token not valid!" << std::endl;
    } else {
      char buffer[100];
      strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S %Z", &rucio_conn_token_exp);
      printf("Token will be valid until %s\n", buffer);
    }
  }

  {
    auto ret = rucio_list_scopes();

    for(const auto& line : ret){
      printf("%s\n", line.c_str());
    }
  }

  return 0;
}
