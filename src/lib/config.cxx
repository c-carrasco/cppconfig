// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023-2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <stdexcept>

#include <cppconfig/config.h>
#include <cppconfig/mm_file.h>


namespace cppconfig {

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
Config::Config (const std::filesystem::path &fileName) {
  util::MMapFile<> mmFile {};

  if (!mmFile.open (fileName))
    throw std::ios_base::failure { "File '" + fileName.string() + "' not found" };

  if (!parse (mmFile.data(), mmFile.bytes())) {
    throw std::runtime_error {
      "Parse error:" + std::to_string (_parser.error().line) + ", " + std::to_string (_parser.error().column)
    };
  }
}

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
Config::Config (const char *buffer, size_t len) {
  if (!parse (buffer, len? len : std::strlen (buffer))) {
    throw std::runtime_error {
      "Parse error:" + std::to_string (_parser.error().line) + ", " + std::to_string (_parser.error().column)
    };
  }
}

// ----------------------------------------------------------------------------
// Config::parse
// ----------------------------------------------------------------------------
bool Config::parse (const char *buffer, size_t len) {
  _root = _parser.parse (buffer, len? len : std::strlen (buffer));
  if (!_root.has_value())
    return false;
  return true;
}

// ----------------------------------------------------------------------------
// Config::_getJsonValue
// ----------------------------------------------------------------------------
std::optional<std::reference_wrapper<json::JsonValue>> Config::_getJsonValue (const std::string_view &sv) {
  std::reference_wrapper<json::JsonValue> v { _root.value() };

  std::string str {};
  str.reserve (sv.size());
  for (size_t i { 0 }; i < sv.size(); ++i) {
    if ((sv[i] == '\\') && (i + 1) && (sv[i + 1] == '.')) {
      ++i;
      str.push_back('.');
    }
    else if (sv[i] == '.') {
      if (!v.get().exists (str))
        return std::nullopt;

      v = v.get()[str];

      str.clear();
    }
    else {
      str.push_back (sv[i]);
    }
  }

  if (!v.get().exists (str))
    return std::nullopt;

  v = v.get()[str];

  return v;
}

}
