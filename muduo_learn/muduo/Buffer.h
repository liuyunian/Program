#ifndef BUFFER_H_
#define BUFFER_H_

#include <string>
#include <algorithm>  // std::copy

#include <assert.h>   // assert

#include <tools/base/copyable.h>

class Buffer : copyable {
public:
  static const size_t kCheapPrepend = 8;
  static const size_t kInitialSize = 1024;

  explicit Buffer(size_t initialSize = kInitialSize) : 
    m_buffer(kCheapPrepend + kInitialSize),
    m_readerIndex(kCheapPrepend),
    m_writerIndex(kCheapPrepend)
  {}

  // implicit copy-ctor, move-ctor, dtor and assignment are fine

  size_t prependable_bytes() const {
    return m_readerIndex;
  }

  size_t readable_bytes() const {
    return m_writerIndex - m_readerIndex;
  }

  size_t writable_bytes() const {
    return m_buffer.size() - m_writerIndex;
  }

  // 从readable区域读走数据之后，调用retrieve()，readerIndex右移
  void retrieve(size_t len){
    assert(len <= readable_bytes());
    if(len < readable_bytes()){
      m_readerIndex += len;
    }
    else{
      retrieve_all();
    }
  }

  // void retrieve_until(const char* end);
  // void retrieve_int64();
  // void retrieve_int32();
  // void retrieve_int16();
  // void retrieve_int8();

  void retrieve_all(){
    m_readerIndex = kCheapPrepend;
    m_writerIndex = kCheapPrepend;
  }

  // 填充数据
  void append(const std::string &str){
    append(str.c_str(), str.size());
  }

  void append(const char *data, size_t len){
    ensure_writeable_bytes();
    std::copy(data, data+len, m_buffer.data());
    has_written(len);
  }

  void append(const void *data, size_t len){
    append(static_cast<const char*>(data), len);
  }

  // void append_int64(int64_t x);
  // void append_int32(int32_t x);
  // void append_int16(int16_t x);
  // void append_int8(int8_t x);

  void ensure_writeable_bytes(){
    if(writable_bytes() < len){
      make_space(len);
    }

    assert(writable_bytes() >= len);
  }

  void has_written(size_t len){
    m_writerIndex += len;
  }

  // 从networt endian读取数据
  // int64_t read_int64();
  // int32_t read_int32();
  // int16_t read_int16();
  // int8_t read_int8();

  const char* peek() const {
    return m_buffer.data() + m_readerIndex;
  }
  // int64_t peek_int64();
  // int32_t peek_int32();
  // int16_t peek_int16();
  // int8_t peek_int8();

  // 预留数据
  // void prepend(const void *data, size_t len);
  // void prepend_int64(int64_t x);
  // void prepend_int32(int32_t x);
  // void prepend_int16(int16_t x);
  // void prepend_int8(int8_t x);

  /**
   * @brief 从socket读取数据到buffer
  */
  ssize_t read_fd(int fd, int *savedErrno);

private:
  void make_space(){
    if(writable_bytes()+prependable_bytes() < len + kCheapPrepend){
      m_buffer.resize(m_writerIndex+len);
    }
    else{
      assert(kCheapPrepend < m_readerIndex);
      size_t readable = readable_bytes();
      std::copy(m_buffer.data()+m_readerIndex,
                m_buffer.data()+m_writerIndex,
                m_buffer.data()+kCheapPrepend);
      m_readerIndex = kCheapPrepend;
      m_writerIndex = m_readerIndex + readable;
      assert(readable == readable_bytes());
    }
  }

private:
  std::vector<char> m_buffer;
  size_t m_readerIndex;
  size_t m_writerIndex;
};

#endif // BUFFER_H_