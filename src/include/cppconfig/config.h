// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023-2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#ifndef __CPP_CONFIG_H__
#define __CPP_CONFIG_H__
#include <filesystem>
#include <string_view>

#include <cppconfig/json_parser.h>


namespace cppconfig {

/// @brief Represents a configuration class for handling JSON configuration files.
class Config {
  public:
    /// @brief Constructs a Config object with the specified file path.
    /// @param fileName The path to the configuration file.
    Config (const std::filesystem::path &fileName);

    /// @brief Constructs a Config object with the provided JSON buffer.
    /// @param buffer The JSON buffer.
    /// @param len The length of the buffer (default is 0, which assumes a null-terminated buffer).
    Config (const char *buffer, size_t len = 0);

    /// @brief Parses the provided JSON buffer and updates the configuration.
    /// @param buffer The JSON buffer.
    /// @param len The length of the buffer (default is 0, which assumes a null-terminated buffer).
    /// @return True if parsing is successful, false otherwise.
    bool parse (const char *buffer, size_t len = 0);

    /// @brief Retrieves a configuration value of the specified type.
    /// @tparam T The type of the configuration value.
    /// @param sv The key to look up in the configuration.
    /// @return An optional containing the retrieved value, or std::nullopt if the key is not found.
    template<typename T = std::string>
    inline std::optional<T> get (std::string_view sv) {
      const auto jsonVal { _getJsonValue (sv) };
      if (jsonVal.has_value()) {
        if constexpr (std::is_same_v<T, bool>) {
          return static_cast<T> (jsonVal.value().get().get<bool>());
        }
        else if constexpr (std::is_integral_v<T>) {
          return static_cast<T> (jsonVal.value().get().get<int64_t>());
        }
        else if constexpr (std::is_floating_point_v<T>) {
          return static_cast<T> (jsonVal.value().get().get<double>());
        }
        else if constexpr (
          (std::is_same_v<T, std::vector<std::string>>) ||
          (std::is_same_v<T, std::vector<int64_t>>) ||
          (std::is_same_v<T, std::vector<double>>) ||
          (std::is_same_v<T, std::vector<bool>>)
        ) {
          T result;
          for (const auto &jv: jsonVal.value().get().asArray())
            result.push_back (jv.get<typename T::value_type>());
          return result;
        }
        else {
          return jsonVal.value().get().get<T>();
        }
      }

      return std::nullopt;
    }

  private:
    json::JsonParser _parser {}; ///< JSON parser for parsing configuration data.
    std::optional<json::JsonValue> _root {}; ///< Root JSON value representing the configuration.

    /// @brief  Gets a reference to the JSON value associated with the specified key.
    /// @param sv The key to look up in the configuration.
    /// @return An optional reference to the JSON value, or std::nullopt if the key is not found.
    std::optional<std::reference_wrapper<json::JsonValue>> _getJsonValue (const std::string_view &sv);
};

}

#endif