// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023-2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <cctype>
#include <charconv>
#include <codecvt>
#include <cstdlib>
#include <locale>

#include <cppconfig/json_tokenizer.h>


namespace cppconfig::json {

// ----------------------------------------------------------------------------
// JsonTokenizer::next
// ----------------------------------------------------------------------------
std::optional<JsonToken> JsonTokenizer::next() {
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
        return _handleString ();
      case ' ':
      case '\t':
      case '\n':
      case '\r':
      case '\0':
        break;
      default:
        if ((c == '-') || std::isdigit (c))
          return _handleNumber ();

        return _setError (Error::kPrematureEnd);
    }
  }

  return std::nullopt;
}


// ----------------------------------------------------------------------------
// JsonTokenizer::_handleString
// ----------------------------------------------------------------------------
JsonToken JsonTokenizer::_handleString () {
  std::vector<char> str {};
  bool backslash { false };
  while (!_buffer.endOfData()) {
    const char c { _buffer.next() };

    if (!backslash) {
      switch (c) {
        case '"':
          return JsonToken { std::string_view { str.data(), str.size() } };
        case '\\':
          backslash = true;
          break;
        default:
          str.push_back (c);
      }

      continue;
    }


    switch (c) {
      case 'b':
        str.push_back (0x08);
        break;
      case 'f':
        str.push_back (0x0c );
        break;
      case 'n':
        str.push_back (0x0a);
        break;
      case 'r':
        str.push_back (0x0d);
        break;
      case 't':
        str.push_back (0x09);
        break;
      case '"':
        str.push_back (0x22);
        break;
      case 'u': {
        const auto hex { _buffer.take (4) };

        if (hex.size() == 4) {
          try {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
            std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> converter;
            const std::string utf8 { converter.to_bytes(std::stoi (hex.c_str(), nullptr, 16)) };
            str.insert (str.end(), utf8.begin(), utf8.end());
            _buffer.forward(4);
#pragma GCC diagnostic pop
          }
          catch (...) {
            return _setError (Error::kInvalidEscape);
          }
        }
        break;
      }
      default:
        return _setError (Error::kInvalidEscape);
    }

    backslash = false;
  }

  return _setError (Error::kPrematureEnd);
}

// ----------------------------------------------------------------------------
// JsonTokenizer::_handleNumber
// ----------------------------------------------------------------------------
JsonToken JsonTokenizer::_handleNumber () {
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
#if defined(__GNUC__) && !defined(__llvm__)
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

  return _setError (Error::kPrematureEnd);
}

}