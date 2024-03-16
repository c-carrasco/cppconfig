CppConfig Library
=================

A C++ Library for Configuration Management

# Overview
CppConfig is a C++ library designed to simplify the management of configuration files in applications. It supports JSON-based configuration, enabling developers to define default parameters that can be extended or overridden based on different deployment environments (e.g., development, QA, staging, production) and/or host names. The library uses environment variables and the host name to determine which configuration files to load, making it highly flexible and adaptable to various deployment strategies.

# Key Features
- Environment-Specific Configuration: CppConfig leverages an environment variable, CPPCONFIG_ENV, to identify the deployment environment (e.g., development, production). This feature allows for seamless transitions between different stages of development by loading environment-specific configuration files.
- Host-Specific Configuration: The library uses the system's host name (in lowercase) to load host-specific configuration files, providing a convenient way to apply unique settings to different servers or development machines.
- File Load Order: CppConfig prioritizes configuration files in the following order:
  - 1. default.json - Serves as the base configuration.
  - 2. {deployment}.json - Overrides or extends the base configuration based on the deployment environment.
  - 3. {hostname}.json - Further customizes the configuration for specific hosts.
- Flexible Value Retrieval: CppConfig supports retrieving configuration values of various types, including integers, floating-point numbers, strings, booleans, and vectors of these types. This flexibility ensures that developers can easily access the configuration data they need in the appropriate format.

# Usage

## Defining Configuration Files
Configuration files should be in JSON format and placed within your application's directory structure following the file load order mentioned above. Here's an example of a default.json file:

```JSON
{
  "database": {
    "host": "localhost",
    "port": 3306
  },
  "logging": {
    "level": "info",
    "file": "app.log"
  }
}
```

## Environment Variable

To specify the deployment environment, set the CPPCONFIG_ENV environment variable:
```SHELL
export CPPCONFIG_ENV=development
```

## Loading Configuration in Your Application
Include the CPPConfig library and use the Config class to load and access your application's configuration:
```CPP
#include <cppconfig/config.h>

int main() {
  cppconfig::Config config("path/to/config/folder");

  auto dbHost = config.get<std::string>("database.host").value_or("default_host");
  auto dbPort = config.get<int>("database.port").value_or(3306);

  // Use the configuration values in your application
}
```

## Extending or Overriding Configuration (Optional)

Create additional JSON files named after the deployment environment and/or host name to override or extend the base configuration. For example, development.json could override the logging level to debug for development purposes:
```JSON
{
  "logging": {
    "level": "debug"
  }
}
```

## System Class

The Config::System interface provides methods for retrieving the host name and the environment variable name. It's utilized internally by the Config class but can also be extended if you need to customize the way these values are retrieved.

## Getting Values

### Basic types

- Integers: int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t
- Float Points: float, double
- strings: std::string
- Booleans: bool

Examples:
```JSON
{
  "product": {
    "id": 123456,
    "name": "Laptop XYZ ACME",
    "out_of_stock": false,
    "price": 9999.90,
    "currency": "USD"
  }
}
```

```CPP
const auto id { config<uint64_t> ("product.id") };
const auto name { config<std::string> ("product.name" ) };
const auto outOfStock { config<bool> ("product.out_of_stock") };
const auto price { config<float> ("product.price") };
```

### Arrays

- Vector of integers: std::vector<int64_t>, std::vector<int32_t>, std::vector<int16_t>, std::vector<int8_t>, std::vector<uint64_t>, std::vector<uint32_t>, std::vector<uint16_t>, std::vector<uint8_t>
- Vector of Float Points: std::vector<float>, std::vector<double>
- Vector of Booleans: std::vector<bool>

Examples:
```JSON
{
  "myserver": {
    "ip": [ "1.2.3.4", "6.7.8.9" ]
  },
  "host": [
    {
      "name": "acme.com",
      "port": 1234
    },
    {
      "name": "foo.com",
      "port": 5678
    }
  ]
}
```

```CPP
const auto ips { config<std::vector<std::string>> ("myserver.ip") };

const auto host0 { config<std::string> ("host[0].name") };
const auto host1 { config<std::string> ("host[1].name") };
const auto port0 { config<uint16_t> ("host[0].port") };
const auto port1 { config<uint16_t> ("host[1].port") };
```

# Conclusion

CppConfig is a powerful library for managing application configurations in C++. By organizing configurations into JSON files and leveraging environment variables and host names, it offers a highly flexible and scalable approach to configuration management across different environments and deployment scenarios.