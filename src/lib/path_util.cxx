// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <limits.h>
#include <unistd.h>

#ifdef __APPLE__
  #include <mach-o/dyld.h>
#endif

#include <cppconfig/path_util.h>


namespace cppconfig::util {

// ----------------------------------------------------------------------------
// PathUtil::getModuleFileName
// ----------------------------------------------------------------------------
std::filesystem::path PathUtil::getModuleFileName() {
#ifdef WINDOWS
  #error "Not implemented"
#elif __APPLE__
  char result[PATH_MAX + 1] = { 0, };

  uint32_t size = PATH_MAX;
  _NSGetExecutablePath(result, &size);

  return result;
#else
  char result[PATH_MAX + 1] = { 0, };

  const ssize_t count { readlink ("/proc/self/exe", result, PATH_MAX) };

  return std::string { result, (count > 0) ? static_cast<std::string::size_type> (count) : 0 };
#endif
}

}
