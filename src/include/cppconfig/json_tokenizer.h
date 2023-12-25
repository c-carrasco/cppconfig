// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023 Carlos Carrasco
// ----------------------------------------------------------------------------
#ifndef __CPP_CONFIG_JSON_TOKENIZER_H__
#define __CPP_CONFIG_JSON_TOKENIZER_H__
#include <optional>
#include <ostream>
#include <string>
#include <string_view>

#include <cppconfig/json_buffer.h>

namespace cppconfig::json {

/// Enumeration representing different types of JSON tokens.
enum class JsonTokenId {
  kObjectBegin,       ///< Indicates the beginning of a JSON object.
  kObjectEnd,         ///< Indicates the end of a JSON object.
  kArrayBegin,        ///< Indicates the beginning of a JSON array.
  kArrayEnd,          ///< Indicates the end of a JSON array.
  kColon,             ///< Represents the colon character (":") in JSON.
  kComma,             ///< Represents the comma character (",") in JSON.
  kValueString,       ///< Represents a JSON string value.
  kValueFloatPoint,   ///< Represents a JSON floating-point value.
  kValueInteger,      ///< Represents a JSON integer value.
  kValueBoolean,      ///< Represents a JSON boolean value.
  kValueNull,         ///< Represents a JSON null value.
  kError,             ///< Represents a JSON error.
  kEmpty              ///< Represents a unitialized token id.
};

/// @brief Represents a JSON token with various data types.
class JsonToken {
  public:
    /// Constructs a JsonToken with the specified id.
    /// @param id The JsonTokenId for the token.
    inline JsonToken (JsonTokenId id) noexcept: _id { id } {
      // empty
    }

    /// Constructs a JsonToken with a boolean value.
    /// @param v The boolean value for the token.
    inline JsonToken (bool v) noexcept: _id { JsonTokenId::kValueBoolean }, _value { v } {
      // empty
    }

    /// Constructs a JsonToken with a floating-point value.
    /// @param v The double value for the token.
    inline JsonToken (double v) noexcept: _id { JsonTokenId::kValueFloatPoint }, _value { v } {
      // empty
    }

    /// Constructs a JsonToken with an integer value.
    /// @param v The int64_t value for the token.
    inline JsonToken (int64_t v) noexcept: _id { JsonTokenId::kValueInteger }, _value { v } {
      // empty
    }

    /// Constructs a JsonToken with a string value.
    /// @param v The string value for the token.
    inline  JsonToken (const std::string_view &v):
      _id { JsonTokenId::kValueString },
      _value { std::string { v.begin(), v.end() } }
    {
      // empty
    }

    /// Returns the JsonTokenId of the token.
    /// @return The JsonTokenId of the token.
    inline JsonTokenId id() const { return _id; }

    /// Returns the value of the token.
    /// @tparam T The type of the value to retrieve.
    /// @return The value of the token as the specified type.
    /// @note This function performs a static assertion to ensure the requested type is valid.
    template<typename T>
    const T & value() const {
      static_assert(
        std::is_same_v<T, std::variant_alternative_t<0, decltype(_value)>> ||
        std::is_same_v<T, std::variant_alternative_t<1, decltype(_value)>> ||
        std::is_same_v<T, std::variant_alternative_t<2, decltype(_value)>> ||
        std::is_same_v<T, std::variant_alternative_t<3, decltype(_value)>>
      );

      return std::get<T> (_value);
    }

    /// Overloaded stream insertion operator for outputting JsonToken to a stream.
    /// @param os The output stream.
    /// @param obj The JsonToken object to output.
    /// @return The output stream.
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
    JsonTokenId _id; ///< The JsonTokenId of the token.
    std::variant<std::string, bool, int64_t, double> _value; ///< The value of the token, stored as a variant.
};

/// JsonTokenizer class for tokenizing JSON input.
class JsonTokenizer {
  public:
    /// Enumeration representing possible errors during JSON tokenization.
    enum class Error {
      kNoError,            ///< No error occurred during tokenization.
      kPrematureEnd  = 1,  ///< Premature end of data during tokenization.
      kInvalidEscape = 2   ///< Invalid escape sequence in a string.
    };

    /// @brief Constructor for JsonTokenizer.
    /// @param buffer The Buffer containing the JSON data to tokenize.
    JsonTokenizer (Buffer &&buffer): _buffer { std::move (buffer) } {
      // empty
    }

    /// @brief Retrieves the next JSON token from the input data.
    /// @return An optional JsonToken, or std::nullopt if no more tokens are available or an error occurs.
    std::optional<JsonToken> next();

    /// @brief Retrieves the current line
    size_t line() const { return _buffer.line(); }

    /// @brief Retrieves the current column
    size_t column() const { return _buffer.column(); }

    Error error() const { return _error; }

  private:
    Buffer _buffer;  ///< The Buffer containing the JSON data to tokenize.
    Error _error { Error::kNoError };  ///< The current error state during tokenization.

    /// Handles a number.
    /// @return A JsonToken representing the number.
    JsonToken _handleNumber ();

    /// Handles a string.
    /// @return A JsonToken representing the string.
    JsonToken _handleString ();

    /// Sets the error state and returns a JsonToken representing the error.
    /// @param err The error code to set.
    /// @return A JsonToken representing the error.
    inline JsonToken _setError (Error err) {
      _error = err;
      return JsonToken { JsonTokenId::kError };
    }
};

}

#endif