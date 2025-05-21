/**
  ******************************************************************************
  * @file           : public.h
  * @author         : xy
  * @brief          : 公共定义
  * @attention      : None
  * @date           : 2025/5/19
  ******************************************************************************
  */

#ifndef MINIREDIS_SRC_BASE_PUBLIC_H_
#define MINIREDIS_SRC_BASE_PUBLIC_H_

#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

using namespace std::placeholders;

class TcpConnection;
class Buffer;

constexpr int MAX_MSG_LEN = 1024 * 1024;
constexpr int kInitEventListSize = 16;
constexpr int kEpollTimeMs = 10000;

constexpr int kNew = -1;
constexpr int kAdded = 1;
constexpr int kDeleted = 2;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using TimerCallback = std::function<void()>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
using CloseCallback = std::function<void(const TcpConnectionPtr &)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr &)>;
using HighWaterMarkCallback = std::function<void(const TcpConnectionPtr &, size_t)>;
using MessageCallback = std::function<void(const TcpConnectionPtr &,Buffer *)>;

void defaultConnectionCallback(const TcpConnectionPtr& conn);
void defaultMessageCallback(const TcpConnectionPtr& conn,
							Buffer* buffer);

using EventCallback = std::function<void()>;
using ReadEventCallback = std::function<void()>;



#endif //MINIREDIS_SRC_BASE_PUBLIC_H_
