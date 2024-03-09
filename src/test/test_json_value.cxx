// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <gtest/gtest.h>

#include <cppconfig/json_value.h>



// ----------------------------------------------------------------------------
// test_constructor
// ----------------------------------------------------------------------------
TEST (JsonValue, test_constructor) {
  cppconfig::json::JsonValue v0 { cppconfig::json::JsonToken {} };
  cppconfig::json::JsonValue v1 { cppconfig::json::JsonToken { true } };
  cppconfig::json::JsonValue v2 { cppconfig::json::JsonToken { int64_t(123) } };
  cppconfig::json::JsonValue v3 { cppconfig::json::JsonToken { double(1.2) } };
  cppconfig::json::JsonValue v4 { cppconfig::json::JsonToken { std::string_view { "str" } } };
  cppconfig::json::JsonValue v5 { cppconfig::json::JsonToken { cppconfig::json::JsonTokenId::kValueNull } };
  cppconfig::json::JsonValue v6 { std::unordered_map<std::string, cppconfig::json::JsonValue> { { "t1", v1 } } };
  cppconfig::json::JsonValue v7 { std::vector<cppconfig::json::JsonValue> { {} } };

  ASSERT_TRUE (v0.empty());
  ASSERT_TRUE (v1.isBool());
  ASSERT_TRUE (v2.isInt());
  ASSERT_TRUE (v3.isFloat());
  ASSERT_TRUE (v4.isString());
  ASSERT_TRUE (v5.isNull());
  ASSERT_TRUE (v6.isObject());
  ASSERT_TRUE (v7.isArray());
}

// ----------------------------------------------------------------------------
// test_exists
// ----------------------------------------------------------------------------
TEST (JsonValue, test_exists) {
  cppconfig::json::JsonValue v {
    std::unordered_map<std::string, cppconfig::json::JsonValue> {
      {
        std::string { "t1" },
        cppconfig::json::JsonValue { cppconfig::json::JsonToken {} }
      }
    }
  };

  ASSERT_TRUE (v.exists ("t1"));
  ASSERT_FALSE (v.exists ("t2"));
}

// ----------------------------------------------------------------------------
// test_operator
// ----------------------------------------------------------------------------
TEST (JsonValue, test_operator) {
  cppconfig::json::JsonValue v0 {
    std::unordered_map<std::string, cppconfig::json::JsonValue> {
      {
        std::string { "t1" },
        cppconfig::json::JsonValue { cppconfig::json::JsonToken { true } }
      }
    }
  };
  cppconfig::json::JsonValue v1 { std::vector<cppconfig::json::JsonValue> { { true } } };

  ASSERT_TRUE (v0["t1"].asBool());
  ASSERT_TRUE (v1[0].asBool());
}

// ----------------------------------------------------------------------------
// test_as
// ----------------------------------------------------------------------------
TEST (JsonValue, test_as) {
  cppconfig::json::JsonValue v1 { cppconfig::json::JsonToken { true } };
  cppconfig::json::JsonValue v2 { cppconfig::json::JsonToken { int64_t(123) } };
  cppconfig::json::JsonValue v3 { cppconfig::json::JsonToken { double(1.0) } };
  cppconfig::json::JsonValue v4 { cppconfig::json::JsonToken { std::string_view { "str" } } };
  cppconfig::json::JsonValue v5 { std::unordered_map<std::string, cppconfig::json::JsonValue> { { "t1", v1 } } };
  cppconfig::json::JsonValue v6 { std::vector<cppconfig::json::JsonValue> { v2 } };

  ASSERT_TRUE (v1.asBool());
  ASSERT_EQ (v2.asInt(), 123);
  ASSERT_EQ (v3.asFloat(), 1.0);
  ASSERT_EQ (v4.asString(), "str");
  ASSERT_EQ (v5.asObject().size(), 1);
  ASSERT_EQ (v6.asArray().size(), 1);
}

// ----------------------------------------------------------------------------
// test_type
// ----------------------------------------------------------------------------
TEST (JsonValue, test_type) {
  cppconfig::json::JsonValue v0 { cppconfig::json::JsonToken {} };
  cppconfig::json::JsonValue v1 { cppconfig::json::JsonToken { true } };
  cppconfig::json::JsonValue v2 { cppconfig::json::JsonToken { int64_t(123) } };
  cppconfig::json::JsonValue v3 { cppconfig::json::JsonToken { double(1.2) } };
  cppconfig::json::JsonValue v4 { cppconfig::json::JsonToken { std::string_view { "str" } } };
  cppconfig::json::JsonValue v5 { cppconfig::json::JsonToken { cppconfig::json::JsonTokenId::kValueNull } };
  cppconfig::json::JsonValue v6 { std::unordered_map<std::string, cppconfig::json::JsonValue> { { "t1", v0 } } };
  cppconfig::json::JsonValue v7 { std::vector<cppconfig::json::JsonValue> { {} } };

  ASSERT_EQ (v0.type(), typeid(void));
  ASSERT_EQ (v1.type(), typeid(bool));
  ASSERT_EQ (v2.type(), typeid(int64_t));
  ASSERT_EQ (v3.type(), typeid(double));
  ASSERT_EQ (v4.type(), typeid(std::string));
  ASSERT_EQ (v5.type(), typeid(std::nullptr_t));
  ASSERT_EQ (v6.type(), typeid(std::unordered_map<std::string, cppconfig::json::JsonValue>));
  ASSERT_EQ (v7.type(), typeid(std::vector<cppconfig::json::JsonValue>));
}
