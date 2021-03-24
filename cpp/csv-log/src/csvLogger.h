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
#include <filesystem>
#include <fstream>

#include <json.hpp>


struct csvConfig_t
{
	std::string logPath;
	int maxDatapointsPerFile = 10000;
	bool useGmtTime = true;

	/* default timestapmp format is unix epoch in ms, 
	 if false use date-time (UTC or local depending on "useGmtTime" */
	bool outTimestampInMs = true; 
};

struct uThing_t
{
	// std::string deviceId;
	std::filesystem::path filePath;
	std::ofstream streamFile;
	std::chrono::time_point<std::chrono::system_clock> startTime;
	int datapointsWritten = 0;
};

class CsvLogger {
public:
	CsvLogger(csvConfig_t&);
	// int CheckReadiness(void);

	int Write(std::string deviceId, nlohmann::json jdata);

private:
	int CreateFile(std::string deviceId, nlohmann::json jdata);
	std::string GetCurrentTimeForFileName();
	std::string GetCurrentTimeForLogging();

	std::string GetCommaSeparatedValues(nlohmann::json& jdata);
	std::filesystem::path AppendTimeToFileName(const std::filesystem::path& fileName);
	std::filesystem::path GetFullPath(std::string deviceId);
	long GetTimestampInMs();
	std::string GetFormatHeader(nlohmann::json& jdata);
	std::string ReformatKey(std::string key);



private:
	std::string m_logPath;
	int m_maxDatapointsPerFile;
	bool m_useGmtTime;
	bool m_outTimestampInMs;
	std::map<std::string, uThing_t> devices;
};