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

#include <json.hpp>
#include "influx.h"
#include "uBridgeConfig.h"
#include "ubridgeClient.h"

using json = nlohmann::json;

InfluxClient::InfluxClient(influxConfig_t& config){
	m_url = config.url;
	m_org = config.org;
	m_token = config.token;
	m_bucket = config.bucket; //'database' on the API v1
}

int InfluxClient::CheckReadiness(void) {
	CURLcode res;
	
	curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1);	
	/* query the "ready" endpoint */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "/ready");
 
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
    	LOG_S(WARNING) << "Influx client: " << curl_easy_strerror(res);

      // fprintf(stderr, "curl_easy_perform() failed: %s\n",
              // curl_easy_strerror(res));

	switch (res) {
		case CURLE_OK:
			LOG_S(INFO) << "InfluxDB READY!";
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
			LOG_S(WARNING) << res <<" error on request to Influx";	
	}

	 /* always cleanup */ 
    curl_easy_cleanup(curl);

	return res;
}

int	InfluxClient::Write(std::string deviceId, json jdata) {
	CURLcode res;
	struct curl_slist *list = NULL;
	
	/*TODO:
		- pull data from thread safe queue
		- add a batchSize config. so we can write N datapoints per call (ideal size from Influx docs is 5000)
		*/
	curl = curl_easy_init();
	
	std::string urlRequest = m_url
		+ "/api/v2/write?org=" + m_org 
		+ "&bucket=" + m_bucket;
		//+ "&precision=ms"; 

	curl_easy_setopt(curl, CURLOPT_URL, urlRequest.c_str());
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1);	

	std::string header = "Authorization: Token " + m_token;
	list = curl_slist_append(list, header.c_str());
  	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

  	
	// std::string data = "sensor1 temperature=25.49";
	std::string data = ConvertToLineProtocol(deviceId, jdata);

	curl_easy_setopt(curl, 
		CURLOPT_POSTFIELDS, 
		data.c_str());

	LOG_S(7) << "InfluxDB Request:  " << urlRequest;
	/* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);

	if(res != CURLE_OK)
    	LOG_S(WARNING) << "Influx client: " << curl_easy_strerror(res);
    else 
    	LOG_S(6) << "Data written to Influx: " << data;
	 /* always cleanup */ 
    curl_easy_cleanup(curl);
    curl_slist_free_all(list); 

	return res;
}

std::string InfluxClient::ConvertToLineProtocol(std::string& deviceId, json& jdata) {
	/* Line protocol fields in [] are optional:
	 _measurement[,_tagKey=tagValue] _field=_value [_timestamp]
	 examples: 
	 "outdoorSensor1 temperature=21.08, humidity=56.31"
	 "outdoorSensor,location=frontDoor temperature=21.08 1566086760000000000"*/
	json flat_jdata = jdata.flatten();
	

	std::string line;
	line += deviceId; //Measurement
	line += " ";
	line += "noise.peak"; //Field1
	line += "=";
	line += "48.62";
	line += ",";
	line += "noise.rms"; //Field2
	line += "=";
	line += "43.42";

	json jtest = R"({"noise":{"rms":33.5,"peak":33.5,"base":30.4},"pir":{"detections":0,"detPerHour":6},"light":{"last":58.13,"average":51.67}})"_json;
	LOG_S(INFO) << jtest;
	LOG_S(INFO) << jtest.flatten();

	json jtest2 = R"({"IAQ": 45.5,"eqBreathVOC": 0.68,"eqCO2": 583.06,"gasResistance": 2235196,"humidity": 33.33,"iaqAccuracy": 3,"pressure": 1015.63,"temperature": 24.9})"_json;
	LOG_S(INFO) << jtest2;
	LOG_S(INFO) << jtest2.flatten();
	// LOG_S(INFO) << line;
	return line;
}


/*

curl --request POST "http://localhost:8086/api/v2/write?org=dev&bucket=sensors&precision=ms" \
--header "Authorization: Token hHyrHUzvi6NzcWeofzly2CCwmWOEY95uK5ZIAj3wrlq8ndcMYPo1M8eOHaqzIramO11gwK9jdXJoDHLlFXci8g==" \
--data-raw "
sensor1 temperature=10.1234
" -v

*/

/*
{
  "IAQ": 45.5,
  "eqBreathVOC": 0.68,
  "eqCO2": 583.06,
  "gasResistance": 2235196,
  "humidity": 33.33,
  "iaqAccuracy": 3,
  "pressure": 1015.63,
  "temperature": 24.9
}

{
	"noise": {
		"rms": 33.5,
		"peak": 33.5,
		"base": 30.4
	},
	"pir": {
		"detections": 0,
		"detPerHour": 6
	},
	"light": {
		"last": 58.13,
		"average": 51.67
	}
}

{"noise":{"rms":33.5,"peak":33.5,"base":30.4},"pir":{"detections":0,"detPerHour":6},"light":{"last":58.13,"average":51.67}}

after json.flatten():
 {"/light/average":51.67,"/light/last":58.13,"/noise/base":30.4,"/noise/peak":33.5,"/noise/rms":33.5,"/pir/detPerHour":6,"/pir/detections":0}
*/

