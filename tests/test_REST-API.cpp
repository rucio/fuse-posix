//
// Created by Gabriele Gaetano Fronzé on 2019-10-17.
//

#include <REST-API.h>
#include <globals.h>
#include <iostream>

using namespace fastlog;

void test_server_connection(std::string server_short_name){
  printf("--------------------------------------------------------------------------------------------------------\n"
         "Testing server %s\n"
         "--------------------------------------------------------------------------------------------------------\n\n",
         server_short_name.data());
  std::cout<<std::flush;

  {
    rucio_get_auth_token(server_short_name);

    auto token = get_server_token(server_short_name);

    fastlog(INFO, "Token Received: %s", token->conn_token.data());

    if (not rucio_is_token_valid(server_short_name)) {
      fastlog(ERROR, "Token Expired");
    } else {
      fastlog(INFO, "Token will expire in %.2f hours", difftime(token->conn_token_exp_epoch, time(nullptr))/3600);
    }
  }
}

void test_server_scopes(std::string server_short_name){
  printf("--------------------------------------------------------------------------------------------------------\n"
         "Retrieving scopes from server %s\n"
         "--------------------------------------------------------------------------------------------------------\n\n",
         server_short_name.data());
  std::cout<<std::flush;

  {
    auto ret = rucio_list_scopes(server_short_name);

    printf("\nDetected scopes:\n");
    for (const auto &line : ret) {
      printf("%s\n", line.data());
    }
    std::cout << std::endl;
  }
}

void test_scope_dids(std::string server_short_name, std::string scope_name){
  printf("--------------------------------------------------------------------------------------------------------\n"
         "Retrieving dids from server %s and scope %s\n"
         "--------------------------------------------------------------------------------------------------------\n\n",
         server_short_name.data(),
         scope_name.data());
  std::cout<<std::flush;

  {
    auto ret = rucio_list_dids(scope_name, server_short_name);

    printf("\nDetected dids:\n");
    for (const auto &did : ret) {
      printf("%s/%s\n", did.scope.data(), did.name.data());
    }
    std::cout << std::endl;
  }
}

int main(){
  parse_settings_cfg();
  test_server_connection("rucio-server-torino");
  test_server_connection("rucio-server-ligo");

  test_server_scopes("rucio-server-ligo");

  auto scopes = rucio_list_scopes("rucio-server-ligo");

  if(not scopes.empty()){
    for(const auto& scope : scopes){
      test_scope_dids("rucio-server-ligo", scope);
    }
  } else {
    std::cout << "No scope found.\n";
  }

  printf("--------------------------------------------------------------------------------------------------------\n"
     "Testing is_container\n"
     "--------------------------------------------------------------------------------------------------------\n\n");

  std::cout << "Container at /rucio-server-torino/user.root/test-ds " << rucio_is_container("/rucio-server-torino/user.root/test-ds") << " -> expected True\n";
  std::cout << "Container at /rucio-server-torino/user.root/test2.txt " << rucio_is_container("/rucio-server-torino/user.root/test2.txt") << " -> expected False\n";

  printf("--------------------------------------------------------------------------------------------------------\n"
       "Retrieving dids from server %s, scope %s and name %s\n"
       "--------------------------------------------------------------------------------------------------------\n\n",
       "rucio-server-torino",
       "user.root",
       "test-ds");
  auto ret = rucio_list_container_dids("user.root", "test-ds", "rucio-server-torino");
  for(const auto& did : ret){
    std::cout << did.name << std::endl;
  }

  return 0;
}
