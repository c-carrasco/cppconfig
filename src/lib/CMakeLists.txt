file (GLOB CXX_FILES FILES *.cxx)

add_library (cppconfig STATIC ${CXX_FILES})

target_include_directories (cppconfig
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
)