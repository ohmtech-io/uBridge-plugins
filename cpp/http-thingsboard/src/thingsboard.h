/***************************************************************************
*** MIT License ***
*
*** Copyright (c) 2020-2025 Daniel Mancuso - OhmTech.io **
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
****************************************************************************/

#pragma once

#include <string>
#include <curl/curl.h>
//#include <nlohmann/json.hpp>
#include "loguru.hpp"

struct thingsboardConfig_t {
    std::string serverUrl = "http://localhost:8080";  // Default ThingsBoard server URL
    std::string accessToken = "";                     // Device access token
};

class ThingsboardClient {
private:
    thingsboardConfig_t config;
    CURL* curl;

    // Callback function to handle CURL response
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

public:
    ThingsboardClient(const thingsboardConfig_t& cfg) : config(cfg) {
        curl = curl_easy_init();
        if (!curl) {
            LOG_S(ERROR) << "Failed to initialize CURL";
            throw std::runtime_error("CURL initialization failed");
        }
    }

    ~ThingsboardClient() {
        if (curl) {
            curl_easy_cleanup(curl);
        }
    }

    // bool sendTelemetry(const std::string& deviceId, const std::string& jsonData) {
    bool sendTelemetry(const std::string& deviceId, json jsonData) {
        if (!curl) {
            LOG_S(ERROR) << "CURL not initialized";
            return false;
        }

        LOG_S(9) << "JSON_DATA: " << jsonData.dump();  // Log the actual JSON being sent
        
        std::string jsonStr = jsonData.dump();
        // Construct the URL for the device
        std::string url = config.serverUrl + "/api/v1/" + config.accessToken + "/telemetry";

        // Set up headers
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        std::string response;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            LOG_S(ERROR) << "Failed to send telemetry: " << curl_easy_strerror(res);
            return false;
        }

        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        if (http_code != 200) {
            LOG_S(ERROR) << "HTTP error: " << http_code << ", Response: " << response;
            return false;
        }

        LOG_S(9) << "Successfully sent telemetry for device " << deviceId;
        return true;
    }
}; 
