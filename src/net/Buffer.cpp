/**
  ******************************************************************************
  * @file           : Buffer.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/5/23
  ******************************************************************************
  */

#include "Buffer.h"

#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>

/**
 * 从 fd 上读取数据  Poller工作在 LT 模式
 * Buffer 缓冲区是有大小的！ 但是从 fd 上读数据的时候，却不知道 tcp 数据最终的大小
 */
ssize_t Buffer::readFd(int fd, int* saveErrno)
{
	char extraBuf[65536] = {0}; // 栈上的内存空间  64K

	struct iovec vec[2];

	const size_t writable = writableBytes(); // 这是 Buffer 底层缓冲区剩余的可写空间大小
	vec[0].iov_base = begin() + writerIndex_;
	vec[0].iov_len = writable;

	vec[1].iov_base = extraBuf;
	vec[1].iov_len = sizeof extraBuf;

	const int iovcnt = (writable < sizeof extraBuf) ? 2 : 1;
	const ssize_t n = ::readv(fd, vec, iovcnt);
	if (n < 0)
	{
		*saveErrno = errno;
	}
	else if (n <= writable) // Buffer 的可写缓冲区已经够存储读出来的数据了
	{
		writerIndex_ += n;
	}
	else // extraBuf 里面也写入了数据
	{
		writerIndex_ = buffer_.size();
		append(extraBuf, n - writable);  // writerIndex_ 开始写 n - writable 大小的数据
	}

	return n;
}

ssize_t Buffer::writeFd(int fd, int* saveErrno)
{
	ssize_t n = ::write(fd, peek(), readableBytes());
	if (n < 0)
	{
		*saveErrno = errno;
	}
	return n;
}