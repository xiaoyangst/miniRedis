/**
  ******************************************************************************
  * @file           : Logger.h
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/5/23
  ******************************************************************************
  */

#ifndef MINIREDIS_SRC_BASE_LOGGER_H_
#define MINIREDIS_SRC_BASE_LOGGER_H_

#include <string>

#define LOG_INFO(logMsgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance(); \
        logger.setLogLevel(INFO); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logMsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while(0)

#define LOG_ERROR(logMsgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance(); \
        logger.setLogLevel(ERROR); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logMsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while(0)

#define LOG_FATAL(logMsgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance(); \
        logger.setLogLevel(FATAL); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logMsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
        exit(-1); \
    } while(0)

#ifdef MUDEBUG
#define LOG_DEBUG(logMsgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance(); \
        logger.setLogLevel(DEBUG); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logMsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while(0)
#else
#define LOG_DEBUG(logMsgFormat, ...)
#endif

enum LogLevel
{
  INFO,  // 普通信息
  ERROR, // 错误信息
  FATAL, // core信息
  DEBUG, // 调试信息
};

class Logger {
 public:
  static Logger& instance();
  void setLogLevel(int level);
  // 写日志
  void log(std::string msg);
 private:
  std::string logLevelToString(int level);
 private:
  int logLevel_;
};

#endif //MINIREDIS_SRC_BASE_LOGGER_H_
