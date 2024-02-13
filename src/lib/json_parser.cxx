// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023-2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <cppconfig/json_parser.h>
#include <iostream>


namespace cppconfig::json {

static_assert (
  static_cast<int32_t> (JsonParser::ErrorCode::kPrematureEnd) ==
  static_cast<int32_t> (JsonTokenizer::Error::kPrematureEnd)
);
static_assert (
  static_cast<int32_t> (JsonParser::ErrorCode::kInvalidEscape) ==
  static_cast<int32_t> (JsonTokenizer::Error::kInvalidEscape)
);


// ----------------------------------------------------------------------------
// JsonParser::parse
// ----------------------------------------------------------------------------
std::optional<JsonValue> JsonParser::parse (const char *data, size_t size) {
  _tokenizer = std::make_unique<JsonTokenizer> (Buffer { data, size });

  auto token { _tokenizer->next() };
  if (!token.has_value())
    return _setError (ErrorCode::kExpectAny);

  if (token->id() == JsonTokenId::kError)
    return _setError (static_cast<ErrorCode> (_tokenizer->error()));
  if (token->id() == JsonTokenId::kObjectBegin)
    return _parseObject ();
  if (token->id() == JsonTokenId::kArrayBegin)
    return _parseArray ();

  return _setError (ErrorCode::kExpectObject);
}

// ----------------------------------------------------------------------------
// JsonParser::_parseObject
// ----------------------------------------------------------------------------
std::optional<JsonValue> JsonParser::_parseObject () {
  std::unordered_map<std::string, JsonValue> map;

  do {
    auto k { _tokenizer->next() };
    auto c { _tokenizer->next() };
    auto v { _tokenizer->next() };

    if (
      k.has_value() && c.has_value() && v.has_value() &&
      (k->id() == JsonTokenId::kValueString) &&
      (c->id() == JsonTokenId::kColon)
    ) {
      switch (v->id()) {
        case JsonTokenId::kValueInteger:
        case JsonTokenId::kValueFloatPoint:
        case JsonTokenId::kValueBoolean:
        case JsonTokenId::kValueString:
        case JsonTokenId::kValueNull:
          map.emplace (std::move (k->value<std::string>()), JsonValue { std::move (v.value()) });
          break;
        case JsonTokenId::kObjectBegin:
          if (auto obj =_parseObject(); obj.has_value()) {
            map.emplace (std::move (k->value<std::string>()), std::move (obj.value()));
            break;
          }

          return std::nullopt;
        case JsonTokenId::kArrayBegin:
          if (auto arr = _parseArray(); arr.has_value()) {
            map.emplace (std::move (k->value<std::string>()), std::move (arr.value()));
            break;
          }

          return std::nullopt;
        default:
          return _setError (ErrorCode::kExpectAny);
      }
    }
    else {
      return _setError (ErrorCode::kExpectPair);
    }

    const auto n { _tokenizer->next() };
    if (!n.has_value())
      return _setError (ErrorCode::kExpectCommaOrEndObj);

    if (n->id() == JsonTokenId::kObjectEnd)
      return JsonValue { std::move (map) };

    if (n->id() != JsonTokenId::kComma)
      break;
  }
  while (true);

  return _setError (ErrorCode::kExpectCommaOrEndObj);
}

// ----------------------------------------------------------------------------
// JsonParser::_parseArray
// ----------------------------------------------------------------------------
std::optional<JsonValue> JsonParser::_parseArray () {
  std::vector<JsonValue> array;

  do {
    auto v { _tokenizer->next() };
    if (v.has_value()) {
      switch (v->id()) {
        case JsonTokenId::kValueInteger:
        case JsonTokenId::kValueFloatPoint:
        case JsonTokenId::kValueBoolean:
        case JsonTokenId::kValueString:
        case JsonTokenId::kValueNull:
          array.push_back (JsonValue { std::move (v.value()) });
          break;
        case JsonTokenId::kObjectBegin:
          if (auto obj = _parseObject(); obj.has_value()) {
            array.push_back (std::move (obj.value()));
            break;
          }
          return std::nullopt;
        case JsonTokenId::kArrayBegin:
          if (auto arr = _parseArray(); arr.has_value()) {
            array.push_back (std::move (arr.value()));
            break;
          }
          return std::nullopt;
        default:
          return _setError (ErrorCode::kExpectAny);
      }
    }
    else {
      return _setError (ErrorCode::kExpectAny);
    }

    const auto n { _tokenizer->next() };
    if (!n.has_value())
      return _setError (ErrorCode::kExpectCommaOrEndArray);

    if (n->id() == JsonTokenId::kArrayEnd)
      return JsonValue { std::move (array) };

    if (n->id() != JsonTokenId::kComma)
      break;
  }
  while (true);

  return _setError (ErrorCode::kExpectCommaOrEndArray);
}

}
