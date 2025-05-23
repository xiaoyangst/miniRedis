/**
  ******************************************************************************
  * @file           : Logger.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/5/23
  ******************************************************************************
  */

#include "Logger.h"
#include "Timestamp.h"
#include "format"

#include <iostream>

Logger &Logger::instance() {
	static Logger logger;
	return logger;
}

void Logger::setLogLevel(int level) {
	logLevel_ = level;
}

// 写日志  [级别信息] time : msg
void Logger::log(std::string msg) {
	std::cout << std::format("{} {}:{}",
							 logLevelToString(logLevel_),
							 Timestamp::now().toString(), msg) << std::endl;
}

std::string Logger::logLevelToString(int level) {
	std::string str;
	switch (level) {
		case INFO: str = "[INFO]";
			break;
		case ERROR: str = "[ERROR]";
			break;
		case FATAL: str = "[FATAL]";
			break;
		case DEBUG: str = "[DEBUG]";
			break;
		default: break;
	}
	return str;
}