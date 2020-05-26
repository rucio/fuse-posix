/*
Created by Gabriele Gaetano Fronzé on 2019-10-16.
Authors:
- Gabriele Gaetano Fronzé <gabriele.fronze at to.infn.it>, 2019
- Vivek Nigam <viveknigam.nigam3@gmail.com>, 2020
*/

#include <REST-API.h>
#include <globals.h>
#include <iostream>

using namespace fastlog;

void test_server_connection(std::string server_short_name){
  printf("--------------------------------------------------------------------------------------------------------\n"
         "Testing server %s\n"
         "--------------------------------------------------------------------------------------------------------\n\n",
         server_short_name.c_str());
  std::cout<<std::flush;

  {
    rucio_get_auth_token_userpass(server_short_name);

    auto token = get_server_token(server_short_name);

    fastlog(INFO, "Token Received: %s", token->conn_token.c_str());

    if (not rucio_is_token_valid(server_short_name)) {
      fastlog(ERROR, "Token Expired");
    } else {
      char buffer[100];
      strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S %Z", &token->conn_token_exp);
      fastlog(INFO, "Token will expire in %.2f hours", difftime(token->conn_token_exp_epoch, time(nullptr))/3600);
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
  parse_settings();
  test_server_connection("rucio-dev-server");
  //test_server_connection("rucio-dev-server");

  test_server_scopes("rucio-dev-server");

  auto scopes = rucio_list_scopes("rucio-dev-server");

  if(not scopes.empty()){
    for(const auto& scope : scopes){
      test_scope_dids("rucio-dev-server", scope);
    }
  } else {
    std::cout << "No scope found.\n";
  }

  printf("--------------------------------------------------------------------------------------------------------\n"
     "Testing is_container\n"
     "--------------------------------------------------------------------------------------------------------\n\n");

  std::cout << "Container at /rucio-dev-server/user.root/test-ds " << rucio_is_container("/rucio-dev-server/user.root/test-ds") << " -> expected True\n";
  std::cout << "Container at /rucio-dev-server/user.root/test2.txt " << rucio_is_container("/rucio-dev-server/user.root/test2.txt") << " -> expected False\n";

  printf("--------------------------------------------------------------------------------------------------------\n"
       "Retrieving dids from server %s, scope %s and name %s\n"
       "--------------------------------------------------------------------------------------------------------\n\n",
       "rucio-dev-server",
       "user.root",
       "test-ds");
  auto ret = rucio_list_container_dids("user.root", "test-ds", "rucio-dev-server");
  for(const auto& did : ret){
    std::cout << did.name << std::endl;
  }

  return 0;
}
