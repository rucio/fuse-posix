//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#include <curl-REST.h>

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

  curl_easy_setopt(static_curl(), CURLOPT_URL, url.c_str());
  curl_easy_setopt(static_curl(), CURLOPT_SSL_VERIFYPEER, 0L); //only for https
  curl_easy_setopt(static_curl(), CURLOPT_SSL_VERIFYHOST, 0L); //only for https
  curl_easy_setopt(static_curl(), CURLOPT_WRITEFUNCTION, curl_append_string_to_vect_callback);
  curl_easy_setopt(static_curl(), CURLOPT_WRITEDATA, &ret.payload);
  curl_easy_setopt (static_curl(), CURLOPT_VERBOSE, 1L); //remove this to disable verbose output

  ret.res = curl_easy_perform(static_curl());
  /* Check for errors */
  if(ret.res != CURLE_OK)
  {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(ret.res));
  }

  return ret;
}

curlRet POST(const std::string& url, const std::string& thing_to_post){
  curlRet ret;

  curl_easy_setopt(static_curl(), CURLOPT_URL, url.c_str());
  curl_easy_setopt(static_curl(), CURLOPT_POSTFIELDS, thing_to_post.c_str());

  /* Perform the request, res will get the return code */
  ret.res = curl_easy_perform(static_curl());
  /* Check for errors */
  if(ret.res != CURLE_OK)
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(ret.res));

  return ret;
}