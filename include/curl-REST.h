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

static const auto CURLOPT_FALSE = 0L;
static const auto CURLOPT_TRUE = 1L;
static const auto CURLOPT_SUPERTRUE = 2L;

// This curl wrapper handles a CURL pointer with automatic cleanup
// It supports () operator for easier usage
struct curlWrap {
  CURL *curl;

  curlWrap() {
    fastlog(DEBUG,"Creating CURL instance\n");
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    // Curl performances optimization
    curl_easy_setopt(curl, CURLOPT_ENCODING, "gzip");
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
    curl_easy_setopt(curl, CURLOPT_TCP_FASTOPEN, CURLOPT_TRUE);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, CURLOPT_TRUE);
  }

  ~curlWrap() {
    fastlog(DEBUG,"Cleaning CURL instance\n");
    curl_easy_cleanup(curl);
  }

  CURL *operator()() {
    return curl;
  }
};

// This struct groups the returned CURL code and a payload in the form of splitted lines
struct curlRet {
  CURLcode res;
  std::vector<std::string> payload;
};

struct curlx509Bundle{
    std::string pCertFile;
    std::string pCACertFile;
    std::string pPassphrase = "";
    std::string pKeyName;
    const std::string pKeyType = "PEM";
    std::string pEngine = "";
};

// This is the REST GET wrapper
curlRet GET(const std::string& url, const std::string& ca_path, const struct curl_slist * headers = nullptr, bool include_headers = false, long timeout = 2L, bool insecure = false);
curlRet GET_x509(const std::string& url, curlx509Bundle& bundle, const struct curl_slist* headers, bool include_headers = false, long timeout = 2L);

static const short max_retry = 1;
curlRet safeGET(const std::string& url, const std::string& ca_path, const struct curl_slist * headers = nullptr, bool include_headers = false, long timeout = 20L);

// This is the REST POST wrapper
curlRet POST(const std::string& url, const std::string& thing_to_post);

#endif //RUCIO_FUSE_CURL_REST_H
