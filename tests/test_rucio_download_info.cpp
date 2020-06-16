//
// Created by Gabriele Gaetano Fronzé on 2020-06-04.
//

#include <globals.h>
#include "rucio-download.h"

int main(){
  parse_settings_cfg();
  auto info = rucio_download_info("scope:filename", "/ruciofs/server/scope/name");
  printf("scope: %s - filename: %s\n",info.scopename().data(),info.filename().data());

  return 0;
}