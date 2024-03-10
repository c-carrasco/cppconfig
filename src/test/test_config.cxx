// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023-2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <cstring>

#include <gtest/gtest.h>

#include <cppconfig/config.h>
#include <cppconfig/path_util.h>


// ----------------------------------------------------------------------------
struct MockSystem: public cppconfig::Config::System {
  MockSystem (const char *h, const char *e): hostName { h }, env { e } {}

  const std::string & getHostName() const override {
    return hostName;
  }

  const std::string & getEnvName() const override {
    return env;
  }

  std::string hostName;
  std::string env;
};


// ----------------------------------------------------------------------------
// test_system
// ----------------------------------------------------------------------------
TEST (System, test_system) {
  cppconfig::Config::System system {};

  ASSERT_FALSE (system.getHostName().empty());
  ASSERT_TRUE (system.getEnvName().empty());
}

// ----------------------------------------------------------------------------
// test_config_file
// ----------------------------------------------------------------------------
TEST (Config, test_config_file) {
  const auto fileName { cppconfig::util::PathUtil::getProgramDirPath() / "data" / "test" / "config00.json" };

  cppconfig::Config config { fileName };

  ASSERT_TRUE (config.get<bool> ("enabled").value());
  ASSERT_EQ (config.get<std::string> ("name").value(), "test00");
  ASSERT_EQ (config.get<int32_t> ("value").value(), 123);
}

// ----------------------------------------------------------------------------
// test_folder_default
// ----------------------------------------------------------------------------
TEST (Config, test_folder_default) {
  const auto folder { cppconfig::util::PathUtil::getProgramDirPath() / "data" / "test" / "config01" };

  cppconfig::Config config { folder };

  ASSERT_EQ (config.get<std::string> ("name").value(), "test01");
  ASSERT_EQ (config.get<float> ("value").value(), 10.0);
  ASSERT_EQ (config.get<std::vector<int64_t>> ("array").value()[0], 1);
  ASSERT_EQ (config.get<std::vector<int64_t>> ("array").value()[1], 2);
  ASSERT_EQ (config.get<std::vector<int64_t>> ("array").value()[2], 3);
}

// ----------------------------------------------------------------------------
// test_folder_load_def_env
// ----------------------------------------------------------------------------
TEST (Config, test_folder_load_def_env) {
  const auto folder { cppconfig::util::PathUtil::getProgramDirPath() / "data" / "test" / "config02" };
  const MockSystem mock { "myhostname", "myenvname" };

  cppconfig::Config config { folder, mock };

  ASSERT_EQ (config.get<bool> ("key_1").value(), false);
  ASSERT_EQ (config.get<int32_t> ("key_2").value(), 100);
  ASSERT_EQ (config.get<uint32_t> ("key_3").value(), 20);
  ASSERT_EQ (config.get<std::string> ("sub_key_1.key_1_1").value(), std::string { "val1" });
  ASSERT_EQ (config.get<float> ("sub_key_1.key_1_2").value(), 2.0);
  ASSERT_EQ (config.get<bool> ("sub_key_1.new").value(), true);
  ASSERT_EQ (
    config.get<std::vector<std::string>> ("sub_key_1.sub_key_1_3.key_1_3_1").value(),
    (std::vector<std::string> { "one", "foo" })
  );
}

// ----------------------------------------------------------------------------
// test_folder_load_def_env_host
// ----------------------------------------------------------------------------
TEST (Config, test_folder_load_def_env_host) {
  const auto folder { cppconfig::util::PathUtil::getProgramDirPath() / "data" / "test" / "config03" };
  const MockSystem mock { "myhostname", "myenvname" };

  cppconfig::Config config { folder, mock };

  ASSERT_EQ (config.get<bool> ("key_1"), false);
  ASSERT_EQ (config.get<int32_t> ("key_2"), 100);
  ASSERT_EQ (config.get<uint32_t> ("key_3"), 20);
  ASSERT_EQ (config.get<std::string> ("sub_key_1.key_1_1"), std::string ("val1111"));
  ASSERT_EQ (config.get<float> ("sub_key_1.key_1_2"), 2.0);
  ASSERT_EQ (config.get<bool> ("sub_key_1.new"), true);
  ASSERT_EQ (
    config.get<std::vector<std::string>> ("sub_key_1.sub_key_1_3.key_1_3_1").value(),
    (std::vector<std::string> { "one", "two", "foo" })
  );
}

// ----------------------------------------------------------------------------
// test_folder_load_def_host
// ----------------------------------------------------------------------------
TEST (Config, test_folder_load_def_host) {
  MockSystem mock { "myhostname", "" };
  const auto folder { cppconfig::util::PathUtil::getProgramDirPath() / "data" / "test" / "config03" };

  cppconfig::Config config { folder, mock };

  ASSERT_EQ (config.get<bool> ("key_1"), false);
  ASSERT_EQ (config.get<int32_t> ("key_2"), 8);
  ASSERT_EQ (config.get<uint32_t> ("key_3"), 20);
  ASSERT_EQ (config.get<std::string> ("sub_key_1.key_1_1"), std::string ("val1111"));
  ASSERT_EQ (config.get<float> ("sub_key_1.key_1_2"), 1.5);
  ASSERT_EQ (
    config.get<std::vector<std::string>> ("sub_key_1.sub_key_1_3.key_1_3_1").value(),
    (std::vector<std::string> { "one", "two", "foo" })
  );
}

// ----------------------------------------------------------------------------
// test_folder_array
// ----------------------------------------------------------------------------
// TEST (Config, test_folder_array) {
//   const MockSystem mock { "myhostname", "myenvname" };
//   const auto folder { cppconfig::util::PathUtil::getProgramDirPath() / "data" / "test" / "config05" };

//   cppconfig::Config config { folder, mock };
// }

// ----------------------------------------------------------------------------
// test_folder_missing_default
// ----------------------------------------------------------------------------
TEST (Config, test_folder_missing_default) {
  const auto folder { cppconfig::util::PathUtil::getProgramDirPath() / "data" / "test" / "config04" };

  ASSERT_THROW (cppconfig::Config config (folder), std::ios_base::failure);
}

// ----------------------------------------------------------------------------
// test_config
// ----------------------------------------------------------------------------
TEST (Config, test_config) {
  constexpr const char *str { R"({
    "int": 123,
    "float": 1.5,
    "bool": true,
    "str.1": "hello",
    "obj": {
      "s0": -10,
      "s1": -2.5,
      "s2": false,
      "s4": "test",
      "s5": {
        "k0": -20,
        "k1": [ "zero", "one" ]
      },
      "s6": null
    },
    "a1": [ 0, 1 ],
    "a2": [ 0.0, 1.0 ],
    "a3": [ true, false ]
  })" };

  cppconfig::Config config { str };

  ASSERT_EQ (config.get<uint8_t> ("int").value(), 123);
  ASSERT_EQ (config.get<int32_t> ("int").value(), 123);
  ASSERT_EQ (config.get<uint64_t> ("int").value(), 123);
  ASSERT_EQ (config.get<float> ("float").value(), 1.5);
  ASSERT_EQ (config.get<double> ("float").value(), 1.5);
  ASSERT_TRUE (config.get<bool> ("bool").value());
  ASSERT_EQ (config.get<std::string> ("str\\.1").value(), "hello");
  ASSERT_EQ (config.get<int64_t> ("obj.s0").value(), -10);
  ASSERT_EQ (config.get<double> ("obj.s1").value(), -2.5);
  ASSERT_FALSE (config.get<bool> ("obj.s2").value());
  ASSERT_EQ (config.get<std::string> ("obj.s4").value(), "test");
  ASSERT_EQ (config.get<int32_t> ("obj.s5.k0").value(), -20);
  ASSERT_EQ (config.get<std::nullptr_t> ("obj.s6").value(), nullptr);
  const auto array1 { config.get<std::vector<std::string>> ("obj.s5.k1").value() };
  ASSERT_EQ (array1.size(), 2);
  ASSERT_EQ (array1[0], "zero");
  ASSERT_EQ (array1[1], "one");
  const auto array2 { config.get<std::vector<int64_t>> ("a1").value() };
  ASSERT_EQ (array2.size(), 2);
  ASSERT_EQ (array2[0], 0);
  ASSERT_EQ (array2[1], 1);
  const auto array3 { config.get<std::vector<double>> ("a2").value() };
  ASSERT_EQ (array3.size(), 2);
  ASSERT_EQ (array3[0], 0.0);
  ASSERT_EQ (array3[1], 1.0);
  ASSERT_FALSE (config.get ("do-not-exists").has_value());
}

