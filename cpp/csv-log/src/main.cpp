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
#include <fstream>
#include <iostream>
#include <chrono>

#include <cxxopts.hpp>

#include "uBridgeConfig.h"
#include "ubridgeClient.h"
#include "csvLogger.h"

// for convenience
using json = nlohmann::json;

ubridge::Config cfg;
CsvLogger* pCsvLogger; 

void subsMessageHandler(ubridge::message& message) {
	LOG_S(9) << "cb msg: " << message.topic << message.data ;

	std::string deviceId = message.topic;
	
	std::string prefix = "/sensors/";
	std::string::size_type i = deviceId.find(prefix);

	if (i != std::string::npos){
		deviceId.erase(i, prefix.length());
 	}

 	LOG_S(9) << "deviceId: " << deviceId;
	pCsvLogger->Write(deviceId, message.data);
}

json loadConfigFile(std::string& config_file){
	json jconfig;

	try {
		LOG_S(INFO) << "Opening " << config_file;
		// read in the json file
		std::ifstream file(config_file, std::ifstream::in);

		if (not file.is_open()) {
			LOG_S(ERROR) << "Error, couldn't open configuration file " << config_file;
			exit(1);
		}
		// parse using "ignore_comments=true" so we can use comments on the config file
		jconfig = json::parse(file, nullptr, true, true); 

	} catch (const std::runtime_error& ex) {
		LOG_S(WARNING) << "Error parsing options: " << ex.what();
	} catch (const nlohmann::detail::parse_error& ex) {
		LOG_S(WARNING) << "Error parsing json: " << ex.what();
	}

	LOG_S(INFO) << "Loaded configuration: "<< std::setw(2) << jconfig;

	return jconfig;
}

ubridge::Config loadBridgeConfig(json& jconfig) {
	ubridge::Config config;
	//override defaults on uBridgeConfig.h if they are present on the json file
	if (jconfig.contains("configSockUrl")) {
		config.configSockUrl = jconfig.at("configSockUrl");
		LOG_S(7) << "uBridge ReqRepSocket url: " << config.configSockUrl;
	}
	if (jconfig.contains("streamSockUrl")) {
		config.streamSockUrl = jconfig.at("streamSockUrl");
		LOG_S(7) << "uBridge Stream Socket url: " << config.streamSockUrl;
	}
	return config;
}

csvConfig_t loadCsvConfig(json& jconfig) {
	csvConfig_t config;
	
	if (jconfig.contains("logPath")) {
		config.logPath = jconfig.at("logPath");
		LOG_S(7) << "CSV logger path: " << config.logPath;
	}
	if (jconfig.contains("maxDatapointsPerFile")) {
		config.maxDatapointsPerFile = jconfig.at("maxDatapointsPerFile");
		LOG_S(7) << "Datapoints limit per file: " << config.maxDatapointsPerFile;
	}
	if (jconfig.contains("useGmtTime")) {
		config.useGmtTime = jconfig.at("useGmtTime");
		std::string x = config.useGmtTime?"UTC":"local";
		LOG_S(7) << "Using "<< x <<" time.";
	}
	if (jconfig.contains("outTimestampInMs")) {
		config.outTimestampInMs = jconfig.at("outTimestampInMs");
		std::string x = config.outTimestampInMs?"Timestamp written in unix epoch (milliseconds)":"Timestamp written in date-time format";
		LOG_S(7) << x;
	}

	return config;
}

int main(int argc, char *argv[])
{
	cxxopts::Options options("CsvPluggin", "This plugin app. acts as a client of ubridge, \
			subscribing to data from the sensors and store it into timestamped CSV files");

    options.add_options()
   		("c,config", "JSON configuration file name", cxxopts::value<std::string>()->default_value("/etc/ubridge/csv-plugin-config.json"))
        ("v,verbose", "Verbosity output level (0-9)", cxxopts::value<int>()->default_value("0"))
        ("h,help", "Print usage")
    ;

    std::string config_file;

	try {
		auto result = options.parse(argc, argv);

		if (result.count("help")) {
	  		std::cout << options.help() << std::endl;
	  		exit(0);
		}

		if (result.count("verbose")) {
			loguru::g_stderr_verbosity = result["verbose"].as<int>();
		}

		/* if no config provided with -c, use default on /etc/ubridge */
		config_file = result["config"].as<std::string>();	
	}
	catch (const cxxopts::OptionException& e) {
		std::cout << "error parsing options: " << e.what() << std::endl;
		exit(1);
	}

	loguru::init(argc, argv);
	// Only log INFO, WARNING, ERROR and FATAL
	loguru::add_file("/tmp/ubridge-csv.log", loguru::Truncate, loguru::Verbosity_INFO);

	LOG_S(INFO) << "--- Initializing ** CSV Logger plugin **... ---";

	//Load the json file from disk...
	json jconfig = loadConfigFile(config_file);

	//parse the uBridge config:
	ubridge::Config bridgeConfig = loadBridgeConfig(jconfig["ubridge"]);
	//parse the CSV config:
	csvConfig_t csvConfig = loadCsvConfig(jconfig["csv"]);

	ReqRepClient uBridgeClient{bridgeConfig.configSockUrl, bridgeConfig.streamSockUrl.c_str()};	
	
	/* check uBridge side: */	
	LOG_S(INFO) << "Pinging uBridge server..." ;
	if (uBridgeClient.connect() != 0) {
		/* wait for ubridge config server */
		LOG_S(INFO) << "uBridge Server not found!" ;	
		LOG_S(INFO) << "Waiting for server..." ;	
		while (uBridgeClient.connect() !=0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		}
	}

	CsvLogger csvLogger{csvConfig};
	pCsvLogger = &csvLogger;
		
	json deviceList;
	uBridgeClient.getDevices(deviceList);

	LOG_S(INFO) << deviceList["devCount"] << " devices detected. Details:" << std::setw(2) << deviceList["devices"];	
	
	LOG_S(INFO) << "Start logging CSV data into " << csvConfig.logPath << "/*.csv...";
	//start message receiving loop...
	uBridgeClient.subscribe("/sensors", subsMessageHandler); //subscribe to all sensors
	
	return 0;
}//main
