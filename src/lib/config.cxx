// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023-2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <unistd.h>
#include <limits.h>

#include <stdexcept>
#include <format>

#include <cppconfig/config.h>
#include <cppconfig/mm_file.h>

#if !defined(HOST_NAME_MAX) && defined(_POSIX_HOST_NAME_MAX)
  #define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#endif


namespace cppconfig {

// ----------------------------------------------------------------------------
// Config::System::getHostName
// ----------------------------------------------------------------------------
const std::string & Config::System::getHostName() const {
  static std::string hostName;

  if (hostName.empty()) {
    char buf[HOST_NAME_MAX + 1] = { 0, };
    gethostname (buf, HOST_NAME_MAX);

    hostName = buf;
    std::transform (hostName.begin(), hostName.end(), hostName.begin(), [] (const auto c) {
      return std::tolower (c);
    });
  }

  return hostName;
}

// ----------------------------------------------------------------------------
// Config::System::getEnvName
// ----------------------------------------------------------------------------
const std::string & Config::System::getEnvName() const {
  static std::string env { std::getenv ("CPPCONFIG_ENV")? std::getenv ("CPPCONFIG_ENV") : "" };

  return env;
}

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
Config::Config (const std::filesystem::path &fileName, const System &system) {
  if (std::filesystem::is_directory (fileName))
    _loadFolder (fileName, system);
  else
    _root = _loadFile (fileName);
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
std::optional<std::reference_wrapper<const json::JsonValue>> Config::_getJsonValue (const std::string_view &sv) const {
  std::reference_wrapper<const json::JsonValue> v { _root.value() };

  int32_t index { -1 };
  std::string str {};
  str.reserve (sv.size());
  for (size_t i { 0 }; i < sv.size(); ++i) {
    if ((sv[i] == '\\') && (sv.size() > i + 1) && (sv[i + 1] == '.')) {
      ++i;
      str.push_back('.');
    }
    else if ((sv[i] == '.') || (sv[i] == ']')) {
      if (!str.empty()) {
        if (!v.get().exists (str))
          return std::nullopt;

        v = v.get()[str];

        str.clear();
      }

      if (index != -1) {
        if (!v.get().isArray() || (index >= std::ssize (v.get().asArray())))
          return std::nullopt;

        v = v.get()[index];

        index = -1;
      }
    }
    else if ((sv[i] == '[')) {
      index = 0;
      for (i = i + 1; i < sv.size(); ++i) {
        if (std::isdigit (sv[i])) {
          index = index * 10 + (static_cast<size_t>(sv[i]) - 48);
        }
        else if (sv[i] == ']') {
          --i;
          break;
        }
        else {
          return std::nullopt;
        }
      }
    }
    else {
      str.push_back (sv[i]);
    }
  }

  if (!str.empty()) {
    if (!v.get().exists (str))
      return std::nullopt;

    v = v.get()[str];
  }

  if (index != -1) {
    if (!v.get().isArray() || (index >= std::ssize (v.get().asArray())))
      return std::nullopt;

    v = v.get()[index];
  }

  return v;
}

// ----------------------------------------------------------------------------
// Config::_loadFile
// ----------------------------------------------------------------------------
std::optional<json::JsonValue> Config::_loadFile (const std::filesystem::path &fileName) {
  util::MMapFile<> mmFile {};

  if (!mmFile.open (fileName))
    throw std::ios_base::failure { "File '" + fileName.string() + "' not found" };

  return _parser.parse (mmFile.data(), mmFile.bytes());
}

// ----------------------------------------------------------------------------
// Config::_loadFolder
// ----------------------------------------------------------------------------
void Config::_loadFolder (const std::filesystem::path &folderName, const System &system) {
  const auto defaultFileName { folderName / "default.json" };
  const auto envFileName { (folderName / system.getEnvName()).replace_extension ("json") };
  const auto hostFileName { (folderName / system.getHostName()).replace_extension ("json") };

  _root = _loadFile (defaultFileName);
  if (!_root.has_value())
    throw std::runtime_error { defaultFileName.string() + ":" + _parser.error().str() };

  if (std::filesystem::exists (envFileName)) {
    auto envDoc = _loadFile (envFileName);

    if (!envDoc.has_value())
      throw std::runtime_error { envFileName.string() + ":" + _parser.error().str() };

    json::JsonValue::merge (envDoc.value(), _root.value());
  }

  if (std::filesystem::exists (hostFileName)) {
    auto hostDoc = _loadFile (hostFileName);

    if (!hostDoc.has_value())
      throw std::runtime_error { hostFileName.string() + ":" + _parser.error().str() };

    json::JsonValue::merge (hostDoc.value(), _root.value());
  }
}

}
