// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <cstring>

#include <gtest/gtest.h>

#include <cppconfig/json_tokenizer.h>

constexpr const char * kJsonStr01 { R"({
  "key0": null,
  "key1": "string1",
  "key2": true,
  "key3": false,
  "float1": -0.12,
  "int1": -123,
  "array": [ 1, 2 ],
  "dict": { "sub1": 0456 }
})"
};

constexpr const std::array kJsonId01 {
  cppconfig::json::JsonTokenId::kObjectBegin,
  cppconfig::json::JsonTokenId::kValueString,
  cppconfig::json::JsonTokenId::kColon,
  cppconfig::json::JsonTokenId::kValueNull,
  cppconfig::json::JsonTokenId::kComma,
  cppconfig::json::JsonTokenId::kValueString,
  cppconfig::json::JsonTokenId::kColon,
  cppconfig::json::JsonTokenId::kValueString,
  cppconfig::json::JsonTokenId::kComma,
  cppconfig::json::JsonTokenId::kValueString,
  cppconfig::json::JsonTokenId::kColon,
  cppconfig::json::JsonTokenId::kValueBoolean,
  cppconfig::json::JsonTokenId::kComma,
  cppconfig::json::JsonTokenId::kValueString,
  cppconfig::json::JsonTokenId::kColon,
  cppconfig::json::JsonTokenId::kValueBoolean,
  cppconfig::json::JsonTokenId::kComma,
  cppconfig::json::JsonTokenId::kValueString,
  cppconfig::json::JsonTokenId::kColon,
  cppconfig::json::JsonTokenId::kValueFloatPoint,
  cppconfig::json::JsonTokenId::kComma,
  cppconfig::json::JsonTokenId::kValueString,
  cppconfig::json::JsonTokenId::kColon,
  cppconfig::json::JsonTokenId::kValueInteger,
  cppconfig::json::JsonTokenId::kComma,
  cppconfig::json::JsonTokenId::kValueString,
  cppconfig::json::JsonTokenId::kColon,
  cppconfig::json::JsonTokenId::kArrayBegin,
  cppconfig::json::JsonTokenId::kValueInteger,
  cppconfig::json::JsonTokenId::kComma,
  cppconfig::json::JsonTokenId::kValueInteger,
  cppconfig::json::JsonTokenId::kArrayEnd,
  cppconfig::json::JsonTokenId::kComma,
  cppconfig::json::JsonTokenId::kValueString,
  cppconfig::json::JsonTokenId::kColon,
  cppconfig::json::JsonTokenId::kObjectBegin,
  cppconfig::json::JsonTokenId::kValueString,
  cppconfig::json::JsonTokenId::kColon,
  cppconfig::json::JsonTokenId::kValueInteger,
  cppconfig::json::JsonTokenId::kObjectEnd,
  cppconfig::json::JsonTokenId::kObjectEnd
};

const std::array kJsonValue01 {
  std::any { },
  std::any { "key0" },
  std::any { },
  std::any { },
  std::any { },
  std::any { "key1" },
  std::any { },
  std::any { "string1" },
  std::any { },
  std::any { "key2" },
  std::any { },
  std::any { true },
  std::any { },
  std::any { "key3" },
  std::any { },
  std::any { false },
  std::any { },
  std::any { "float1" },
  std::any { },
  std::any { -0.12 },
  std::any { },
  std::any { "int1" },
  std::any { },
  std::any { -123LL },
  std::any { },
  std::any { "array" },
  std::any { },
  std::any { },
  std::any { 1LL },
  std::any { },
  std::any { 2LL },
  std::any { },
  std::any { },
  std::any { "dict" },
  std::any { },
  std::any { },
  std::any { "sub1" },
  std::any { },
  std::any { 456LL },
  std::any { },
  std::any { }
};

// ----------------------------------------------------------------------------
// test_emtpy
// ----------------------------------------------------------------------------
TEST (JsonTokenizer, test_emtpy) {
  cppconfig::json::JsonTokenizer tokenizer { cppconfig::json::Buffer { std::unique_ptr<char []> {}, 0 } };
  ASSERT_FALSE (tokenizer.next().has_value());
}

// ----------------------------------------------------------------------------
// test_next
// ----------------------------------------------------------------------------
TEST (JsonTokenizer, test_next) {
  const auto len { std::strlen (kJsonStr01) + 1 };
  auto data = std::make_unique<char []> (len);

  std::copy_n (kJsonStr01, len, data.get());
  cppconfig::json::Buffer buffer { std::move (data), len };
  cppconfig::json::JsonTokenizer tokenizer { std::move (buffer) };

  size_t idx { 0 };
  for (auto token = tokenizer.next(); token.has_value(); token = tokenizer.next(), ++idx) {
    EXPECT_EQ (token->id(), kJsonId01[idx]);

    if (kJsonValue01[idx].has_value()) {
      if (typeid(const char *) == kJsonValue01[idx].type())
        EXPECT_EQ (token->value<std::string>(), std::any_cast<const char *> (kJsonValue01[idx]));
      else if (typeid(int64_t) == kJsonValue01[idx].type())
        EXPECT_EQ (token->value<int64_t>(), std::any_cast<int64_t> (kJsonValue01[idx]));
      else if (typeid(double) == kJsonValue01[idx].type())
        EXPECT_EQ (token->value<double>(), std::any_cast<double> (kJsonValue01[idx]));
      else if (typeid(bool) == kJsonValue01[idx].type())
        EXPECT_EQ (token->value<bool>(), std::any_cast<bool> (kJsonValue01[idx]));
      else
        ASSERT_TRUE (false);
    }
  }
  ASSERT_EQ (kJsonId01.size(), idx);
}