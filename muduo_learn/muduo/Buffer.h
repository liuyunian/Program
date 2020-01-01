#ifndef BUFFER_H_
#define BUFFER_H_

#include <string>
#include <vector>
#include <algorithm>  // std::copy

#include <assert.h>   // assert

#include <tools/base/copyable.h>

static const size_t kCheapPrepend = 8;
static const size_t kInitialSize = 1024;

class Buffer : copyable {
public:
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

  const char* readable_index() const {
    return m_buffer.data() + m_readerIndex;
  }

  char* writable_index(){
    return m_buffer.data() + m_writerIndex;
  }

  void adjust_writer_index(size_t len){
    m_writerIndex += len;
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

  std::string retrieve_as_string(size_t len){
    assert(len <= readable_bytes());
    std::string result(readable_index(), len);
    retrieve(len);
    return result;
  }

  std::string retrieve_all_as_string(){
    return retrieve_as_string(readable_bytes());
  }

  void retrieve_all(){
    m_readerIndex = kCheapPrepend;
    m_writerIndex = kCheapPrepend;
  }


  // 填充数据
  void append(const std::string &str){
    append(str.c_str(), str.size());
  }

  void append(const char *data, size_t len){
    ensure_writeable_bytes(len);
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

  void ensure_writeable_bytes(size_t len){
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

private:
  void make_space(size_t len){
    if(writable_bytes()+prependable_bytes() < len + kCheapPrepend){
      m_buffer.resize(m_writerIndex+len);
    }
    else{
      assert(kCheapPrepend < m_readerIndex);
      size_t readable = readable_bytes();
      std::copy(m_buffer.data()+m_readerIndex,
                writable_index(),
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