// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <cstring>

#include <gtest/gtest.h>

#include <cppconfig/json_parser.h>
#include <cppconfig/mm_file.h>


// ----------------------------------------------------------------------------
// test_parser
// ----------------------------------------------------------------------------
TEST (JsonParser, test_well_formed) {
  constexpr const char *json { R"(
    {
      "int": 123,
      "float": 1.5,
      "bool": true,
      "none": null,
      "str": "hello",
      "obj": {
        "s0": -10,
        "s1": -2.5,
        "s2": false,
        "s3": null,
        "s4": "test",
        "s5": {
          "k0": -20,
          "k1": [ 1, "one", [ -1.0 ] ]
        }
      },
      "arr": [ 10, "two", -2.2, null, false, [ -1, 2.2, "three" ], { "first": -99 }, true ]
    }
  )"};

  cppconfig::json::JsonParser parser;

  const auto root { parser.parse (json, std::strlen (json) + 1) };
  ASSERT_TRUE (root.has_value());
  const auto &doc { root.value() };

  ASSERT_TRUE (doc.exists ("int"));
  ASSERT_TRUE (doc["int"].isInt());
  ASSERT_EQ (doc["int"].asInt(), 123);
  ASSERT_TRUE (doc.exists ("float"));
  ASSERT_TRUE (doc["float"].isFloat());
  ASSERT_EQ (doc["float"].asFloat(), 1.5);
  ASSERT_TRUE (doc.exists ("bool"));
  ASSERT_TRUE (doc["bool"].isBool());
  ASSERT_TRUE (doc["bool"].asBool());
  ASSERT_TRUE (doc.exists ("none"));
  ASSERT_TRUE (doc["none"].isNull());
  ASSERT_TRUE (doc.exists ("str"));
  ASSERT_TRUE (doc["str"].isString());
  ASSERT_EQ (doc["str"].asString(), "hello");

  ASSERT_TRUE (doc.exists ("obj"));
  const auto &obj { doc["obj"] };
  ASSERT_TRUE (obj.isObject());
  ASSERT_TRUE (obj.exists ("s0"));
  ASSERT_EQ (obj["s0"].asInt(), -10);
  ASSERT_TRUE (obj.exists ("s1"));
  ASSERT_EQ (obj["s1"].asFloat(), -2.5);
  ASSERT_TRUE (obj.exists ("s2"));
  ASSERT_FALSE (obj["s2"].asBool());
  ASSERT_TRUE (obj.exists ("s3"));
  ASSERT_TRUE (obj["s3"].isNull());
  ASSERT_TRUE (obj.exists ("s4"));
  ASSERT_EQ (obj["s4"].asString(), "test");

  ASSERT_TRUE (obj.exists ("s5"));
  const auto &s5 { obj["s5"] };
  ASSERT_TRUE (s5.isObject());
  ASSERT_TRUE (s5["k0"].isInt());
  ASSERT_TRUE (s5.exists ("k0"));
  ASSERT_EQ (s5["k0"].asInt(), -20);

  ASSERT_TRUE (s5.exists ("k1"));
  ASSERT_TRUE (s5["k1"].isArray());
  const auto &k1 { s5["k1"].asArray() };
  ASSERT_EQ (k1.size(), 3);
  ASSERT_TRUE (k1[0].isInt());
  ASSERT_EQ (k1[0].asInt(), 1);
  ASSERT_TRUE (k1[1].isString());
  ASSERT_EQ (k1[1].asString(), "one");
  ASSERT_TRUE (k1[2].isArray());
  ASSERT_EQ (k1[2].asArray().size(), 1);
  ASSERT_TRUE (k1[2].asArray()[0].isFloat());
  ASSERT_EQ (k1[2].asArray()[0].asFloat(), -1.0);

  ASSERT_TRUE (doc.exists ("arr"));
  ASSERT_TRUE ( doc["arr"].isArray());
  const auto &arr { doc["arr"].asArray() };
  ASSERT_EQ (arr.size(), 8);
  ASSERT_EQ (arr[0].asInt(), 10);
  ASSERT_EQ (arr[1].asString(), "two");
  ASSERT_EQ (arr[2].asFloat(), -2.2);
  ASSERT_TRUE (arr[3].isNull());
  ASSERT_FALSE (arr[4].asBool());
  ASSERT_TRUE (arr[5].isArray());
  ASSERT_EQ (arr[5].asArray().size(), 3);
  ASSERT_EQ (arr[5].asArray()[0].asInt(), -1);
  ASSERT_EQ (arr[5].asArray()[1].asFloat(), 2.2);
  ASSERT_EQ (arr[5].asArray()[2].asString(), "three");
  ASSERT_TRUE (arr[6].isObject());
  ASSERT_TRUE (arr[6].exists ("first"));
  ASSERT_TRUE (arr[6]["first"].isInt());
  ASSERT_EQ (arr[6]["first"].asInt(), -99);
  ASSERT_TRUE (arr[7].asBool());
}