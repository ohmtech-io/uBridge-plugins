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
#include <fstream>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <sstream>

#include <json.hpp>
#include "uBridgeConfig.h"
#include "ubridgeClient.h"
#include "csvLogger.h"

using json = nlohmann::json;

namespace fs = std::filesystem;

CsvLogger::CsvLogger(csvConfig_t& config){
	m_logPath = config.logPath;
	m_maxDatapointsPerFile = config.maxDatapointsPerFile;
	m_useGmtTime = config.useGmtTime;
	m_outTimestampInMs = config.outTimestampInMs;
}

std::string CsvLogger::GetCurrentTimeForFileName()
{
    auto time = std::time(nullptr);
    std::stringstream ss;

    if (m_useGmtTime) {
    	ss << std::put_time(std::gmtime(&time), "%F_%TZ"); //ISO 8601, expressed in UTC.
    } else {
		ss << std::put_time(std::localtime(&time), "%F_%T"); //ISO 8601 without timezone information.
    }
    auto s = ss.str();
    std::replace(s.begin(), s.end(), ':', '-');
    return s;
}

std::string CsvLogger::GetCurrentTimeForLogging()
{
    auto time = std::time(nullptr);
    std::stringstream ss;

    if (m_useGmtTime) {
    	ss << std::put_time(std::gmtime(&time), "%F_%TZ"); //ISO 8601, expressed in UTC.
    } else {
		ss << std::put_time(std::localtime(&time), "%F_%T"); //ISO 8601 without timezone information.
    }
    auto s = ss.str();
    return s;
}

fs::path CsvLogger::AppendTimeToFileName(const fs::path& fileName)
{
    return fileName.stem().string() + "_" + GetCurrentTimeForFileName() + fileName.extension().string();
}

fs::path CsvLogger::GetFullPath(std::string deviceId)
{
	//composing the full name:
	std::string fileName = deviceId;

	// Remove the "::" from the device name (uThing::VOC_9142 --> uThingVOC_9142)
	std::string toErase = "::";
	size_t pos = fileName.find(toErase);
	if (pos != std::string::npos)
	{
	    fileName.erase(pos, toErase.length());
	}

	fs::path filePath = m_logPath / AppendTimeToFileName(fileName);
	filePath.replace_extension(".csv");

	return filePath;		
}

long CsvLogger::GetTimestampInMs()
{
	/* Get the UNIX Epoch time in ms */
	auto now = std::chrono::system_clock::now();
	//Cast the time point to ms, then get its duration, then get the duration's count.
	return (std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count());
}

int	CsvLogger::Write(std::string deviceId, json jdata) 
{	
	if (devices.find(deviceId) == devices.end()) {
		/* the device doesn't exist (first time seen)*/
		CreateFile(deviceId, jdata);
	} 

	LOG_S(9) << "data" << jdata;
	LOG_S(INFO) << GetCommaSeparatedValues(jdata);

	// we need to make a list with the deviceIds
	// then compose a csv file name with devId+datetime
	// if this file exist, append data 
 // 	else, create file and add the format on the first line ie [noise_rms], [noise_peak], [noise_base], [pir_detections], [pir_lastHourD], [light_last], [light_average]

	return 0;
}

int CsvLogger::CreateFile(std::string deviceId, json jdata)
{
	uThing_t uThing;

	uThing.filePath = GetFullPath(deviceId);

	LOG_S(INFO) << "New device detected, writing data in: "<< uThing.filePath;

	uThing.startTime = std::chrono::system_clock::now();

	try {
		fs::create_directory(m_logPath);
	}
	catch (const std::exception& ex) {
		LOG_S(WARNING) << "Error trying to create the configured directory: " << ex.what();
	}

	std::string dataHeader = GetFormatHeader(jdata);
	LOG_S(5) << "CSV Data Header: "<< dataHeader;

	uThing.streamFile.open (uThing.filePath);
	uThing.streamFile << dataHeader;
	uThing.streamFile.close();

	devices.emplace(deviceId, std::move(uThing));

	return 0;
}

std::string CsvLogger::GetCommaSeparatedValues(json& jdata) {

	// jdata = R"({"noise":{"rms":33.5,"peak":33.5,"base":30.4},"pir":{"detections":0,"detPerHour":6},"light":{"last":58.13,"average":51.67}})"_json;
	json flat_jdata = jdata.flatten();
	/* Flat the entire json objects so we can easily iterate it, 
	for example:
		{"noise":{"rms":33.5,"peak":33.5,"base":30.4},"pir":{"detections":0,"detPerHour":6},"light":{"last":58.13,"average":51.67}}
	after json.flatten():
 		{"/light/average":51.67,"/light/last":58.13,"/noise/base":30.4,"/noise/peak":33.5,"/noise/rms":33.5,"/pir/detPerHour":6,"/pir/detections":0}
	*/
	std::string line;

	for (auto& [key, value] : flat_jdata.items()) {
  		line += value.dump() + ","; //Field/s
	}

	//Add timestamp depending on the configured format:
	if (m_outTimestampInMs) {
		line += std::to_string(GetTimestampInMs());
	} else {
		line += GetCurrentTimeForLogging();
	}

	LOG_S(6) << "CSV data: "<< line;

	return line;
}

std::string CsvLogger::GetFormatHeader(json& jdata) {

	// jdata = R"({"noise":{"rms":33.5,"peak":33.5,"base":30.4},"pir":{"detections":0,"detPerHour":6},"light":{"last":58.13,"average":51.67}})"_json;
	json flat_jdata = jdata.flatten();
	/* Flat the entire json objects so we can easily iterate it, 
	for example:
		{"noise":{"rms":33.5,"peak":33.5,"base":30.4},"pir":{"detections":0,"detPerHour":6},"light":{"last":58.13,"average":51.67}}
	after json.flatten():
 		{"/light/average":51.67,"/light/last":58.13,"/noise/base":30.4,"/noise/peak":33.5,"/noise/rms":33.5,"/pir/detPerHour":6,"/pir/detections":0}
	*/
	std::string line;

	for (auto& [key, value] : flat_jdata.items()) {
  		LOG_S(9) << key;
  		line += ReformatKey(key)+","; //Field/s
	}
	line += "timestamp\n";

	LOG_S(4) << "CSV header: "<< line;

	return line;
}

std::string CsvLogger::ReformatKey(std::string key) {
	//example flattened keys: "/light/average", "/temperature"
	key.erase(key.begin()); //erase the 1st char ("/")

	replace(key.begin(), key.end(), '/', '.' );

	return key;
}

