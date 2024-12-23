#pragma once

/*
Objective of this code: make life easier to do simple HTTP requests

What this project is not:
- A replacement to cURL functions
- A full featured version of cURL

What this project is:
- An easy way to use cURL in modern C++ (with destructors and so on)
- My way of doing things ;P
- Easy interface to common stuff
*/

#include <string>
#include <vector>
#include <unordered_map>
extern "C" {
#include <curl/curl.h>
}

namespace http {
    
    using headers = std::vector<std::string>;
    using pairs = std::unordered_map<std::string, std::string>;

    struct response {
        headers head;
        long code{};
        std::string body;
    };
    struct request {
        // headers
        headers head;
        // body post data
        std::string post_body;
        // for post and maybe put, file data
        std::string file_data;

        request& set_headers(headers&&);
        request& set_post_body(const std::string&);
        request& set_post_body(const pairs&);
        request& set_file_data(const std::string&);
        request& set_file_data(std::string&&);
    };

    response do_PUT    (const std::string&, const request& = {});
    response do_PUT    (const std::string&, const pairs&, const request& = {});
    response do_DELETE (const std::string&, const request& = {});
    response do_DELETE (const std::string&, const pairs&,  const request& = {});
    response do_PATCH  (const std::string&, const request& = {});
    response do_PATCH  (const std::string&, const pairs&,  const request& = {});
    response do_GET    (const std::string&, const request& = {});
    response do_GET    (const std::string&, const pairs&,  const request& = {});
    response do_POST   (const std::string&, const request& = {});
    response do_POST   (const std::string&, const pairs&,  const request& = {});
    response do_OPTIONS(const std::string&, const request& = {});
    response do_OPTIONS(const std::string&, const pairs&,  const request& = {});
}

#include "curl_cpp.ipp"