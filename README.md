Configure your C++ Apps
=======================

[![ci](https://github.com/c-carrasco/cppconfig/actions/workflows/main.yml/badge.svg)](https://github.com/c-carrasco/cppconfig/actions/workflows/main.yml)

# TODO

- [X] JSON Tokenizer
- [X] JSON Parser
- [X] Logic
- [ ] Documentation

# Introduction

CppConfig organizes hierarchical configurations for your app deployments.

It lets you define a set of default parameters, and extend them for different deployment environments (development, qa, staging, production, etc.).

Configurations are stored in configuration files within your application, and can be overridden and extended by environment variables.

# Features

- Cross-platform compatible.
- Simple: Get started fast.
- Powerful: For multi-node enterprise deployment.
- Flexible - Supporting multiple config file formats.

# Quick Start

Edit the default config file.

```SHELL
$> mkdir config
$> vim config/default.json
```

```JSON
{
  "database": {
    "host": "postgresql://localhost",
    "port": 1234,
    "name": "Users"
  }
}
```

Edit config overrides for production deployment:

```SHELL
$> vim config/production.json
```

```JSON
{
  "database": {
    "host": "postgresql://db-prod.acme.com",
  }
}
```

Use configs in your code:

```CPP
#include <iostream>
#include <cppconfig/config.h>

int main (int argc, char *argv[]) {
  // argv[1] = path to the folder containing the json configuration files.
  cppconfig::Config config { argv[1] };

  const auto host { config.get<std::string> ("database.host") };
  const auto port { config.get<uint16> ("database.port" ) };
  const auto db { config.get<std::string> ("database.name" ) };

  std::cout << "host: " << host << std::cendl;
  std::cout << "port: " << port << std::cendl;
  std::cout << "db: " << db << std::cendl;

  return 0;
}
```

Build & launch your app:

```SHELL
$> g++ ... -o my_app
$> CPPCONFIG_ENV=production ./my_app config
host: postgresql://db-prod.acme.com
port: 1234
db: Users
```

# Prerequisites to build this project

Before diving into `cpp-config`, make sure you have the following tools and dependencies set up:

- Conan 1.x
- CMake 3.20 or higer
- GCC, Clang or AppleClang
- GNU Make or ninja
- Docker (optional)
- Doxygen (for generating documentation)

# Build the project

## Achieve Reproducible Builds with Docker

For a streamlined development environment, Docker is your friend. It's highly recommended to utilize Docker for building your code.

## The Building Script

Located in the root folder of the project, the build.sh file acts as the catalyst for your building endeavors.

**Usage Guide**
---------------
* **_release_**
Construct the code in release mode.
* **_debug_**
Construct the code in debug mode (selected if no mode is specified).
* **_clean_**
Wipe out the _build_ directory along with its contents.
* **_verbose_**
Force GNU make to show the commands.
* **_ninja_**
Opt for `ninja` over GNU Make for code compilation.
* **_tests_**
Execute tests post compilation.
* **_asan=on_**
Enable the [Address Sanitizer]
* **_ubsan=on_**
Enable the [Undefined Behavior Sanitizer]
* **_docker[=compiler]_**
Use docker for local development.
  Available options:
    - `gcc13`: Use GCC 13 as default compiler (selected if no compiler is specified).
    - `clang16`: Use Clang 16 as default compiler.

Examples:

```
# Compile code in release mode
./build.sh clean release

# Build code in debug mode using Docker image with Clang 16, Address Sanitizer enabled, and run unit tests
./build.sh docker=clang16 debug test asan=on

# Start docker dev environment with gcc13
./build.sh docker=gcc13
```

# How to use it

TBC

# Example

TBC

# Installation

TBC

# Contributing
We welcome contributions from the community.

# References

https://www.json.org/json-en.html

# License
This project is licensed under the MIT License - see the [LICENSE](./LICENSE) file for details.