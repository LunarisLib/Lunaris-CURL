#pragma once
#include "curl_cpp.h"

#include <sstream>
#include <memory>

namespace http {
        
    // Write callBack Body
    static size_t __wbbcurl(char *c, size_t s, size_t n, void *p){
        if (s * n == 0) return 0;
        std::string& str = *((std::string*)p);
        str.append(c, s * n);
        return s * n;
    }
    // Write callBack for Headers
    static size_t __wbhcurl(char *c, size_t s, size_t n, void *p){
        if (s * n == 0) return 0;
        headers& head = *((headers*)p);
        head.push_back(std::string(c, s * n));
        return s * n;
    }
    // Read callBack from Streamstring
    static size_t __rbscurl(char* c, size_t s, size_t n, void* p) {
        if (s * n == 0) return 0;
        std::stringstream& ss = *((std::stringstream*)p);
        ss.read(c, s * n);
        return ss.gcount();
    }

    request& request::set_headers(headers&& h)
    {
        head = std::move(h);
        return *this;
    }
    request& request::set_post_body(const std::string& s)
    {
        post_body = s;
        return *this;
    }
    request& request::set_post_body(const pairs& hd)
    {
        post_body.clear();

        for(const auto& each : hd) {
            post_body += each.first + "=" + each.second + "&";
        }
        if (post_body.size()) post_body.pop_back();

        return *this;
    }
    request& request::set_file_data(const std::string& s)
    {
        file_data = s;
        return *this;
    }
    request& request::set_file_data(std::string&& s)
    {
        file_data = std::move(s);
        return *this;
    }

    response generic(const char* type, const pairs& get_params, const std::string& path, const request& req)
    {
        auto m_c = std::unique_ptr<CURL, void(*)(CURL*)>(curl_easy_init(), [](CURL* c){ curl_easy_cleanup(c); });
        std::unique_ptr<std::stringstream> ss;
        std::unique_ptr<curl_slist, void(*)(curl_slist*)> headerlist(nullptr, [](curl_slist* s){ if (s) curl_slist_free_all(s); });
        response res;
        std::string parsed_url = path;

        if (get_params.size()) {
            if (parsed_url.find("?") != std::string::npos) parsed_url += "&";
            else parsed_url += "?";

            for(const auto& each : get_params) {
                parsed_url += each.first + "=" + each.second + "&";
            }

            parsed_url.pop_back(); // must be valid, because of size() implies first if-else to add at least 1 char.
        }

        curl_easy_setopt(m_c.get(), CURLOPT_URL, parsed_url.c_str());
        curl_easy_setopt(m_c.get(), CURLOPT_CUSTOMREQUEST, type); 
        curl_easy_setopt(m_c.get(), CURLOPT_FOLLOWLOCATION, 1L); 

        // post body data
        if (req.post_body.size()) {
            curl_easy_setopt(m_c.get(), CURLOPT_POSTFIELDS, req.post_body.data()); 
            curl_easy_setopt(m_c.get(), CURLOPT_POSTFIELDSIZE, req.post_body.size()); 
        }
        // headers to send
        if (req.head.size()) {
            curl_slist* tmp = nullptr;
            for(const auto& each : req.head)
                tmp = curl_slist_append(tmp, each.c_str());

            headerlist.reset(tmp);
            curl_easy_setopt(m_c.get(), CURLOPT_HTTPHEADER, headerlist.get());
        }
        // has file to upload? (POST || PUT)
        if (req.file_data.size()) {
            ss = std::make_unique<std::stringstream>(req.file_data);

            curl_easy_setopt(m_c.get(), CURLOPT_READFUNCTION, __rbscurl);
            curl_easy_setopt(m_c.get(), CURLOPT_READDATA, ss.get());
            curl_easy_setopt(m_c.get(), CURLOPT_INFILESIZE_LARGE, (curl_off_t)req.post_body.size());
        }

        // this is to get back body
        curl_easy_setopt(m_c.get(), CURLOPT_WRITEFUNCTION, __wbbcurl);
        curl_easy_setopt(m_c.get(), CURLOPT_WRITEDATA, &res.body);
        // this is to get back headers
        curl_easy_setopt(m_c.get(), CURLOPT_HEADERFUNCTION, __wbhcurl);
        curl_easy_setopt(m_c.get(), CURLOPT_HEADERDATA, &res.head);

        const auto ret = curl_easy_perform(m_c.get());

        if (ret != CURLE_OK) {
            res.code = - (ret);
            return res;
        }

        curl_easy_getinfo(m_c.get(), CURLINFO_RESPONSE_CODE, &res.code);
        return res;
    }


    response do_PUT    (const std::string& path, const request& req)
    {
        return generic("PUT", {}, path, req);
    }
    response do_PUT    (const std::string& path, const pairs& get, const request& req)
    {
        return generic("PUT", get, path, req);
    }
    
    response do_DELETE (const std::string& path, const request& req)
    {
        return generic("DELETE", {}, path, req);
    }
    response do_DELETE (const std::string& path, const pairs& get, const request& req)
    {
        return generic("DELETE", get, path, req);
    }
    
    response do_PATCH  (const std::string& path, const request& req)
    {
        return generic("PATCH", {}, path, req);
    }
    response do_PATCH  (const std::string& path, const pairs& get, const request& req)
    {
        return generic("PATCH", get, path, req);
    }

    response do_GET   (const std::string& path, const request& req)
    {
        return generic("GET", {}, path, req);
    }
    response do_GET   (const std::string& path, const pairs& get, const request& req)
    {
        return generic("GET", get, path, req);
    }
    
    response do_POST   (const std::string& path, const request& req)
    {
        return generic("POST", {}, path, req);
    }
    response do_POST   (const std::string& path, const pairs& get, const request& req)
    {
        return generic("POST", get, path, req);
    }
    
    response do_OPTIONS(const std::string& path, const request& req)
    {
        return generic("OPTIONS", {}, path, req);
    }
    response do_OPTIONS(const std::string& path, const pairs& get, const request& req)
    {
        return generic("OPTIONS", get, path, req);
    }

}