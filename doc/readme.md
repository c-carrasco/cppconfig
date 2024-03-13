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

# Conclusion
CPPConfig is a powerful library for managing application configurations in C++. By organizing configurations into JSON files and leveraging environment variables and host names, it offers a highly flexible and scalable approach to configuration management across different environments and deployment scenarios.