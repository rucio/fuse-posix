/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Copyright European Organization for Nuclear Research (CERN)
Licensed under the Apache License, Version 2.0 (the "License");
You may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
Authors:
 - Gabriele Gaetano Fronzé, <gfronze@cern.ch>, 2019-2020
/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <globals.h>
#include "rucio-download.h"

int main(){
  parse_settings_cfg();
  auto info = rucio_download_info("scope:filename", "/ruciofs/server/scope/name");
  printf("scope: %s - filename: %s\n",info.scopename().data(),info.filename().data());

  return 0;
}