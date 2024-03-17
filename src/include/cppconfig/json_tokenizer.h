// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023-2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#ifndef __CPP_CONFIG_JSON_TOKENIZER_H__
#define __CPP_CONFIG_JSON_TOKENIZER_H__
#include <optional>

#include <cppconfig/json_buffer.h>
#include <cppconfig/json_token.h>


namespace cppconfig::json {

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

    /// @brief Retrieves the current line.
    /// @return The current line number.
    inline size_t line() const { return _buffer.line(); }

    /// @brief Retrieves the current column.
    /// @return The current column number.
    inline size_t column() const { return _buffer.column(); }

    /// @brief Retrieves the last error.
    /// @return  The last error.
    inline Error error() const { return _error; }

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