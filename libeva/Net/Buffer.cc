#include "libeva/Net/Buffer.h"
#include <errno.h>
#include <sys/uio.h>

namespace libeva {

const char Buffer::kCRLF[] = "\r\n";

// TODO: 
ssize_t Buffer::readFd( int fd, int *savedErr ) {

	char extrabuf[65536];
	struct iovec vec[2];
	const size_t writable = writableBytes();
	vec[0].iov_base = begin()+writerIndex_;
	vec[0].iov_len = writable;
	vec[1].iov_base = extrabuf;
	vec[1].iov_len = sizeof extrabuf;
	// when there is enough space in this buffer, don't read into extrabuf.
	// when extrabuf is used, we read 128k-1 bytes at most.
	const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
	const ssize_t n = ::readv(fd, vec, iovcnt);
	if (n < 0)
	{
	  *savedErr = errno;
	}
	else if ( static_cast<size_t>(n) <= writable)
	{
	  writerIndex_ += n;
	}
	else
	{
	  writerIndex_ = buffer_.size();
	  append(extrabuf, n - writable);
	}
	return n;
	}

}
