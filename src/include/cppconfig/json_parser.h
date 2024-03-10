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
#include <sstream>

#include <cppconfig/json_tokenizer.h>
#include <cppconfig/json_value.h>


namespace cppconfig::json {

/// @brief The JsonParser class is responsible for parsing JSON data.
class JsonParser {
  public:
    enum class ErrorCode {
      kNoError = 0,           //!< No error occurred
      kPrematureEnd,          //!< Unexpected end of input
      kInvalidEscape,         //!<Invalid escape character
      kExpectAny,             //!< Expected string, number, boolean, null, '{' or '['
      kExpectObject,          //!< Expected '{' or '['
      kExpectPair,            //!< Expected key in a key-value pair.
      kExpectCommaOrEndObj,   //!< Expected ',' or '}'
      kExpectCommaOrEndArray, //!< Expected ',' or ']'
    };

    /// @brief Structure representing an error during JSON parsing.
    struct Error {
      size_t line { 0 }; //!< Line number where the error occurred.
      size_t column { 0 }; //!< Column number where the error occurred.
      ErrorCode code { ErrorCode::kNoError }; //!< Error code indicating the type of error.

      /// @brief Converts the error information to a string representation.
      /// @return String representation of the error.
      inline std::string str() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
      }

      /// @brief Overloaded stream insertion operator for easy printing of Error objects.
      /// @param os Output stream.
      /// @param obj Error object to be printed.
      /// @return Reference to the output stream.
      friend std::ostream & operator<< (std::ostream &os, const Error &obj) {
        os << obj.line << ":" << obj.column << ": parse error - " << toCString (obj.code);
        return os;
      }

      /// @brief Converts an ErrorCode to a string representation.
      /// @param code The ErrorCode to convert.
      /// @return String representation of the ErrorCode.
      static constexpr const char * toCString (const ErrorCode code) {
        switch (code) {
          case ErrorCode::kPrematureEnd: return "premature end";
          case ErrorCode::kInvalidEscape: return "invalid escape character";
          case ErrorCode::kExpectAny: return "expected string, number, boolean, null, '{' or '['";
          case ErrorCode::kExpectObject: return "expected '{' or '['";
          case ErrorCode::kExpectPair: return "expected key";
          case ErrorCode::kExpectCommaOrEndObj: return "expected ',' or '}'";
          case ErrorCode::kExpectCommaOrEndArray: return "expected ',' or ']'";
          default: return "unknown";
        }

        return "unknown";
      }
    };

    /// @brief Parse JSON data from a buffer.
    /// @param buffer Pointer to the buffer containing JSON data.
    /// @param size Size of the buffer.
    /// @return Optional containing the parsed JSON value if successful, otherwise nullopt.
    std::optional<JsonValue> parse (const char *buffer, size_t size=0);

    /// @brief Get the last parsing error, if any.
    /// @return Reference to the last parsing error.
    inline const Error & error() const { return _error; }

  private:
    std::unique_ptr<JsonTokenizer> _tokenizer; // Tokenizer object used for JSON parsing
    Error _error {}; // Last parsing error

    /// @brief Parse a JSON object.
    std::optional<JsonValue> _parseObject ();
    /// @brief Parse a JSON array.
    std::optional<JsonValue> _parseArray ();

    /// @brief Set the last parsing error and return nullopt.
    /// @param code The ErrorCode indicating the type of error.
    /// @return Nullopt.
    inline std::optional<JsonValue> _setError (ErrorCode code) {
      _error.code = code;
      _error.line = _tokenizer->line();
      _error.column = _tokenizer->column();
      return std::nullopt;
    }

};

}

#endif