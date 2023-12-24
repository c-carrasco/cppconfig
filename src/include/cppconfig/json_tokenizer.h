// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023 Carlos Carrasco
// ----------------------------------------------------------------------------
#ifndef __CPP_CONFIG_JSON_TOKENIZER_H__
#define __CPP_CONFIG_JSON_TOKENIZER_H__
#include <cstdlib>
#include <charconv>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>


namespace cppconfig::json {

class Buffer {
  public:
    Buffer (std::unique_ptr<char []> &&d, size_t len): _data { std::move (d) }, _size { len } {
      // empty
    }

    inline char next() {
      return _data.get()[_idx++];
    }


    inline bool endOfData() const {
      return _idx >= _size;
    }

    inline bool match (const char *mem, size_t count) const {
      if (_idx + count < _size)
        return std::memcmp (_data.get() + _idx, mem, count) == 0;

      return false;
    }

    inline bool match (const std::function<bool (char)> &f) const {
      return (_idx + 1 < _size) && f (_data.get()[_idx + 1]);
    }

    inline std::pair<int32_t, std::string_view> find (char ch) const {
      if (const auto it = std::find (_data.get() + _idx, _data.get() + _size, ch); *it == ch) {
        const auto len { it - (_data.get() + _idx) };
        return std::make_pair (len, std::string_view { _data.get() + _idx, static_cast<size_t> (len) });
      }

      return std::make_pair (-1, std::string_view {});
    }

    inline  size_t count (const std::function<bool (char)> &cb) const {
      size_t counter { 0 };
      for (size_t i = _idx; i < _size; ++i) {
        if (!cb (_data.get()[i]))
          break;

        ++counter;
      }

      return counter;
    }

    inline void forward (size_t len) {
      _idx += len;
    }

    inline const char * current() const { return _data.get() + _idx; }

    inline const char * end() const { return _data.get() + _size; }

  private:
    std::unique_ptr<char []> _data;
    size_t _size { 0 };
    size_t _idx { 0 };
};

enum class JsonTokenId {
  kObjectBegin,
  kObjectEnd,
  kArrayBegin,
  kArrayEnd,
  kColon,
  kComma,
  kValueString,
  kValueFloatPoint,
  kValueInteger,
  kValueBoolean,
  kValueNull,
  kError
};

class JsonToken {
  public:
    inline JsonToken (JsonTokenId id) noexcept: _id { id } {
      // empty
    }

    inline JsonToken (bool v) noexcept: _id { JsonTokenId::kValueBoolean }, _value { v } {
      // empty
    }

    inline JsonToken (double v) noexcept: _id { JsonTokenId::kValueFloatPoint }, _value { v } {
      // empty
    }

    inline JsonToken (int64_t v) noexcept: _id { JsonTokenId::kValueInteger }, _value { v } {
      // empty
    }

    inline  JsonToken (const std::string_view &v):
      _id { JsonTokenId::kValueString },
      _value { std::string { v.begin(), v.end() } }
    {
      // empty
    }

    inline JsonTokenId id() const { return _id; }

    template<typename T>
    T value() const {
      static_assert(
        std::is_same_v<T, std::variant_alternative_t<0, decltype(_value)>> ||
        std::is_same_v<T, std::variant_alternative_t<1, decltype(_value)>> ||
        std::is_same_v<T, std::variant_alternative_t<2, decltype(_value)>> ||
        std::is_same_v<T, std::variant_alternative_t<3, decltype(_value)>>
      );

      return std::get<T> (_value);
    }

    friend std::ostream & operator<< (std::ostream &os, const JsonToken &obj) {
      switch (obj._id) {
        case JsonTokenId::kObjectBegin: os << "ObjectBegin"; break;
        case JsonTokenId::kObjectEnd: os << "ObjectEnd"; break;
        case JsonTokenId::kArrayBegin: os << "ArrayBegin"; break;
        case JsonTokenId::kArrayEnd: os << "ArrayEnd"; break;
        case JsonTokenId::kColon: os << "Colon"; break;
        case JsonTokenId::kComma: os << "Comma"; break;
        case JsonTokenId::kValueString: os << "ValueString[" << std::get<std::string> (obj._value) << "]"; break;
        case JsonTokenId::kValueFloatPoint: os << "kValueFloatPoint[" << std::get<double> (obj._value) << "]"; break;
        case JsonTokenId::kValueInteger: os << "kValueInteger[" << std::get<int64_t> (obj._value) << "]"; break;
        case JsonTokenId::kValueBoolean: os << "ValueBoolean[" << std::get<bool> (obj._value) << "]"; break;
        case JsonTokenId::kValueNull: os << "ValueNull"; break;
        case JsonTokenId::kError: os << "Error"; break;
        default: os << "(UNKNOWN)";
      }

      return os;
    }

  private:
    JsonTokenId _id;
    std::variant<std::string, bool, int64_t, double> _value;
};

class JsonTokenizer {
  public:
    enum class Error {
      kNoError,
      kPrematureEnd
    };

    JsonTokenizer (Buffer &&buffer): _buffer { std::move (buffer) } {
      // empty
    }

    std::optional<JsonToken> next() {
      while (!_buffer.endOfData()) {
        const auto c { _buffer.next() };

        switch (c) {
          case '{': return JsonToken { JsonTokenId::kObjectBegin };
          case '}': return JsonToken { JsonTokenId::kObjectEnd };
          case '[': return JsonToken { JsonTokenId::kArrayBegin };
          case ']': return JsonToken { JsonTokenId::kArrayEnd };
          case ':': return JsonToken { JsonTokenId::kColon };
          case ',': return JsonToken { JsonTokenId::kComma };
          case 'n':
            if (_buffer.match ("ull", 3)) {
              _buffer.forward (3);
              return JsonToken {  JsonTokenId::kValueNull };
            }

            return _setError (Error::kPrematureEnd);
          case 't':
            if (_buffer.match ("rue", 3)) {
              _buffer.forward (3);
              return JsonToken { true };
            }

            return _setError (Error::kPrematureEnd);
          case 'f':
            if (_buffer.match ("alse", 4)) {
              _buffer.forward (4);
              return JsonToken { false };
            }

            return _setError (Error::kPrematureEnd);
          case '"':
            // TODO: handle '\' + "|\|/|\|b|f|n|r|t|u(4 hex digit)
            if (const auto [ pos, str ] = _buffer.find ('"'); pos > -1) {
              _buffer.forward (pos + 1);
              return JsonToken { str };
            }

            return _setError (Error::kPrematureEnd);
          case ' ':
          case '\t':
          case '\n':
          case '\r':
          case '\0':
            break;
          default:
            if ((c == '-') || std::isdigit (c)) {
              bool isFp { false };

              const size_t len = _buffer.count ([ &isFp ] (const char c) {
                switch (c) {
                  case '.':
                    isFp = true;
                    break;
                  case ' ':
                  case ',':
                  case ']':
                  case '}':
                  case '\n':
                    return false;
                }
                return true;
              });

              if (isFp) {
#if __has_feature(__cpp_lib_constexpr_charconv)
                double value {};
                const auto r { std::from_chars (_buffer.current() - 1, _buffer.current() + len, value) };
                if (r.ptr == _buffer.current() + len) {
                  _buffer.forward (len);
                  return JsonToken { value };
                }
#else
                char * end = const_cast<char *> (_buffer.current() + len);
                const auto value { std::strtod (_buffer.current() - 1, &end) };
                if ((value != 0) || (end != _buffer.current())) {
                  _buffer.forward (len);
                  return JsonToken { value };
                }
#endif
              }
              else {
                int64_t value {};
                const auto r { std::from_chars (_buffer.current() - 1, _buffer.end(), value) };
                if (r.ptr == _buffer.current() + len) {
                  _buffer.forward (len);
                  return JsonToken { value };
                }
              }
            }

            // https://www.tutorialspoint.com/json/json_data_types.htm

            return _setError (Error::kPrematureEnd);
        }
      }

      return std::nullopt;
    }

  private:
    Buffer _buffer;
    Error _error { Error::kNoError };

    JsonToken _setError (Error err) {
      _error = err;
      return JsonToken { JsonTokenId::kError };
    }
};

}

#endif