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
#include <string>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <iomanip>

#include <json.hpp>
#include "mqtt/async_client.h"

#include "uBridgeConfig.h"
#include "ubridgeClient.h"
#include "mqttClient.h"

using json = nlohmann::json;

const int QOS = 1;

	const char* PAYLOADS[] = {
		"Hello World!",
		"Hi there!",
		"Is anyone listening?",
		"Someone is always listening.",
		nullptr
	};

MQTTclient::MQTTclient(mqttConfig_t& config, mqtt::iasync_client &client) {
	m_mqttServerAddress = config.mqttServerAddress;
	m_baseTopic = config.baseTopic;
	m_clientId = config.clientId;
	m_breakDownJson = config.breakDownJson;
	m_username = config.username;
	m_password = config.password;

	p_client = &client;

}


int MQTTclient::connect() {
	try {
		auto connOpts = mqtt::connect_options_builder()
			.user_name(m_username)
			.password(m_password)
			.keep_alive_interval(std::chrono::seconds(30))
			.automatic_reconnect(std::chrono::seconds(2), std::chrono::seconds(30))
			.clean_session(false)
			.finalize();

		LOG_S(INFO) << "Connecting to broker: " << m_mqttServerAddress;
		p_client->connect(connOpts)->wait();
		LOG_S(INFO) << "Connected!";
	}	
	catch (const mqtt::exception& exc) {
		std::cerr << exc << std::endl;
		return 1;
	}
	return 0;
}

int MQTTclient::publish(std::string deviceId, json jdata) {	

	if (m_breakDownJson) {

	} else {
		LOG_S(6) << "Publishing: " << deviceId << jdata;
		mqtt::topic top(*p_client, m_baseTopic + "/" +  deviceId, QOS);
		mqtt::token_ptr tok;

		tok = top.publish(jdata.dump());
	}
	return 0;
}


std::string MQTTclient::ReformatKey(std::string key) {
	//example flattened keys: "/light/average", "/temperature"
	key.erase(key.begin()); //erase the 1st char ("/")

	replace(key.begin(), key.end(), '/', '.' );

	return key;
}

