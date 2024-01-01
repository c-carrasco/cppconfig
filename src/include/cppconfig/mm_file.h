// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023-2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#ifndef __CPP_CONFIG_MEM_MAP_FILE_H__
#define __CPP_CONFIG_MEM_MAP_FILE_H__
#include <sys/mman.h>
#include <sys/stat.h>
#include <type_traits>
#include <fcntl.h>
#include <unistd.h>
#include <cinttypes>
#include <string>


namespace cppconfig::util {

/// @brief Represents a memory-mapped file with optional madvise behavior.
/// @tparam T The type of data stored in the memory-mapped file (default is char).
/// @tparam MADVISE The madvise behavior for the memory-mapped file (default is POSIX_MADV_SEQUENTIAL).
template<typename T=char, int32_t MADVISE=POSIX_MADV_SEQUENTIAL>
class MMapFile {
  public:
    /// @brief Default constructor for MMapFile.
    MMapFile() = default;

    /// @brief Destructor for MMapFile. Closes the memory-mapped file.
    ~MMapFile() {
      close();
    }

    /// @brief Deleted copy constructor to prevent unintended copying.
    MMapFile (const MMapFile &) = delete;

    /// @brief @brief Deleted copy assignment operator to prevent unintended copying.
    MMapFile & operator= (const MMapFile &) = delete;

    /// @brief Opens a memory-mapped file at the specified path in read-only mode.
    /// @param path The path to the memory-mapped file.
    /// @return True if the file is successfully opened, false otherwise.
    bool open (const std::filesystem::path &path) {
      if (isOpen())
        return false;

      _fd = ::open(path.c_str(), O_RDONLY);
      if (_fd == -1)
        return false;

      struct stat fs;
      if (fstat (_fd, &fs) == -1)
        return false;

      _size = fs.st_size;

      void *data { static_cast<T *> (mmap (nullptr, _size, PROT_READ, MAP_SHARED, _fd, 0)) };
      _data = static_cast<T *> (data);

      posix_madvise (data, _size, MADVISE);

      return true;
    }

    /// @brief Checks if the memory-mapped file is open.
    /// @return True if the file is open, false otherwise.
    inline bool isOpen() const noexcept { return _fd != -1; }

    /// @brief Closes the memory-mapped file.
    /// @return True if the file is successfully closed, false otherwise.
    inline bool close() {
      if (isOpen()) {
        if (munmap(static_cast<char *> (_data), _size) != -1) {
          ::close (_fd);
          _fd = -1;
          _size = 0;
          _data = nullptr;

          return true;
        }
      }

      return false;
    }

    /// @brief Gets the size of the memory-mapped file in bytes.
    /// @return The size of the memory-mapped file.
    inline size_t bytes() const {
      return _size;
    }

    /// @brief Gets a pointer to the data in the memory-mapped file.
    /// @return A pointer to the data.
    inline T * data() const {
      return _data;
    }

  private:
    int32_t _fd { -1 };   ///< File descriptor for the memory-mapped file.
    size_t _size { 0 };   ///< Size of the memory-mapped file in bytes.
    T *_data { nullptr }; ///< Pointer to the data in the memory-mapped file.
};

}

#endif
