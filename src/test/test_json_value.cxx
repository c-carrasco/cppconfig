// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <gtest/gtest.h>

#include <cppconfig/json_parser.h>
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

// ----------------------------------------------------------------------------
// test_merge_invalid_types
// ----------------------------------------------------------------------------
TEST (JsonValue, test_merge_invalid_types) {
  cppconfig::json::JsonValue v1 { cppconfig::json::JsonToken { true } };
  cppconfig::json::JsonValue v2 { cppconfig::json::JsonToken { int64_t(123) } };

  ASSERT_FALSE (cppconfig::json::JsonValue::merge (v1, v2));
}

// ----------------------------------------------------------------------------
// test_merge_basic_types
// ----------------------------------------------------------------------------
TEST (JsonValue, test_merge_basic_types) {
  cppconfig::json::JsonValue v1 { cppconfig::json::JsonToken { int64_t(123) } };
  cppconfig::json::JsonValue v2 { cppconfig::json::JsonToken { int64_t(456) } };
  ASSERT_TRUE (cppconfig::json::JsonValue::merge (v1, v2));
  ASSERT_EQ (v1.asInt(), v2.asInt());

  cppconfig::json::JsonValue v3 { cppconfig::json::JsonToken { true } };
  cppconfig::json::JsonValue v4 { cppconfig::json::JsonToken { false } };
  ASSERT_TRUE (cppconfig::json::JsonValue::merge (v3, v4));
  ASSERT_EQ (v3.asBool(), v4.asBool());

  cppconfig::json::JsonValue v5 { cppconfig::json::JsonToken { double(123.0) } };
  cppconfig::json::JsonValue v6 { cppconfig::json::JsonToken { double(456.0) } };
  ASSERT_TRUE (cppconfig::json::JsonValue::merge (v5, v6));
  ASSERT_EQ (v5.asFloat(), v6.asFloat());

  cppconfig::json::JsonValue v7 { cppconfig::json::JsonToken { std::string_view { "seven" } } };
  cppconfig::json::JsonValue v8 { cppconfig::json::JsonToken { std::string_view { "eight" } } };
  ASSERT_TRUE (cppconfig::json::JsonValue::merge (v7, v8));
  ASSERT_EQ (v7.asString(), v8.asString());

  cppconfig::json::JsonValue v9 { cppconfig::json::JsonToken { int64_t(1)} };
  cppconfig::json::JsonValue v10 { cppconfig::json::JsonToken { cppconfig::json::JsonTokenId::kValueNull } };
  ASSERT_TRUE (cppconfig::json::JsonValue::merge (v9, v10));
  ASSERT_EQ (v9.asInt(), v10.asInt());

  cppconfig::json::JsonValue v11 { cppconfig::json::JsonToken { cppconfig::json::JsonTokenId::kValueNull } };
  cppconfig::json::JsonValue v12 { cppconfig::json::JsonToken { int64_t(2) } };
  ASSERT_TRUE (cppconfig::json::JsonValue::merge (v11, v12));
  ASSERT_TRUE (v11.isNull());
  ASSERT_TRUE (v12.isNull());
}

// ----------------------------------------------------------------------------
// test_merge_object
// ----------------------------------------------------------------------------
TEST (JsonValue, test_merge_object) {
  cppconfig::json::JsonParser parser1;
  cppconfig::json::JsonParser parser2;

  auto root1 { parser1.parse (R"({ "obj1": { "i1": 11, "s1": "str11", "f1": 11.0, "b1": true, "n1": null } })") };
  auto root2 { parser2.parse (R"({ "obj1": { "i1": 22, "s1": "str22", "f1": 22.0, "b1": false, "n1": "not-null" } })") };

  ASSERT_TRUE (root1.has_value());
  ASSERT_TRUE (root2.has_value());
  ASSERT_TRUE (cppconfig::json::JsonValue::merge (root1.value(), root2.value()));
  ASSERT_EQ (root2.value()["obj1"]["i1"].asInt(), 11);
  ASSERT_EQ (root2.value()["obj1"]["s1"].asString(), "str11");
  ASSERT_EQ (root2.value()["obj1"]["f1"].asFloat(), 11.0);
  ASSERT_EQ (root2.value()["obj1"]["b1"].asBool(), true);
  ASSERT_TRUE (root2.value()["obj1"]["n1"].isNull());
}

// ----------------------------------------------------------------------------
// test_merge_array_basic_types
// ----------------------------------------------------------------------------
TEST (JsonValue, test_merge_array_basic_types) {
  cppconfig::json::JsonParser parser1;
  cppconfig::json::JsonParser parser2;

  auto root1 { parser1.parse (R"({ "obj1": { "a1": [1, 2] } })") };
  auto root2 { parser2.parse (R"({ "obj1": { "a1": [] } })") };

  ASSERT_TRUE (root1.has_value());
  ASSERT_TRUE (root2.has_value());
  ASSERT_TRUE (cppconfig::json::JsonValue::merge (root1.value(), root2.value()));
  ASSERT_EQ (root2.value()["obj1"]["a1"].asArray().size(), 2);
  ASSERT_EQ (root2.value()["obj1"]["a1"][0].asInt(), 1);
  ASSERT_EQ (root2.value()["obj1"]["a1"][1].asInt(), 2);
}

// ----------------------------------------------------------------------------
// test_merge_array_object
// ----------------------------------------------------------------------------
TEST (JsonValue, test_merge_array_object) {
  cppconfig::json::JsonParser parser1;
  cppconfig::json::JsonParser parser2;

  auto root1 { parser1.parse (R"({ "obj1": [ { "a": 1 } ] })") };
  auto root2 { parser2.parse (R"({ "obj1": [ { "b": 2 } ] })") };

  ASSERT_TRUE (root1.has_value());
  ASSERT_TRUE (root2.has_value());
  ASSERT_TRUE (cppconfig::json::JsonValue::merge (root1.value(), root2.value()));
  ASSERT_EQ (root2.value()["obj1"].asArray().size(), 2);
  ASSERT_TRUE (root2.value()["obj1"][0].isObject());
  ASSERT_TRUE (root2.value()["obj1"][1].isObject());
  ASSERT_EQ (root2.value()["obj1"][0]["b"].asInt(), 2);
  ASSERT_EQ (root2.value()["obj1"][1]["a"].asInt(), 1);
}
