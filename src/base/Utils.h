/**
  ******************************************************************************
  * @file           : Utils.h
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/5/21
  ******************************************************************************
  */

#ifndef MINIREDIS_SRC_BASE_UTILS_H_
#define MINIREDIS_SRC_BASE_UTILS_H_

class Utils {
 public:
  static struct sockaddr_in getLocalAddr(int sockfd);
  static struct sockaddr_in getPeerAddr(int sockfd);
};

#endif //MINIREDIS_SRC_BASE_UTILS_H_
