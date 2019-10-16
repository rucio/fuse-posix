//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#include <curl-REST.h>

#define CURLOPT_FALSE 0L
#define CURLOPT_TRUE 1L

std::string to_string(char* contents, size_t size)
{
    std::string s;
    for (size_t i = 0; i < size; i++) {
        s = s + contents[i];
    }
    return s;
}

size_t curl_append_string_to_vect_callback(void *contents, size_t size, size_t nmemb, std::vector<std::string> &s)
{
    size_t newLength = size*nmemb;
    s.emplace_back(to_string((char*)contents, newLength));
    return newLength;
}

curlRet GET(const std::string& url){
  curlRet ret;

  curl_easy_setopt(get_curl_instance(), CURLOPT_URL, url.c_str());
  curl_easy_setopt(get_curl_instance(), CURLOPT_SSL_VERIFYPEER, CURLOPT_FALSE); //only for https
  curl_easy_setopt(get_curl_instance(), CURLOPT_SSL_VERIFYHOST, CURLOPT_FALSE); //only for https
  curl_easy_setopt(get_curl_instance(), CURLOPT_WRITEFUNCTION, curl_append_string_to_vect_callback);
  curl_easy_setopt(get_curl_instance(), CURLOPT_WRITEDATA, &ret.payload);
  curl_easy_setopt(get_curl_instance(), CURLOPT_VERBOSE, CURLOPT_TRUE); //remove this to disable verbose output

  // Perform CURL request
  ret.res = curl_easy_perform(get_curl_instance());

  // Check return code to detect issues
  if(ret.res != CURLE_OK)
  {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(ret.res));
  }

  return ret;
}

curlRet POST(const std::string& url, const std::string& thing_to_post){
  curlRet ret;

  curl_easy_setopt(get_curl_instance(), CURLOPT_URL, url.c_str());
  curl_easy_setopt(get_curl_instance(), CURLOPT_POSTFIELDS, thing_to_post.c_str());
  curl_easy_setopt(get_curl_instance(), CURLOPT_SSL_VERIFYPEER, CURLOPT_FALSE); //only for https
  curl_easy_setopt(get_curl_instance(), CURLOPT_SSL_VERIFYHOST, CURLOPT_FALSE); //only for https
  curl_easy_setopt(get_curl_instance(), CURLOPT_VERBOSE, CURLOPT_TRUE); //remove this to disable verbose output

  // Perform CURL request
  ret.res = curl_easy_perform(get_curl_instance());

  // Check return code to detect issues
  if(ret.res != CURLE_OK)
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(ret.res));

  return ret;
}