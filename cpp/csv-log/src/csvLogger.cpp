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
		uThing_t uThing;

		uThing.filePath = GetFullPath(deviceId);

		LOG_S(INFO) << "New device detected, writing data in: "<< uThing.filePath;

		uThing.startTime = std::chrono::system_clock::now();



		devices.emplace(deviceId, std::move(uThing));
	} 

	LOG_S(7) << "data" << jdata;

	// we need to make a list with the deviceIds
	// then compose a csv file name with devId+datetime
	// if this file exist, append data 
 // 	else, create file and add the format on the first line ie [noise_rms], [noise_peak], [noise_base], [pir_detections], [pir_lastHourD], [light_last], [light_average]

	return 1;
}

