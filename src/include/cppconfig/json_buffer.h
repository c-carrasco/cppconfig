// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023-2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#ifndef __CPP_CONFIG_JSON_BUFFER_H__
#define __CPP_CONFIG_JSON_BUFFER_H__
#include <cstring>
#include <functional>
#include <memory>
#include <string>


namespace cppconfig::json {

class Buffer {
  public:
    Buffer (std::unique_ptr<char []> &&d, size_t len): _data { std::move (d) }, _ptr { _data.get() }, _size { len } {
      // empty
    }

    Buffer (const char *d, size_t len): _ptr { d }, _size { len } {
      // empty
    }

    inline char next() {
      const auto c { _ptr[_idx++] };
      if (c == '\n') {
        ++_line;
        _col = 0;
      }
      else {
        ++_col;
      }

      return c;
    }


    inline bool endOfData() const {
      return _idx >= _size;
    }

    inline bool match (const char *mem, size_t count) const {
      if (_idx + count < _size)
        return std::memcmp (_ptr + _idx, mem, count) == 0;

      return false;
    }

    inline std::string take (size_t len) const {
      if (_idx + len < _size)
        return std::string { _ptr + _idx, len };

      return std::string {};
    }

    inline  size_t count (const std::function<bool (char)> &cb) const {
      size_t counter { 0 };
      for (size_t i = _idx; i < _size; ++i) {
        if (!cb (_ptr[i]))
          break;

        ++counter;
      }

      return counter;
    }

    inline void forward (size_t len) {
      _col += len;
      _idx += len;
    }

    inline size_t line() const { return _line; }

    inline size_t column() const { return _col; }

    inline const char * current() const { return _ptr + _idx; }

    inline const char * end() const { return _ptr + _size; }

  private:
    std::unique_ptr<char []> _data;
    const char *_ptr { nullptr };
    size_t _size { 0 };
    size_t _idx { 0 };
    size_t _line { 0 };
    size_t _col { 0 };
};

}

#endif
