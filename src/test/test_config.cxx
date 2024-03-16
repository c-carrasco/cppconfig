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
// test_array_types
// ----------------------------------------------------------------------------
TEST (Config, test_array_types) {
  const cppconfig::Config config0 { R"({ "array": [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ] })" };
  ASSERT_EQ (config0.get<std::vector<int64_t>>  ("array").value()[0], 1);
  ASSERT_EQ (config0.get<std::vector<int32_t>>  ("array").value()[1], 2);
  ASSERT_EQ (config0.get<std::vector<int16_t>>  ("array").value()[2], 3);
  ASSERT_EQ (config0.get<std::vector<int8_t>>   ("array").value()[3], 4);
  ASSERT_EQ (config0.get<std::vector<uint64_t>> ("array").value()[4], 5);
  ASSERT_EQ (config0.get<std::vector<uint32_t>> ("array").value()[5], 6);
  ASSERT_EQ (config0.get<std::vector<uint16_t>> ("array").value()[6], 7);
  ASSERT_EQ (config0.get<std::vector<uint8_t>>  ("array").value()[7], 8);

  const cppconfig::Config config1 { R"({ "array": [ 1.0, 2.0 ] })" };
  ASSERT_EQ (config1.get<std::vector<float>>  ("array").value()[0], 1.0);
  ASSERT_EQ (config1.get<std::vector<double>> ("array").value()[1], 2.0);

  const cppconfig::Config config2 { R"({ "array": [ true, false ] })" };
  ASSERT_EQ (config2.get<std::vector<bool>> ("array").value()[0], true);
  ASSERT_EQ (config2.get<std::vector<bool>> ("array").value()[1], false);

  const cppconfig::Config config3 { R"({ "array": [ "hello", "world" ] })" };
  ASSERT_EQ (config3.get<std::vector<std::string>> ("array").value()[0], "hello");
  ASSERT_EQ (config3.get<std::vector<std::string>> ("array").value()[1], "world");
}

// ----------------------------------------------------------------------------
// test_basic_types
// ----------------------------------------------------------------------------
TEST (Config, test_basic_types) {
  cppconfig::Config config0 { R"({ "int": 123 })" };
  ASSERT_EQ (config0.get<int64_t>  ("int").value(), 123);
  ASSERT_EQ (config0.get<int32_t>  ("int").value(), 123);
  ASSERT_EQ (config0.get<int16_t>  ("int").value(), 123);
  ASSERT_EQ (config0.get<int8_t>   ("int").value(), 123);
  ASSERT_EQ (config0.get<uint64_t> ("int").value(), 123);
  ASSERT_EQ (config0.get<uint32_t> ("int").value(), 123);
  ASSERT_EQ (config0.get<uint16_t> ("int").value(), 123);
  ASSERT_EQ (config0.get<uint8_t>  ("int").value(), 123);

  cppconfig::Config config1 { R"({ "fp": 123.0 })" };
  ASSERT_EQ (config1.get<float> ("fp").value(), 123.0);
  ASSERT_EQ (config1.get<double> ("fp").value(), 123.0);

  cppconfig::Config config2 { R"({ "bool": true })" };
  ASSERT_EQ (config2.get<bool> ("bool").value(), true);

  cppconfig::Config config3 { R"({ "str": "hello" })" };
  ASSERT_EQ (config3.get<std::string> ("str").value(), "hello");
}

// ----------------------------------------------------------------------------
// test_object_multilevel
// ----------------------------------------------------------------------------
TEST (Config, test_object_multilevel) {
  cppconfig::Config config0 { R"({ "key1": { "key2": { "key3": 99 } } })" };
  ASSERT_EQ (config0.get<int32_t> ("key1.key2.key3").value(), 99);
  ASSERT_FALSE (config0.get<int32_t> ("key1.key2.keyx").has_value());
  ASSERT_FALSE (config0.get<int32_t> ("key1.keyx.key3").has_value());
  ASSERT_FALSE (config0.get<int32_t> ("keyx.key2.key3").has_value());
}

// ----------------------------------------------------------------------------
// test_key_with_dot
// ----------------------------------------------------------------------------
TEST (Config, test_key_with_dot) {
  cppconfig::Config config0 { R"({ "key.01": { "key.02": "test" } })" };
  ASSERT_EQ (config0.get<std::string> ("key\\.01.key\\.02").value(), "test");
}

// ----------------------------------------------------------------------------
// test_array
// ----------------------------------------------------------------------------
TEST (Config, test_array) {
  cppconfig::Config config0 { R"({ "array": [ { "a": 1 }, { "b": 2.0 }, { "c": "str" } ] })" };
  ASSERT_EQ (config0.get<int32_t> ("array[0].a").value(), 1);
  ASSERT_EQ (config0.get<float> ("array[1].b").value(), 2.0);
  ASSERT_EQ (config0.get<std::string> ("array[2].c").value(), "str");

  cppconfig::Config config1 { R"([ 1.0, true, [ 1, [ 2 ] ], { "a": [ 3, [ 4 ] ] } ])" };
  ASSERT_EQ (config1.get<float> ("[0]").value(), 1.0);
  ASSERT_EQ (config1.get<bool> ("[1]").value(), true);
  ASSERT_EQ (config1.get<int32_t> ("[2][0]").value(), 1);
  ASSERT_EQ (config1.get<int32_t> ("[2][1][0]").value(), 2);
  ASSERT_EQ (config1.get<int32_t> ("[3].a[0]").value(), 3);
  ASSERT_EQ (config1.get<int32_t> ("[3].a[1][0]").value(), 4);
}

// ----------------------------------------------------------------------------
// test_empty_config
// ----------------------------------------------------------------------------
TEST (Config, test_empty_config) {
  cppconfig::Config config0 { R"({})" };
  ASSERT_FALSE (config0.get<std::string> ("hello").has_value());

  cppconfig::Config config1 { R"({ "array": [] })" };
  ASSERT_EQ (config1.get<std::vector<int32_t>> ("array").value().size(), 0);
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
  ASSERT_EQ (config.get<std::vector<int32_t>> ("array").value()[1], 2);
  ASSERT_EQ (config.get<std::vector<int16_t>> ("array").value()[2], 3);
}

// ----------------------------------------------------------------------------
// test_folder_load_def_env
// ----------------------------------------------------------------------------
TEST (Config, test_folder_load_def_env) {
  const auto folder { cppconfig::util::PathUtil::getProgramDirPath() / "data" / "test" / "config02" };
  const MockSystem mock { "myhostname", "myenvname" };

  const cppconfig::Config config { folder, mock };

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

  const cppconfig::Config config { folder, mock };

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

  const cppconfig::Config config { folder, mock };

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
// test_folder_missing_default
// ----------------------------------------------------------------------------
TEST (Config, test_folder_missing_default) {
  const auto folder { cppconfig::util::PathUtil::getProgramDirPath() / "data" / "test" / "config04" };

  ASSERT_THROW (cppconfig::Config config (folder), std::ios_base::failure);
}
