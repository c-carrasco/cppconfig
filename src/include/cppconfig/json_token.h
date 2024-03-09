// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023-2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#ifndef __CPP_CONFIG_JSON_TOKEN_H__
#define __CPP_CONFIG_JSON_TOKEN_H__
#include <string>
#include <string_view>
#include <variant>

#include <iostream>

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
    /// Constructs an empty JsonToken.
    inline JsonToken () noexcept: _id { JsonTokenId::kEmpty }, _value { nullptr } {
      // empty
    }

    /// Constructs a JsonToken with the specified id.
    /// @param id The JsonTokenId for the token.
    inline JsonToken (JsonTokenId id) noexcept: _id { id }, _value { nullptr } {
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

    /// @brief Copy constructor for JsonToken.
    /// @param obj obj The JsonToken object to be copied.
    inline JsonToken (const JsonToken &obj): _id { obj._id }, _value { obj._value } {
      // empty
    }

    /// @brief Copy assignment operator for JsonToken.
    /// @param obj The JsonToken object to be assigned.
    /// @return A reference to the current JsonToken instance after the assignment.
    inline JsonToken & operator= (const JsonToken &obj) {
      _id = obj._id;
      _value = obj._value;
      return *this;
    }

    /// @brief Move constructor for JsonToken.
    /// @param obj The JsonToken object to be moved.
    inline JsonToken (JsonToken &&obj) noexcept {
      _id = obj._id;
      _value = std::move(obj._value);
    }

    /// @brief Move assignment operator for JsonToken.
    /// @param obj The JsonToken object to be moved.
    /// @return A reference to the current JsonToken instance after the move assignment.
    inline JsonToken & operator= (JsonToken &&obj) noexcept {
      _id = obj._id;
      _value = std::move (obj._value);
      return *this;
    }

    /// Returns the JsonTokenId of the token.
    /// @return The JsonTokenId of the token.
    inline JsonTokenId id() const { return _id; }

    /// Returns a const reference to the value of the token.
    /// @tparam T The type of the value to retrieve.
    /// @return The value of the token as the specified type.
    /// @note This function performs a static assertion to ensure the requested type is valid.
    template<typename T>
    inline const T & value() const {
      static_assert(
        std::is_same_v<T, std::variant_alternative_t<0, decltype(_value)>> ||
        std::is_same_v<T, std::variant_alternative_t<1, decltype(_value)>> ||
        std::is_same_v<T, std::variant_alternative_t<2, decltype(_value)>> ||
        std::is_same_v<T, std::variant_alternative_t<3, decltype(_value)>> ||
        std::is_same_v<T, std::variant_alternative_t<4, decltype(_value)>>
      );

      return std::get<T> (_value);
    }

    /// Returns a reference to the value of the token.
    /// @tparam T The type of the value to retrieve.
    /// @return The value of the token as the specified type.
    /// @note This function performs a static assertion to ensure the requested type is valid.
    template<typename T>
    inline T & value() {
      static_assert(
        std::is_same_v<T, std::variant_alternative_t<0, decltype(_value)>> ||
        std::is_same_v<T, std::variant_alternative_t<1, decltype(_value)>> ||
        std::is_same_v<T, std::variant_alternative_t<2, decltype(_value)>> ||
        std::is_same_v<T, std::variant_alternative_t<3, decltype(_value)>> ||
        std::is_same_v<T, std::variant_alternative_t<4, decltype(_value)>>
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
    std::variant<std::string, bool, int64_t, double, std::nullptr_t> _value; ///< The value of the token, stored as a variant.
};

}

#endif
