/**
  ******************************************************************************
  * @file           : Timestamp.h
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/5/23
  ******************************************************************************
  */

#ifndef MINIREDIS_SRC_BASE_TIMESTAMP_H_
#define MINIREDIS_SRC_BASE_TIMESTAMP_H_

#include <iostream>
#include <string>

class Timestamp
{
 public:
  Timestamp();
  explicit Timestamp(int64_t microSecondsSinceEpoch);
  static Timestamp now();
  std::string toString() const;
 private:
  int64_t microSecondsSinceEpoch_;
};

#endif //MINIREDIS_SRC_BASE_TIMESTAMP_H_
