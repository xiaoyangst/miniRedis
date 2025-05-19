/**
  ******************************************************************************
  * @file           : MsgInterflow.h
  * @author         : xy
  * @brief          : send 和 recv 的封装
  * @attention      : None
  * @date           : 2025/5/19
  ******************************************************************************
  */

#ifndef MINIREDIS_SRC_BASE_MSGINTERFLOW_H_
#define MINIREDIS_SRC_BASE_MSGINTERFLOW_H_

#include <string>

class MsgInterflow {
 public:
  static bool sendMsg(int sockfd, const std::string& message);
  static bool recvMsg(int sockfd, std::string& out_message);
 private:
  static bool sendAll(int sockfd, const std::string& data);
  static bool recvN(int sockfd, std::string& out, size_t n);
};

#endif //MINIREDIS_SRC_BASE_MSGINTERFLOW_H_
