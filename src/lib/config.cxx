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

    _merge (envDoc.value(), _root.value());
  }

  if (std::filesystem::exists (hostFileName)) {
    auto hostDoc = _loadFile (hostFileName);

    if (!hostDoc.has_value())
      throw std::runtime_error { hostFileName.string() + ":" + _parser.error().str() };

    _merge (hostDoc.value(), _root.value());
  }
}

// ----------------------------------------------------------------------------
// Config::_merge
// ----------------------------------------------------------------------------
bool Config::_merge (json::JsonValue &src, json::JsonValue &dst) {
  if (src.isObject()) {
    for (auto itSrc = src.asObject().begin(); itSrc != src.asObject().end(); ++itSrc) {
      const auto &key { itSrc->first };

      const auto itDst { dst.asObject().find (key) };
      if (itDst == dst.asObject().end()) { // source item is NOT present in destination -> insert
        if (!dst.asObject().emplace (key, itSrc->second).second)
          return false; // could not be inserted
      }
      else { // source item is present in destination -> update
        const auto &srcType { itSrc->second.type() };
        const auto &dstType { itDst->second.type() };

        if (srcType != dstType)
          return false;

        if (itSrc->second.isObject()) {
          if (!_merge (itSrc->second, itDst->second))
            return false;
        }
        else if (itSrc->second.isArray()) {
          for (const auto &item: itSrc->second.asArray())
            itDst->second.asArray().push_back (item);
        }
        else {
          itDst->second = itSrc->second;
        }
      }
    }
  }
  else if (src.isArray()) {
    // FIXME: implement
    assert (false);
  }

  return true;
}

}
