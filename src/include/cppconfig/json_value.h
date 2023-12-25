// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023 Carlos Carrasco
// ----------------------------------------------------------------------------
#ifndef __CPP_CONFIG_JSON_VALUE_H__
#define __CPP_CONFIG_JSON_VALUE_H__
#include <cassert>
#include <map>
#include <string>
#include <vector>

#include <cppconfig/json_tokenizer.h>


namespace cppconfig::json {

class JsonValue {
  public:
    JsonValue (JsonToken &&token): _token { std::move (token) } {
      // empty
    }

    JsonValue (std::map<std::string, JsonValue> &&map): _token { JsonTokenId::kObjectBegin }, _map { std::move (map) } {
      // empty
    }

    JsonValue (std::vector<JsonValue> &&array): _token { JsonTokenId::kArrayBegin }, _array { std::move (array) } {
      // empty
    }

    inline bool isBool() const { return _token.id() == JsonTokenId::kValueBoolean; }
    inline bool isInt() const { return _token.id() == JsonTokenId::kValueInteger; }
    inline bool isFloat() const { return _token.id() == JsonTokenId::kValueFloatPoint; }
    inline bool isString() const { return _token.id() == JsonTokenId::kValueString; }
    inline bool isNull() const { return _token.id() == JsonTokenId::kValueNull; }
    inline bool isObject() const { return _token.id() == JsonTokenId::kObjectBegin; }
    inline bool isArray() const { return _token.id() == JsonTokenId::kArrayBegin; }
    inline bool empty() const { return _token.id() == JsonTokenId::kEmpty; }

    inline bool exists (const std::string &k) const {
      return _map.find (k) != _map.end();
    }

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

    JsonValue & operator[] (const std::string &k) {
      assert (_token.id() == JsonTokenId::kObjectBegin);
      return _map.find (k)->second;
    }

    const JsonValue & operator[] (const std::string &k) const {
      assert (_token.id() == JsonTokenId::kObjectBegin);
      return _map.find (k)->second;
    }

    JsonValue & operator[] (size_t i) {
      assert (_token.id() == JsonTokenId::kArrayBegin);
      return _array[i];
    }

    const JsonValue & operator[] (size_t i) const {
      assert (_token.id() == JsonTokenId::kArrayBegin);
      return _array[i];
    }

    inline bool asBool() const { return get<bool> (); }
    inline int64_t asInt() const { return get<int64_t> (); }
    inline double asFloat() const { return get<double> (); }
    inline const std::string & asString() const { return get<std::string> (); }
    inline const std::map<std::string, JsonValue> & asObject() const { return get<std::map<std::string, JsonValue>> (); }
    inline const std::vector<JsonValue> & asArray() const { return get<std::vector<JsonValue>> (); }

  private:
    JsonToken _token;
    std::map<std::string, JsonValue> _map;
    std::vector<JsonValue> _array;

    // JsonValue(): _token { JsonTokenId::kEmpty } {
    //   // empty
    // }
};

}

#endif
