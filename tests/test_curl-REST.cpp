//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

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