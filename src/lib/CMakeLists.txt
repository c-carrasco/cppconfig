# file (GLOB CXX_FILES FILES *.cxx)

# add_library (cppconfig STATIC ${CXX_FILES})

# target_include_directories (cppconfig
#   PUBLIC
#   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
# )

add_library (cppconfig INTERFACE)

target_include_directories (cppconfig INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  $<INSTALL_INTERFACE:include>
)

# target_link_libraries (cppconfig INTERFACE fmt::fmt)