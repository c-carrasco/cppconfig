// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023 Carlos Carrasco
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

template<typename T=char, int32_t MADVISE=POSIX_MADV_SEQUENTIAL>
class MMapFile {
  public:
    MMapFile() = default;

    ~MMapFile() {
      close();
    }

    MMapFile (const MMapFile &) = delete;
    MMapFile & operator= (const MMapFile &) = delete;

    static inline MMapFile create (const std::filesystem::path &path) {
      MMapFile mmFile {};
      mmFile.open (path);
      return mmFile;
    }

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

    inline bool isOpen() const noexcept { return _fd != -1; }

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

    inline size_t bytes() const {
      return _size;
    }

    inline T * data() const {
      return _data;
    }

  private:
    int32_t _fd { -1 };
    size_t _size { 0 };
    T *_data { nullptr };
};

}

#endif
