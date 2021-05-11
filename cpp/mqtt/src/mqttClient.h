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
#pragma once

#include <string>
#include <json.hpp>
#include "mqtt/async_client.h"

struct mqttConfig_t
{
	std::string mqttServerAddress;
	std::string username;
	std::string password;
	std::string baseTopic;
	std::string clientId;
	bool breakDownJson;
};

class MQTTclient {
public:
	MQTTclient(mqttConfig_t& config, mqtt::iasync_client &client);
	int connect();
	int publish(std::string deviceId, nlohmann::json jdata);

private:
	std::string ReformatKey(std::string key);

private:
	std::string m_mqttServerAddress;
	std::string m_baseTopic;
	std::string m_clientId;
	std::string m_username;
	std::string m_password;
	bool m_breakDownJson;

	// mqtt::async_client* client;
	mqtt::iasync_client* p_client;
};