/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Copyright European Organization for Nuclear Research (CERN)
Licensed under the Apache License, Version 2.0 (the "License");
You may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
Authors:
 - Gabriele Gaetano Fronzé, <gfronze@cern.ch>, 2019-2020
/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <curl-REST.h>
#include <iostream>

int main(){

  auto res = GET("www.example.com", "");

  std::cout << std::endl;

  for(auto const& line : res.payload){
    printf("%s\n", line.data());
  }

  std::cout << std::endl;

  printf("%lu lines received\n", res.payload.size());

  return 0;
}