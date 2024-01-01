// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023-2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#ifndef __CPP_CONFIG_JSON_PARSER_H__
#define __CPP_CONFIG_JSON_PARSER_H__
#include <filesystem>
#include <optional>
#include <map>

#include <cppconfig/json_tokenizer.h>
#include <cppconfig/json_value.h>


namespace cppconfig::json {

class JsonParser {
  public:
    enum class ErrorCode {
      kNoError = 0,
      kPrematureEnd,
      kInvalidEscape,
      kExpectAny, // str|number|null|boolean|{|[
      kExpectObject, // {|[
      kExpectPair, // key
      kExpectCommaOrEndObj, // ,|}
      kExpectCommaOrEndArray,  // ,|]
    };

    struct Error {
      size_t line { 0 };
      size_t column { 0 };
      ErrorCode code { ErrorCode::kNoError};
    };

    std::optional<JsonValue> parse (const char *buffer, size_t size);

    inline const Error & error() const { return _error; }

  private:
    std::unique_ptr<JsonTokenizer> _tokenizer;
    Error _error {};

    std::optional<JsonValue> _parseObject ();
    std::optional<JsonValue> _parseArray ();

    inline std::optional<JsonValue> _setError (ErrorCode code) {
      _error.code = code;
      _error.line = _tokenizer->line();
      _error.column = _tokenizer->column();
      return std::nullopt;
    }

};

}

#endif