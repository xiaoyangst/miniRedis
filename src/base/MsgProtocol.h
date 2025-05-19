/**
  ******************************************************************************
  * @file           : MsgProtocol.h
  * @author         : xy
  * @brief          : 发送消息的封装与接受消息的解析
  * @attention      : 头部 + 消息主体 。头部为八字节，记录消息体的长度
  * @date           : 2025/5/19
  ******************************************************************************
  */

#ifndef MINIREDIS_SRC_BASE_MSGPROTOCOL_H_
#define MINIREDIS_SRC_BASE_MSGPROTOCOL_H_

#include <string>
class MsgProtocol {
 public:
  static std::string pack(const std::string& msg);
  static std::string unpack(const std::string& msg);
};

#endif //MINIREDIS_SRC_BASE_MSGPROTOCOL_H_
