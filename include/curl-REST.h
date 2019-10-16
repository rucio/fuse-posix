//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#ifndef RUCIO_FUSE_CURL_REST_H
#define RUCIO_FUSE_CURL_REST_H

#include <vector>
#include <string>
#include <curl/curl.h>

static short curl_singleton = 0;
static void* static_curl_address = nullptr;

// This curl wrapper handles a CURL pointer with automatic cleanup
// It supports () operator for easier usage
struct curl_wrapper{
  CURL* curl;

  curl_wrapper(){
    if(curl_singleton == 0) {
      curl_global_init(CURL_GLOBAL_DEFAULT);
      curl = curl_easy_init();
      curl_singleton++;
      static_curl_address = curl;
    } else {
      printf("CURL already instanciated.\n");
      curl = ((curl_wrapper*)static_curl_address)->curl;
    }
  }

  ~curl_wrapper(){
    if(curl) {
      curl_easy_cleanup(curl);
      curl_singleton--;
      static_curl_address = nullptr;
    }
    curl_global_cleanup();
  }

  CURL* operator()(){
    return curl;
  }
};

// This is the static CURL used by the whole project
static curl_wrapper static_curl = curl_wrapper();

// This struct groups the returned CURL code and a payload in the form of splitted lines
struct curlRet {
  CURLcode res;
  std::vector<std::string> payload;
};

// This is the REST GET wrapper
curlRet GET(const std::string& url);

curlRet POST(const std::string& url, const std::string& thing_to_post);

#endif //RUCIO_FUSE_CURL_REST_H
