// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023-2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <cstring>

#include <gtest/gtest.h>

#include <cppconfig/config.h>


// ----------------------------------------------------------------------------
// test_config
// ----------------------------------------------------------------------------
TEST (Config, test_config) {
  constexpr const char *str { R"({
    "int": 123,
    "float": 1.5,
    "bool": true,
    "str.1": "hello",
    "obj": {
      "s0": -10,
      "s1": -2.5,
      "s2": false,
      "s4": "test",
      "s5": {
        "k0": -20,
        "k1": [ "zero", "one" ]
      },
      "s6": null
    },
    "a1": [ 0, 1 ],
    "a2": [ 0.0, 1.0 ],
    "a3": [ true, false ]
  })" };

  cppconfig::Config config { str };

  ASSERT_EQ (config.get<uint8_t> ("int").value(), 123);
  ASSERT_EQ (config.get<int32_t> ("int").value(), 123);
  ASSERT_EQ (config.get<uint64_t> ("int").value(), 123);
  ASSERT_EQ (config.get<float> ("float").value(), 1.5);
  ASSERT_EQ (config.get<double> ("float").value(), 1.5);
  ASSERT_TRUE (config.get<bool> ("bool").value());
  ASSERT_EQ (config.get<std::string> ("str\\.1").value(), "hello");
  ASSERT_EQ (config.get<int64_t> ("obj.s0").value(), -10);
  ASSERT_EQ (config.get<double> ("obj.s1").value(), -2.5);
  ASSERT_FALSE (config.get<bool> ("obj.s2").value());
  ASSERT_EQ (config.get<std::string> ("obj.s4").value(), "test");
  ASSERT_EQ (config.get<int32_t> ("obj.s5.k0").value(), -20);
  ASSERT_EQ (config.get<std::nullptr_t> ("obj.s6").value(), nullptr);
  const auto array1 { config.get<std::vector<std::string>> ("obj.s5.k1").value() };
  ASSERT_EQ (array1.size(), 2);
  ASSERT_EQ (array1[0], "zero");
  ASSERT_EQ (array1[1], "one");
  const auto array2 { config.get<std::vector<int64_t>> ("a1").value() };
  ASSERT_EQ (array2.size(), 2);
  ASSERT_EQ (array2[0], 0);
  ASSERT_EQ (array2[1], 1);
  const auto array3 { config.get<std::vector<double>> ("a2").value() };
  ASSERT_EQ (array3.size(), 2);
  ASSERT_EQ (array3[0], 0.0);
  ASSERT_EQ (array3[1], 1.0);
  ASSERT_FALSE (config.get ("do-not-exists").has_value());
}
