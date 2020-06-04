//
// Created by Gabriele Gaetano Fronzé on 2020-06-04.
//

#include "rucio-download.h"

int main(){

  auto info = rucio_download_info("scope:filename");
  printf("scope: %s - filename: %s\n",info.scopename().data(),info.filename().data());

  return 0;
}