// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023-2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <cstring>
#include <array>
#include <any>

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
  "dict": { "sub1": 0456 },
  "key4": "\"unicode\" \u00b0C\nhello\tworld"
})"
};

const std::array<cppconfig::json::JsonTokenId, 45> kJsonId01 {
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
  cppconfig::json::JsonTokenId::kComma,
  cppconfig::json::JsonTokenId::kValueString,
  cppconfig::json::JsonTokenId::kColon,
  cppconfig::json::JsonTokenId::kValueString,
  cppconfig::json::JsonTokenId::kObjectEnd
};

const std::array<std::any, 45> kJsonValue01 {
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
  std::any { int64_t(-123) },
  std::any { },
  std::any { "array" },
  std::any { },
  std::any { },
  std::any { int64_t(1) },
  std::any { },
  std::any { int64_t(2) },
  std::any { },
  std::any { },
  std::any { "dict" },
  std::any { },
  std::any { },
  std::any { "sub1" },
  std::any { },
  std::any { int64_t(456) },
  std::any { },
  std::any { },
  std::any { "key4" },
  std::any { },
  std::any { R"("unicode" °C
hello	world)" },
  std::any { }
};

// ----------------------------------------------------------------------------
// test_emtpy
// ----------------------------------------------------------------------------
TEST (JsonTokenizer, test_emtpy) {
  cppconfig::json::JsonTokenizer tokenizer { cppconfig::json::Buffer { nullptr, 0 } };
  ASSERT_FALSE (tokenizer.next().has_value());
}

// ----------------------------------------------------------------------------
// test_next
// ----------------------------------------------------------------------------
TEST (JsonTokenizer, test_next) {
  cppconfig::json::Buffer buffer { kJsonStr01, std::strlen (kJsonStr01) };
  cppconfig::json::JsonTokenizer tokenizer { std::move (buffer) };

  size_t idx { 0 };
  for (auto token = tokenizer.next(); token.has_value(); token = tokenizer.next(), ++idx) {
    ASSERT_EQ (token->id(), kJsonId01[idx]);

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

// ----------------------------------------------------------------------------
// test_premature_error
// ----------------------------------------------------------------------------
TEST (JsonTokenizer, test_premature_error) {
  constexpr const char *str0 { "{ \"test" };
  cppconfig::json::JsonTokenizer tokenizer0 { cppconfig::json::Buffer { str0, std::strlen (str0) } };
  const auto t0 { tokenizer0.next() };
  ASSERT_TRUE (t0.has_value());
  ASSERT_EQ (t0->id(), cppconfig::json::JsonTokenId::kObjectBegin);
  const auto t1 { tokenizer0.next() };
  ASSERT_TRUE (t1.has_value());
  ASSERT_EQ (t1->id(), cppconfig::json::JsonTokenId::kError);
  ASSERT_EQ (tokenizer0.error(), cppconfig::json::JsonTokenizer::Error::kPrematureEnd);
  ASSERT_EQ (tokenizer0.line(), 0);
  ASSERT_EQ (tokenizer0.column(), 7);

  constexpr const char *str1 { "{ \"test\":\n1a }" };
  cppconfig::json::JsonTokenizer tokenizer1 { cppconfig::json::Buffer { str1, std::strlen (str1) } };
  ASSERT_EQ (tokenizer1.next()->id(), cppconfig::json::JsonTokenId::kObjectBegin);
  ASSERT_EQ (tokenizer1.next()->id(), cppconfig::json::JsonTokenId::kValueString);
  ASSERT_EQ (tokenizer1.next()->id(), cppconfig::json::JsonTokenId::kColon);
  ASSERT_EQ (tokenizer1.next()->id(), cppconfig::json::JsonTokenId::kError);
  ASSERT_EQ (tokenizer1.error(), cppconfig::json::JsonTokenizer::Error::kPrematureEnd);
  ASSERT_EQ (tokenizer1.line(), 1);
  ASSERT_EQ (tokenizer1.column(), 1);
}

// ----------------------------------------------------------------------------
// test_escape_error
// ----------------------------------------------------------------------------
TEST (JsonTokenizer, test_escape_error) {
  constexpr const char *str1 { "{\n\"test\":\n \"aa\\kbbb\" }" };
  cppconfig::json::JsonTokenizer tokenizer0 { cppconfig::json::Buffer { str1, std::strlen (str1) } };
  ASSERT_EQ (tokenizer0.next()->id(), cppconfig::json::JsonTokenId::kObjectBegin);
  ASSERT_EQ (tokenizer0.next()->id(), cppconfig::json::JsonTokenId::kValueString);
  ASSERT_EQ (tokenizer0.next()->id(), cppconfig::json::JsonTokenId::kColon);
  ASSERT_EQ (tokenizer0.next()->id(), cppconfig::json::JsonTokenId::kError);
  ASSERT_EQ (tokenizer0.error(), cppconfig::json::JsonTokenizer::Error::kInvalidEscape);
  ASSERT_EQ (tokenizer0.line(), 2);
  ASSERT_EQ (tokenizer0.column(), 6);
}
