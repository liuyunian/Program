#include <errno.h>
#include <sys/uio.h>

#include "muduo/Buffer.h"

const size_t Buffer::kCheapPrepend;
const size_t Buffer::kInitialSize;

ssize_t Buffer::read_fd(int fd, int *savedErrno){
  // saved an ioctl()/FIONREAD call to tell how much to read
  char extrabuf[65536];
  struct iovec vec[2];
  const size_t writable = writable_bytes();
  vec[0].iov_base = m_buffer.data()+writerIndex_;
  vec[0].iov_len = writable;
  vec[1].iov_base = extrabuf;
  vec[1].iov_len = sizeof extrabuf;
  // when there is enough space in this buffer, don't read into extrabuf.
  // when extrabuf is used, we read 128k-1 bytes at most.
  const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
  const ssize_t len = sockets::readv(fd, vec, iovcnt);
  if(len < 0){
    *savedErrno = errno;
  }
  else if(static_cast<size_t>(len) <= writable){
    m_writerIndex += len;
  }
  else{
    m_writerIndex = buffer_.size();
    append(extrabuf, len - writable);
  }

  return len;
}

