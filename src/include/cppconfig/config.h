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

/// @class Config
/// @brief Manages application configuration by handling JSON configuration files.
///
/// This class enables the definition and management of a set of default parameters,
/// which can be extended or overridden based on different deployment environments
/// (e.g., development, QA, staging, production) and specific host names. Configuration
/// files should be placed within the application's directory structure. They can
/// subsequently be extended or overridden through the use of environment variables
/// and host names, allowing for flexible and dynamic configuration management.
///
/// The class primarily relies on an environment variable (`CPPCONFIG_ENV`) to determine
/// the deployment environment, and the system's host name to apply host-specific
/// configurations.
///
/// The CPPCONFIG_ENV environment variable should contain the deployment environment name of the
/// application. It is crucial for determining the order in which configuration files
/// are loaded. Common values for `CPPCONFIG_ENV` include `development`, `production`, etc.
///
/// The system's host name (as returned by the `hostname` command in lowercase) is
/// utilized as a part of the configuration file naming convention, enabling
/// host-specific configurations.
///
/// The class loads configuration files in the following order, allowing each step
/// to override or extend the previous:
///  @li default.json - The base configuration.
///  @li {deployment}.json - Deployment-specific configuration, where `{deployment}`
///                          is derived from the `CPPCONFIG_ENV` environment variable.
///  @li {hostname}.json - Host-specific configuration, where `{hostname}` is the
///                        system's host name in lowercase.
class Config {
  public:
    /// @brief Represents a system configuration interface.
    /// Provides an interface to retrieve the host name and the environment variable name
    /// that are used by the Config class to load configuration settings.
    struct System {
      /// @brief Virtual destructor.
      virtual ~System() {}

      /// @brief Retrieves the host name of the system.
      /// @return A constant reference to a std::string containing the host name.
      virtual const std::string & getHostName() const;

      /// @brief Retrieves the environment variable name used for configuration.
      /// @return A constant reference to a std::string containing the environment variable name.
      virtual const std::string & getEnvName() const;

      /// @brief Returns a reference to the singleton instance of the System class.
      /// @return A constant reference to the singleton System instance.
      static const System & instance() {
        static System instance;

        return instance;
      }
    };

    /// @brief Constructs a Config object with the specified file path.
    /// @param fileName The path to the configuration file.
    Config (const std::filesystem::path &fileName, const System &system = System::instance());

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
    /// The supported types for T are:
    ///   @li integer: int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t and uint8_t
    ///   @li float-point: float, double
    ///   @li string: std::string
    ///   @li bloolean: bool
    ///   @li vector: std::vector<integer|float-point|string|boolean>
    /// @param key The key to look up in the configuration, supporting dot notation for nested objects
    /// and array indexing with '[]' (e.g., "key1.array[3].key2").
    /// @return An optional containing the retrieved value, or std::nullopt if the key is not found.
    template<typename T = std::string>
    inline std::optional<T> get (std::string_view key) {
      const auto jsonVal { _getJsonValue (key) };
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
          (std::is_same_v<T, std::vector<double>>) ||
          (std::is_same_v<T, std::vector<int64_t>>) ||
          (std::is_same_v<T, std::vector<bool>>)
        ) {
          T result;
          for (const auto &jv: jsonVal.value().get().asArray())
            result.push_back (jv.get<typename T::value_type>());
          return result;
        }
        else if constexpr (
          (std::is_same_v<T, std::vector<int32_t>>) ||
          (std::is_same_v<T, std::vector<int16_t>>) ||
          (std::is_same_v<T, std::vector<int8_t>>) ||
          (std::is_same_v<T, std::vector<uint64_t>>) ||
          (std::is_same_v<T, std::vector<uint32_t>>) ||
          (std::is_same_v<T, std::vector<uint16_t>>) ||
          (std::is_same_v<T, std::vector<uint8_t>>)
        ) {
          T result;
          for (const auto &jv: jsonVal.value().get().asArray())
            result.push_back (static_cast<typename T::value_type> (jv.get<int64_t>()));
          return result;
        }
        else if constexpr (std::is_same_v<T, std::vector<float>>) {
          T result;
          for (const auto &jv: jsonVal.value().get().asArray())
            result.push_back (static_cast<typename T::value_type> (jv.get<double>()));
          return result;
        }
        else {
          return jsonVal.value().get().get<T>();
        }
      }

      return std::nullopt;
    }

  private:
    json::JsonParser _parser {}; /// JSON parser for parsing configuration data.
    std::optional<json::JsonValue> _root {}; /// Root JSON value representing the configuration.

    /// @brief Gets a reference to the JSON value associated with the specified key.
    /// @param sv The key to look up in the configuration.
    /// @return An optional reference to the JSON value, or std::nullopt if the key is not found.
    std::optional<std::reference_wrapper<json::JsonValue>> _getJsonValue (const std::string_view &sv);

    /// @brief Loads a JSON file and returns its parsed content.
    /// @param fileName The path to the JSON file to be loaded.
    /// @return An optional containing the parsed JSON content if successful, or an empty optional
    ///         if the file is not found or an error occurs during parsing.
    /// @throws std::ios_base::failure if the specified file is not found.
    std::optional<json::JsonValue> _loadFile (const std::filesystem::path &fileName);

    /// @brief Loads configuration files from a specified folder based on the given system.
    /// @param folderName The path to the folder containing configuration files.
    /// @param system The system information used to determine the environment and host-specific files.
    /// @throws std::runtime_error if any of the configuration files cannot be loaded or parsed successfully.
    void _loadFolder (const std::filesystem::path &folderName, const System &system);
};

}

#endif