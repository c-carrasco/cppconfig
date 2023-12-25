// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <cstring>
#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include <cppconfig/mm_file.h>


// ----------------------------------------------------------------------------
// test_emtpy
// ----------------------------------------------------------------------------
TEST (MMapFile, test_mmap) {
  const auto fileName { std::filesystem::temp_directory_path() / "test_mmap_file.bin" };
  std::filesystem::remove (fileName);

  std::ofstream file { fileName, std::ios::out };
  file << "test line 01" << std::endl;
  file << "hello world!" << std::endl;
  file.close();

  cppconfig::util::MMapFile mmFile {};
  ASSERT_FALSE (mmFile.isOpen());
  ASSERT_EQ (mmFile.bytes(), 0);
  ASSERT_EQ (mmFile.data(), nullptr);

  ASSERT_TRUE (mmFile.open (fileName));
  ASSERT_FALSE (mmFile.open (fileName));

  ASSERT_TRUE (mmFile.isOpen());
  ASSERT_EQ (mmFile.bytes(), 26);
  ASSERT_EQ (std::memcmp (mmFile.data(), "test line 01\nhello world!\n", mmFile.bytes()), 0);

  ASSERT_TRUE (mmFile.close());
  ASSERT_FALSE (mmFile.isOpen());
  ASSERT_EQ (mmFile.bytes(), 0);
  ASSERT_EQ (mmFile.data(), nullptr);

  ASSERT_FALSE (mmFile.close());
}
