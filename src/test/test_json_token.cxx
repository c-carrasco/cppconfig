// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023-2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <gtest/gtest.h>

#include <cppconfig/json_tokenizer.h>


// ----------------------------------------------------------------------------
// test_constructor
// ----------------------------------------------------------------------------
TEST (JsonToken, test_constructor) {
  cppconfig::json::JsonToken jt01 { cppconfig::json::JsonTokenId::kObjectBegin };
  ASSERT_EQ (jt01.id(), cppconfig::json::JsonTokenId::kObjectBegin);
  cppconfig::json::JsonToken jt02 { cppconfig::json::JsonTokenId::kObjectEnd };
  ASSERT_EQ (jt02.id(), cppconfig::json::JsonTokenId::kObjectEnd);
  cppconfig::json::JsonToken jt03 { cppconfig::json::JsonTokenId::kArrayBegin };
  ASSERT_EQ (jt03.id(), cppconfig::json::JsonTokenId::kArrayBegin);
  cppconfig::json::JsonToken jt04 { cppconfig::json::JsonTokenId::kArrayEnd };
  ASSERT_EQ (jt04.id(), cppconfig::json::JsonTokenId::kArrayEnd);
  cppconfig::json::JsonToken jt05 { cppconfig::json::JsonTokenId::kColon };
  ASSERT_EQ (jt05.id(), cppconfig::json::JsonTokenId::kColon);
  cppconfig::json::JsonToken jt06 { cppconfig::json::JsonTokenId::kComma };
  ASSERT_EQ (jt06.id(), cppconfig::json::JsonTokenId::kComma);
  cppconfig::json::JsonToken jt07 { cppconfig::json::JsonTokenId::kValueString };
  ASSERT_EQ (jt07.id(), cppconfig::json::JsonTokenId::kValueString);
  cppconfig::json::JsonToken jt08 { cppconfig::json::JsonTokenId::kValueFloatPoint };
  ASSERT_EQ (jt08.id(), cppconfig::json::JsonTokenId::kValueFloatPoint);
  cppconfig::json::JsonToken jt09 { cppconfig::json::JsonTokenId::kValueInteger };
  ASSERT_EQ (jt09.id(), cppconfig::json::JsonTokenId::kValueInteger);
  cppconfig::json::JsonToken jt10 { cppconfig::json::JsonTokenId::kValueBoolean };
  ASSERT_EQ (jt10.id(), cppconfig::json::JsonTokenId::kValueBoolean);
  cppconfig::json::JsonToken jt11 { cppconfig::json::JsonTokenId::kValueNull };
  ASSERT_EQ (jt11.id(), cppconfig::json::JsonTokenId::kValueNull);
  cppconfig::json::JsonToken jt12 { cppconfig::json::JsonTokenId::kError };
  ASSERT_EQ (jt12.id(), cppconfig::json::JsonTokenId::kError);

  cppconfig::json::JsonToken jt13 { std::string_view { "Hello World!" } };
  ASSERT_EQ (jt13.id(), cppconfig::json::JsonTokenId::kValueString);
  ASSERT_EQ (jt13.value<std::string>(), "Hello World!");
  cppconfig::json::JsonToken jt14 { 1.234f };
  ASSERT_EQ (jt14.id(), cppconfig::json::JsonTokenId::kValueFloatPoint);
  ASSERT_NEAR (jt14.value<double>(), 1.234, 0.001);
  cppconfig::json::JsonToken jt15 { int64_t(-123L) };
  ASSERT_EQ (jt15.id(), cppconfig::json::JsonTokenId::kValueInteger);
  ASSERT_EQ (jt15.value<int64_t>(), -123);
  cppconfig::json::JsonToken jt16 { true };
  ASSERT_EQ (jt16.id(), cppconfig::json::JsonTokenId::kValueBoolean);
  ASSERT_EQ (jt16.value<bool>(), true);
}
