/***************************************************************************
*** MIT License ***
*
*** Copyright (c) 2020-2021 Daniel Mancuso - OhmTech.io **
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
// #include <stdio>
#include <string>
#include <cstdlib>
#include <curl/curl.h>
#include "influx.h"
#include "uBridgeConfig.h"
#include "ubridgeClient.h"

InfluxClient::InfluxClient(std::string url) : _url(url){

}

int InfluxClient::CheckReadiness(void) {
	CURLcode res;
	
	curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_URL, _url);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1);	
	/* query the "ready" endpoint */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "/ready");
 
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
	switch (res) {
		case CURLE_OK:
			// LOG_S(INFO) << "Influx DB ready";
			break;
		case CURLE_HTTP_RETURNED_ERROR:
			break;
		case CURLE_OPERATION_TIMEDOUT:
			break;
		case CURLE_COULDNT_CONNECT:
			break;
		case CURLE_COULDNT_RESOLVE_HOST:
			break;	
		default:
			int a=1;
			// LOG_S(INFO) << res <<" error on request to Influx";	
	}

	 /* always cleanup */ 
    curl_easy_cleanup(curl);

	return res;
}

int	InfluxClient::Write() {
	return 0;
	// CURLcode curl_easy_setopt(CURL *handle, CURLOPT_TIMEOUT, long timeout);	
}





