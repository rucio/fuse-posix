//
// Created by Gabriele Gaetano Fronzé on 2019-10-17.
//

#include <REST-API.h>
#include <globals.h>
#include <iostream>

void test_server_connection(std::string server_short_name){
  printf("--------------------------------------------------------------------------------------------------------\n"
         "Testing server %s\n"
         "--------------------------------------------------------------------------------------------------------\n\n",
         server_short_name.c_str());
  std::cout<<std::flush;

  {
    rucio_get_auth_token_userpass(server_short_name);

    auto token = get_server_token(server_short_name);

    printf("Got the following token: %s\n", token->conn_token.c_str());

    if (not rucio_is_token_valid("rucio-server")) {
      std::cout << "Token not valid!" << std::endl;
    } else {
      char buffer[100];
      strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S %Z", &token->conn_token_exp);
      printf("Token will be valid until %s\n\n", buffer);
    }
  }
}

void test_server_scopes(std::string server_short_name){
  printf("--------------------------------------------------------------------------------------------------------\n"
         "Retrieving scopes from server %s\n"
         "--------------------------------------------------------------------------------------------------------\n\n",
         server_short_name.c_str());
  std::cout<<std::flush;

  {
    auto ret = rucio_list_scopes(server_short_name);

    printf("\nDetected scopes:\n");
    for (const auto &line : ret) {
      printf("%s\n", line.c_str());
    }
    std::cout << std::endl;
  }
}

void test_scope_dids(std::string server_short_name, std::string scope_name){
  printf("--------------------------------------------------------------------------------------------------------\n"
         "Retrieving dids from server %s and scope %s\n"
         "--------------------------------------------------------------------------------------------------------\n\n",
         server_short_name.c_str(),
         scope_name.c_str());
  std::cout<<std::flush;

  {
    auto ret = rucio_list_dids(scope_name, server_short_name);

    printf("\nDetected dids:\n");
    for (const auto &did : ret) {
      printf("%s/%s\n", did.scope.c_str(), did.name.c_str());
    }
    std::cout << std::endl;
  }
}

int main(){
  test_server_connection("rucio-server");
  test_server_connection("rucio-server-clone");

  test_server_scopes("rucio-server");

  auto scopes = rucio_list_scopes("rucio-server");

  if(not scopes.empty()){
    for(const auto& scope : scopes){
      test_scope_dids("rucio-server", scope);
    }
  } else {
    std::cout << "No scope found.\n";
  }

  return 0;
}
