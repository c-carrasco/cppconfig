file (GLOB CXX_FILES FILES *.cxx)

set (EXE_NAME "test_cxxlogger")

add_executable (${EXE_NAME} ${CXX_FILES})

target_include_directories(${EXE_NAME} PRIVATE ${GTEST_INCLUDE_DIRECTORIES})

target_link_libraries(${EXE_NAME}
  cppconfig
  GTest::GTest
)

add_test (NAME ${EXE_NAME} COMMAND $<TARGET_FILE:${EXE_NAME}>)