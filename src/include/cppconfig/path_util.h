// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023-2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#ifndef __CPP_CONFIG_MEM_MAP_FILE_H__
#define __CPP_CONFIG_MEM_MAP_FILE_H__
#include <filesystem>


namespace cppconfig::util {

class PathUtil {
  public:
    //! @brief Retrieves the path of the executable file of the current process.
    [[nodiscard]] static std::filesystem::path getModuleFileName();

    //! @brief Retrieves the path to the folder containing the executable file of the current process.
    [[nodiscard]] inline static std::filesystem::path getProgramDirPath() { return getModuleFileName().parent_path(); }
};

}

#endif
