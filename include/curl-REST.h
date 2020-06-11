//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#ifndef RUCIO_FUSE_CURL_REST_H
#define RUCIO_FUSE_CURL_REST_H

#include <vector>
#include <string>
#include <curl/curl.h>
#include <fastlog.h>

using namespace fastlog;

namespace curlSingleton {
  static short curl_singleton = 0;
  static void *static_curl_address = nullptr;

// This curl wrapper handles a CURL pointer with automatic cleanup
// It supports () operator for easier usage
  struct curlWrap {
    CURL *curl;

    curlWrap() {
      if (curl_singleton == 0) {
        fastlog(DEBUG,"Creating CURL instance\n");
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();
        static_curl_address = curl;
      } else {
        curl = (CURL *) static_curl_address;
      }
      curl_singleton++;
    }

    ~curlWrap() {
      curl_singleton--;
      if (curl_singleton == 0) {
        fastlog(DEBUG,"Cleaning CURL instance\n");
        curl_easy_cleanup(curl);
        curl_global_cleanup();
      }
    }

    CURL *operator()() {
      return curl;
    }
  };
}

//static curlSingleton::curlWrap static_curl;

// This struct groups the returned CURL code and a payload in the form of splitted lines
struct curlRet {
  CURLcode res;
  std::vector<std::string> payload;
};

struct curlx509Bundle{
    std::string pCertFile;
    std::string pCACertFile;
    std::string pPassphrase;
    std::string pKeyName;
    std::string pKeyType = "PEM";
    std::string pEngine = nullptr;
};

// This is the REST GET wrapper
curlRet GET(const std::string& url, const struct curl_slist * headers = nullptr, bool include_headers = false);
curlRet GET_x509(const std::string& url, curlx509Bundle& bundle, const struct curl_slist* headers, bool include_headers);

// This is the REST POST wrapper
curlRet POST(const std::string& url, const std::string& thing_to_post);

#endif //RUCIO_FUSE_CURL_REST_H
