//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#include <curl-REST.h>
#include <utils.h>
#include <iostream>
#include <unordered_map>
#include <fastlog.h>

using namespace fastlog;

size_t curl_append_string_to_vect_callback(void *contents, size_t size, size_t nmemb, std::vector<std::string> &s)
{
    size_t newLength = size*nmemb;
    s.emplace_back(to_string((char*)contents, newLength));
    return newLength;
}

curlRet GET(const std::string& url, const std::string& ca_path, const struct curl_slist* headers, bool include_headers, long timeout, bool insecure){
  curlRet ret;
  {
    auto static_curl = curlWrap();

    fastlog(INFO, "GET %s", url.data());
    fastlog(DEBUG, "CA path %s", ca_path.data());

    curl_easy_setopt(static_curl(), CURLOPT_URL, url.data());
    curl_easy_setopt(static_curl(), CURLOPT_CUSTOMREQUEST, "GET");

    if(insecure){
      curl_easy_setopt(static_curl(), CURLOPT_SSL_VERIFYPEER, CURLOPT_FALSE); //only for https
      curl_easy_setopt(static_curl(), CURLOPT_SSL_VERIFYHOST, CURLOPT_FALSE); //only for https
    } else {
      curl_easy_setopt(static_curl(), CURLOPT_SSL_VERIFYPEER, CURLOPT_TRUE); //only for https
      curl_easy_setopt(static_curl(), CURLOPT_SSL_VERIFYHOST, CURLOPT_SUPERTRUE); //only for https
    }

    if(not ca_path.empty()) {
      curl_easy_setopt(static_curl(), CURLOPT_CAINFO, ca_path.data());
      curl_easy_setopt(static_curl(), CURLOPT_CAPATH, ca_path.data());
    }

    curl_easy_setopt(static_curl(), CURLOPT_WRITEFUNCTION, curl_append_string_to_vect_callback);
    curl_easy_setopt(static_curl(), CURLOPT_WRITEDATA, &ret.payload);
    curl_easy_setopt(static_curl(), CURLOPT_VERBOSE, CURLOPT_FALSE); //remove this to disable verbose output
    curl_easy_setopt(static_curl(), CURLOPT_TIMEOUT, timeout);

    // Include reply headers in CURLOPT_WRITEFUNCTION
    if (include_headers) {
      curl_easy_setopt(static_curl(), CURLOPT_HEADER, CURLOPT_TRUE);
    }

    // Add headers to request if present
    if (headers) {
      curl_easy_setopt(static_curl(), CURLOPT_HTTPHEADER, headers);
    }

    // Perform CURL request
    ret.res = curl_easy_perform(static_curl());
  }

  // Check return code to detect issues
  if(ret.res != CURLE_OK)
  {
    fastlog(ERROR, "curl_easy_perform() failed: %s url=", curl_easy_strerror(ret.res), url.data());
  }

  return ret;
}

curlRet GET_x509(const std::string& url, curlx509Bundle& bundle, const struct curl_slist* headers, bool include_headers, long timeout){
  curlRet ret;
  {
    auto static_curl = curlWrap();

    fastlog(DEBUG, "GET x509 %s", url.data());
    fastlog(DEBUG, "CA path %s", bundle.pCACertFile.data());

    curl_easy_setopt(static_curl(), CURLOPT_URL, url.data());

    // x509 setup
    curl_easy_setopt(static_curl(), CURLOPT_SSLCERT, bundle.pCertFile.data());
    curl_easy_setopt(static_curl(), CURLOPT_SSLCERTTYPE, "PEM");
    curl_easy_setopt(static_curl(), CURLOPT_CUSTOMREQUEST, "GET");

    if (bundle.pEngine.length() != 0) {
      curl_easy_setopt(static_curl(), CURLOPT_SSLENGINE, bundle.pEngine.data());
    } else {
      curl_easy_setopt(static_curl(), CURLOPT_SSLENGINE_DEFAULT, CURLOPT_TRUE);
    }

    curl_easy_setopt(static_curl(), CURLOPT_SSLENGINE_DEFAULT, CURLOPT_TRUE);
    curl_easy_setopt(static_curl(), CURLOPT_SSLKEYTYPE, bundle.pKeyType.data());
    curl_easy_setopt(static_curl(), CURLOPT_SSLKEY, bundle.pKeyName.data());

    if (!bundle.pPassphrase.empty()) curl_easy_setopt(static_curl(), CURLOPT_KEYPASSWD, bundle.pPassphrase.data());

    curl_easy_setopt(static_curl(), CURLOPT_CAINFO, bundle.pCACertFile.data());
    curl_easy_setopt(static_curl(), CURLOPT_CAPATH, bundle.pCACertFile.data());

    curl_easy_setopt(static_curl(), CURLOPT_SSL_VERIFYPEER, CURLOPT_TRUE); //only for https
    curl_easy_setopt(static_curl(), CURLOPT_SSL_VERIFYHOST, CURLOPT_SUPERTRUE); //only for https

    curl_easy_setopt(static_curl(), CURLOPT_WRITEFUNCTION, curl_append_string_to_vect_callback);
    curl_easy_setopt(static_curl(), CURLOPT_WRITEDATA, &ret.payload);
    curl_easy_setopt(static_curl(), CURLOPT_VERBOSE, CURLOPT_FALSE); //remove this to disable verbose output

    // Include reply headers in CURLOPT_WRITEFUNCTION
    if (include_headers) {
      curl_easy_setopt(static_curl(), CURLOPT_HEADER, CURLOPT_TRUE);
    }

    // Add headers to request if present
    if (headers) {
      curl_easy_setopt(static_curl(), CURLOPT_HTTPHEADER, headers);
    }

    // Perform CURL request
    ret.res = curl_easy_perform(static_curl());
  }

  // Check return code to detect issues
  if(ret.res != CURLE_OK)
  {
    fastlog(ERROR, "curl_easy_perform() failed: %s\n", curl_easy_strerror(ret.res));
  }

  return ret;
}

curlRet safeGET(const std::string& url, const std::string& ca_path, const struct curl_slist * headers, bool include_headers, long timeout){
  curlRet ret = GET(url, ca_path, headers, include_headers, timeout);
  while(ret.res != CURLE_OK){
    fastlog(ERROR, "safeGET: Curl error on URL %s. Retry.", url.data());
    timeout*=2;
    ret = GET(url, ca_path, headers, include_headers, timeout);
  }

  return ret;
}

curlRet POST(const std::string& url, const std::string& thing_to_post){
  curlRet ret;
  {
    auto static_curl = curlWrap();

    fastlog(DEBUG, "POST %s", url.data());

    curl_easy_setopt(static_curl(), CURLOPT_URL, url.data());
    curl_easy_setopt(static_curl(), CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(static_curl(), CURLOPT_POSTFIELDS, thing_to_post.data());
    curl_easy_setopt(static_curl(), CURLOPT_SSL_VERIFYPEER, CURLOPT_FALSE); //only for https
    curl_easy_setopt(static_curl(), CURLOPT_SSL_VERIFYHOST, CURLOPT_FALSE); //only for https
    curl_easy_setopt(static_curl(), CURLOPT_VERBOSE, CURLOPT_TRUE); //remove this to disable verbose output

    // Perform CURL request
    ret.res = curl_easy_perform(static_curl());
  }

  // Check return code to detect issues
  if(ret.res != CURLE_OK)
    fastlog(ERROR, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(ret.res));

  return ret;
}