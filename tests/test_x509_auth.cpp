//
// Created by Gabriele Gaetano Fronzé on 2020-06-18.
//

#include <globals.h>
#include <REST-API.h>
#include <fastlog.h>

int main(){
  fastlog::logLevel = DEBUG;

  parse_settings_cfg();

  rucio_get_auth_token_userpass("test-userpass");
  rucio_get_auth_token_x509("test-x509");

  return 0;
}

