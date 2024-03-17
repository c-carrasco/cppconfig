// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023-2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <cstring>

#include <gtest/gtest.h>

#include <cppconfig/json_parser.h>
#include <cppconfig/mm_file.h>


// ----------------------------------------------------------------------------
// test_float_point
// ----------------------------------------------------------------------------
TEST (JsonParser, test_float_point) {
  cppconfig::json::JsonParser parser;

  const auto root0 { parser.parse (R"({ "f1": 1.50, "f2": -1.5, "f3": 0.915, "f4": -0.0123, "f5": 0., "f6": -0. })") };
  ASSERT_TRUE (root0.has_value());
  ASSERT_NEAR (root0.value()["f1"].asFloat(), 1.5, 0.001);
  ASSERT_NEAR (root0.value()["f2"].asFloat(), -1.5, 0.001);
  ASSERT_NEAR (root0.value()["f3"].asFloat(), 0.915, 0.0001);
  ASSERT_NEAR (root0.value()["f4"].asFloat(), -0.0123, 0.0001);
  ASSERT_NEAR (root0.value()["f5"].asFloat(), 0.0, 0.001);
  ASSERT_NEAR (root0.value()["f6"].asFloat(), 0.0, 0.001);

  const auto root1 { parser.parse (R"({
    "f1": 2E1, "f2": -2E2, "f3": 2e-3, "f4": -2e-4, "f5": 0e0, "f6": -0.3e-12, "f7": -12.456E12
  })") };
  ASSERT_TRUE (root1.has_value());
  ASSERT_EQ (root1.value()["f1"].asFloat(), 2e1);
  ASSERT_EQ (root1.value()["f2"].asFloat(), -2e2);
  ASSERT_EQ (root1.value()["f3"].asFloat(), 2e-3);
  ASSERT_EQ (root1.value()["f4"].asFloat(), -2e-4);
  ASSERT_EQ (root1.value()["f5"].asFloat(), 0e0);
  ASSERT_EQ (root1.value()["f6"].asFloat(), -0.3e-12);
  ASSERT_EQ (root1.value()["f7"].asFloat(), -12.456E12);
}

// ----------------------------------------------------------------------------
// test_integer
// ----------------------------------------------------------------------------
TEST (JsonParser, test_integer) {
  cppconfig::json::JsonParser parser;

  const auto root { parser.parse (R"({ "i1": 0, "i2": -1, "i3": -0, "i4": 1234567890, "i5": -1234556780 })") };
  ASSERT_TRUE (root.has_value());
  ASSERT_EQ (root.value()["i1"].asInt(), 0);
  ASSERT_EQ (root.value()["i2"].asInt(), -1);
  ASSERT_EQ (root.value()["i3"].asInt(), 0);
  ASSERT_EQ (root.value()["i4"].asInt(), 1234567890);
  ASSERT_EQ (root.value()["i5"].asInt(), -1234556780);
}

// ----------------------------------------------------------------------------
// test_bool
// ----------------------------------------------------------------------------
TEST (JsonParser, test_bool) {
  cppconfig::json::JsonParser parser;

  const auto root { parser.parse (R"({ "b1": true, "b2": false })") };
  ASSERT_TRUE (root.has_value());
  ASSERT_EQ (root.value()["b1"].asBool(), true);
  ASSERT_EQ (root.value()["b2"].asBool(), false);
}

// ----------------------------------------------------------------------------
// test_string
// ----------------------------------------------------------------------------
TEST (JsonParser, test_string) {
  cppconfig::json::JsonParser parser;
  const auto root { parser.parse (R"({ "s1": "", "s2": "a", "s3": "abc", "s4": "temp: 2\u00b0C" })") };
  ASSERT_TRUE (root.has_value());
  ASSERT_EQ (root.value()["s1"].asString(), "");
  ASSERT_EQ (root.value()["s2"].asString(), "a");
  ASSERT_EQ (root.value()["s3"].asString(), "abc");
  ASSERT_EQ (root.value()["s4"].asString(), "temp: 2°C");
}

// ----------------------------------------------------------------------------
// test_null
// ----------------------------------------------------------------------------
TEST (JsonParser, test_null) {
  cppconfig::json::JsonParser parser;
  const auto root { parser.parse (R"({ "n1": null, "n2": "" })") };
  ASSERT_TRUE (root.has_value());
  ASSERT_TRUE (root.value()["n1"].isNull());
  ASSERT_FALSE (root.value()["n2"].isNull());
}

// ----------------------------------------------------------------------------
// test_array
// ----------------------------------------------------------------------------
TEST (JsonParser, test_array) {
  cppconfig::json::JsonParser parser;

  const auto root0 { parser.parse (R"({ "a1": [1], "a2": [1,2], "a3": [1,2,3] })") };
  ASSERT_TRUE (root0.has_value());
  ASSERT_EQ (root0.value()["a1"].asArray().size(), 1);
  ASSERT_EQ (root0.value()["a1"].asArray()[0].asInt(), 1);
  ASSERT_EQ (root0.value()["a2"].asArray().size(), 2);
  ASSERT_EQ (root0.value()["a2"].asArray()[0].asInt(), 1);
  ASSERT_EQ (root0.value()["a2"].asArray()[1].asInt(), 2);
  ASSERT_EQ (root0.value()["a3"].asArray().size(), 3);
  ASSERT_EQ (root0.value()["a3"].asArray()[0].asInt(), 1);
  ASSERT_EQ (root0.value()["a3"].asArray()[1].asInt(), 2);
  ASSERT_EQ (root0.value()["a3"].asArray()[2].asInt(), 3);

  const auto root1 { parser.parse (R"({ "a1": [ "s1" ], "a2": [ "s1", "s2" ], "a3": [ "s1", "s2", "s3" ] })") };
  ASSERT_TRUE (root1.has_value());
  ASSERT_EQ (root1.value()["a1"].asArray().size(), 1);
  ASSERT_EQ (root1.value()["a1"].asArray()[0].asString(), "s1");
  ASSERT_EQ (root1.value()["a2"].asArray().size(), 2);
  ASSERT_EQ (root1.value()["a2"].asArray()[0].asString(), "s1");
  ASSERT_EQ (root1.value()["a2"].asArray()[1].asString(), "s2");
  ASSERT_EQ (root1.value()["a3"].asArray().size(), 3);
  ASSERT_EQ (root1.value()["a3"].asArray()[0].asString(), "s1");
  ASSERT_EQ (root1.value()["a3"].asArray()[1].asString(), "s2");
  ASSERT_EQ (root1.value()["a3"].asArray()[2].asString(), "s3");

  const auto root2 { parser.parse (R"({ "a1": [ 2, "s2", -2.0, true, null, [1], { "a22": 22 } ] })") };
  ASSERT_TRUE (root2.has_value());
  ASSERT_EQ (root2.value()["a1"].asArray().size(), 7);
  ASSERT_EQ (root2.value()["a1"].asArray()[0].asInt(), 2);
  ASSERT_EQ (root2.value()["a1"].asArray()[1].asString(), "s2");
  ASSERT_EQ (root2.value()["a1"].asArray()[2].asFloat(), -2.0);
  ASSERT_EQ (root2.value()["a1"].asArray()[3].asBool(), true);
  ASSERT_TRUE (root2.value()["a1"].asArray()[4].isNull());
  ASSERT_EQ (root2.value()["a1"].asArray()[5].asArray().size(), 1);
  ASSERT_EQ (root2.value()["a1"].asArray()[5].asArray()[0].asInt(), 1);
  ASSERT_EQ (root2.value()["a1"].asArray()[6]["a22"].asInt(), 22);

  const auto root3 { parser.parse (R"([ 1, { "i": -1 }, [ "s1" ] ])") };
  ASSERT_TRUE (root3.has_value());
  ASSERT_EQ (root3.value().asArray().size(), 3);
  ASSERT_EQ (root3.value().asArray()[0].asInt(), 1);
  ASSERT_EQ (root3.value().asArray()[1]["i"].asInt(), -1);
  ASSERT_EQ (root3.value().asArray()[2][0].asString(), "s1");

  // empty array
  ASSERT_TRUE (cppconfig::json::JsonParser().parse (R"([])"));
  ASSERT_TRUE (cppconfig::json::JsonParser().parse (R"([ [ [ ] ] ])"));
  ASSERT_TRUE (cppconfig::json::JsonParser().parse (R"({ "o": [] })"));
}

// ----------------------------------------------------------------------------
// test_object
// ----------------------------------------------------------------------------
TEST (JsonParser, test_object) {
  const auto root0 { cppconfig::json::JsonParser().parse (R"({ "o1": 1, "o2": { "o22": { "o222": "str" } } })") };
  ASSERT_TRUE (root0.has_value());
  ASSERT_EQ (root0.value()["o1"].asInt(), 1);
  ASSERT_EQ (root0.value()["o2"]["o22"]["o222"].asString(), "str");

  // empty object
  const auto root { cppconfig::json::JsonParser().parse (R"({ "obj1": {} })") };
  ASSERT_TRUE (root.has_value());
  ASSERT_EQ (root.value()["obj1"].asObject().size(), 0);

  ASSERT_TRUE (cppconfig::json::JsonParser().parse (R"({})"));
}

// ----------------------------------------------------------------------------
// test_error
// ----------------------------------------------------------------------------
TEST (JsonParser, test_error) {
  constexpr const char *json0 { R"(
    {
      "v1": 2
      "v2": "hello"
    }
  )"};

  cppconfig::json::JsonParser parser;

  const auto root0 { parser.parse (json0, std::strlen (json0)) };
  ASSERT_FALSE (root0.has_value());
  ASSERT_EQ (parser.error().code, cppconfig::json::JsonParser::ErrorCode::kExpectCommaOrEndObj);
  ASSERT_EQ (parser.error().line, 3);
  ASSERT_EQ (parser.error().column, 10);
}

// ----------------------------------------------------------------------------
// test_error1
// ----------------------------------------------------------------------------
TEST (JsonParser, test_error1) {
  constexpr const char *json { R"(
    {
      "v1": "hello
    }
  )"};

  cppconfig::json::JsonParser parser;

  const auto root { parser.parse (json) };
  ASSERT_FALSE (root.has_value());
  ASSERT_EQ (parser.error().code, cppconfig::json::JsonParser::ErrorCode::kExpectAny);
  ASSERT_EQ (parser.error().line, 4);
  ASSERT_EQ (parser.error().column, 2);
}

// ----------------------------------------------------------------------------
// test_error2
// ----------------------------------------------------------------------------
TEST (JsonParser, test_error2) {
  constexpr const char *json { R"(
  )"};

  cppconfig::json::JsonParser parser;

  const auto root { parser.parse (json) };
  ASSERT_FALSE (root.has_value());
  ASSERT_EQ (parser.error().code, cppconfig::json::JsonParser::ErrorCode::kExpectAny);
  ASSERT_EQ (parser.error().line, 1);
  ASSERT_EQ (parser.error().column, 2);
}


// ----------------------------------------------------------------------------
// test_error3
// ----------------------------------------------------------------------------
TEST (JsonParser, test_error3) {
  constexpr const char *json { R"({
    "a": [
  })"};

  cppconfig::json::JsonParser parser;

  const auto root { parser.parse (json) };
  ASSERT_FALSE (root.has_value());
  ASSERT_EQ (parser.error().code, cppconfig::json::JsonParser::ErrorCode::kExpectAny);
  ASSERT_EQ (parser.error().line, 2);
  ASSERT_EQ (parser.error().column, 3);
}

// ----------------------------------------------------------------------------
// test_error4
// ----------------------------------------------------------------------------
TEST (JsonParser, test_error4) {
  constexpr const char *json { R"({
    "a": [ [], "v2": 3
  })"};

  cppconfig::json::JsonParser parser;

  const auto root { parser.parse (json) };
  ASSERT_FALSE (root.has_value());
  ASSERT_EQ (parser.error().code, cppconfig::json::JsonParser::ErrorCode::kExpectCommaOrEndArray);
  ASSERT_EQ (parser.error().line, 1);
  ASSERT_EQ (parser.error().column, 20);
}

// ----------------------------------------------------------------------------
// test_error5
// ----------------------------------------------------------------------------
TEST (JsonParser, test_error5) {
  constexpr const char *json { R"({
    "a": 1ab
  })"};

  cppconfig::json::JsonParser parser;

  const auto root { parser.parse (json) };
  ASSERT_FALSE (root.has_value());
  ASSERT_EQ (parser.error().code, cppconfig::json::JsonParser::ErrorCode::kExpectAny);
  ASSERT_EQ (parser.error().line, 1);
  ASSERT_EQ (parser.error().column, 10);
}
