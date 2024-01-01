// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023-2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#ifndef __CPP_CONFIG_JSON_VALUE_H__
#define __CPP_CONFIG_JSON_VALUE_H__
#include <cassert>
#include <map>
#include <string>
#include <vector>

#include <cppconfig/json_tokenizer.h>


namespace cppconfig::json {

/// @brief Represents a JSON value, which can be a boolean, integer, float, string, null, object, or array.
class JsonValue {
  public:
    /// @brief Constructs a JSON value from a JSON token (move semantics).
    /// @param token The JSON token.
    JsonValue (JsonToken &&token): _token { std::move (token) } {
      // empty
    }

    /// @brief Constructs a JSON object value from a map of string to JSON values (move semantics).
    /// @param map The map representing the JSON object.
    JsonValue (std::map<std::string, JsonValue> &&map): _token { JsonTokenId::kObjectBegin }, _map { std::move (map) } {
      // empty
    }

    /// @brief Constructs a JSON array value from a vector of JSON values (move semantics).
    /// @param array The vector representing the JSON array.
    JsonValue (std::vector<JsonValue> &&array): _token { JsonTokenId::kArrayBegin }, _array { std::move (array) } {
      // empty
    }

    /// @brief Checks if the JSON value is a boolean.
    inline bool isBool() const { return _token.id() == JsonTokenId::kValueBoolean; }

    /// @brief Checks if the JSON value is an integer.
    inline bool isInt() const { return _token.id() == JsonTokenId::kValueInteger; }

    /// @brief Checks if the JSON value is a floating-point number.
    inline bool isFloat() const { return _token.id() == JsonTokenId::kValueFloatPoint; }

    /// @brief Checks if the JSON value is a string.
    inline bool isString() const { return _token.id() == JsonTokenId::kValueString; }

    /// @brief Checks if the JSON value is null.
    inline bool isNull() const { return _token.id() == JsonTokenId::kValueNull; }

    /// @brief Checks if the JSON value is an object.
    inline bool isObject() const { return _token.id() == JsonTokenId::kObjectBegin; }

    /// @brief Checks if the JSON value is an array.
    inline bool isArray() const { return _token.id() == JsonTokenId::kArrayBegin; }

    /// @brief Checks if the JSON value is empty.
    inline bool empty() const { return _token.id() == JsonTokenId::kEmpty; }

    /// @brief Checks if a key exists in the JSON object.
    /// @param k The key to check.
    /// @return True if the key exists, false otherwise.
    inline bool exists(const std::string &k) const {
      return _map.find(k) != _map.end();
    }

    /// @brief Gets the stored value as a specific type.
    /// \tparam T The type to retrieve.
    /// @return A const reference to the stored value.
    template<typename T>
    inline const T & get () const {
      if constexpr (std::is_same_v<T, std::map<std::string, JsonValue>>) {
        return _map;
      }
      else if constexpr (std::is_same_v<T, std::vector<JsonValue>>) {
        return _array;
      }
      else {
        return _token.value<T> ();
      }
    }

    /// @brief Provides access to the value associated with a key in a JSON object.
    /// @param k The key.
    /// @return A reference to the JSON value associated with the key.
    JsonValue & operator[] (const std::string &k) {
      assert (_token.id() == JsonTokenId::kObjectBegin);
      return _map.find (k)->second;
    }

    /// @brief Provides const access to the value associated with a key in a JSON object.
    /// @param k The key.
    /// @return A const reference to the JSON value associated with the key.
    const JsonValue & operator[] (const std::string &k) const {
      assert (_token.id() == JsonTokenId::kObjectBegin);
      return _map.find (k)->second;
    }

    /// @brief Provides access to the value at a specific index in a JSON array.
    /// @param i The index.
    /// @return A reference to the JSON value at the specified index.
    JsonValue & operator[] (size_t i) {
      assert (_token.id() == JsonTokenId::kArrayBegin);
      return _array[i];
    }

    /// @brief Provides const access to the value at a specific index in a JSON array.
    /// @param i The index.
    /// @return A const reference to the JSON value at the specified index.
    const JsonValue & operator[] (size_t i) const {
      assert (_token.id() == JsonTokenId::kArrayBegin);
      return _array[i];
    }

    /// @brief Gets the stored value as a boolean.
    inline bool asBool() const { return get<bool>(); }

    /// @brief Gets the stored value as an integer.
    inline int64_t asInt() const { return get<int64_t>(); }

    /// @brief Gets the stored value as a floating-point number.
    inline double asFloat() const { return get<double>(); }

    /// @brief Gets the stored value as a string.
    inline const std::string & asString() const { return get<std::string>(); }

    /// @brief Gets the stored value as a JSON object.
    inline const std::map<std::string, JsonValue> & asObject() const { return get<std::map<std::string, JsonValue>>(); }

    /// @brief Gets the stored value as a JSON array.
    inline const std::vector<JsonValue> & asArray() const { return get<std::vector<JsonValue>>(); }

  private:
    JsonToken _token; ///< The underlying JSON token.
    std::map<std::string, JsonValue> _map; ///< Map representation for JSON object.
    std::vector<JsonValue> _array; ///< Vector representation for JSON array.
};

}

#endif
