// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023-2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#ifndef __CPP_CONFIG_JSON_BUFFER_H__
#define __CPP_CONFIG_JSON_BUFFER_H__
#include <cstring>
#include <functional>
#include <memory>
#include <string>


namespace cppconfig::json {

/// @brief A class to manage a buffer of characters efficiently.
///
/// The Buffer class provides mechanisms to navigate and manipulate a sequence of characters. It supports operations such as
/// reading the next character, checking if the end of the data has been reached, matching a sequence of characters,
/// extracting a substring, counting characters based on a condition, moving forward in the buffer, and retrieving the current
/// position in terms of line and column.
class Buffer {
  public:
    /// @brief Constructs a buffer from a unique pointer to a character array and its length.
    /// @param d Unique pointer to the character data.
    /// @param len Length of the character data.
    Buffer (std::unique_ptr<char []> &&d, size_t len): _data { std::move (d) }, _ptr { _data.get() }, _size { len } {
      // empty
    }

    /// @brief Constructs a buffer from a raw pointer to a character array and its length.
    /// @param d Pointer to the character data.
    /// @param len Length of the character data.
    Buffer (const char *d, size_t len): _ptr { d }, _size { len } {
      // empty
    }

    /// @brief Returns the next character in the buffer and updates the current position.
    /// @return The next character.
    inline char next() {
      const auto c { _ptr[_idx++] };
      if (c == '\n') {
        ++_line;
        _col = 0;
      }
      else {
        ++_col;
      }

      return c;
    }

    /// @brief Checks if the end of the data has been reached.
    /// @return True if the end of data is reached, false otherwise.
    inline bool endOfData() const {
      return _idx >= _size;
    }

    /// @brief Checks if the next sequence of characters matches the given memory block.
    /// @param mem Pointer to the memory block to match.
    /// @param count Number of characters to match.
    /// @return True if the sequence matches, false otherwise.
    inline bool match (const char *mem, size_t count) const {
      if (_idx + count < _size)
        return std::memcmp (_ptr + _idx, mem, count) == 0;

      return false;
    }

    /// @brief Extracts a substring from the current position with the given length.
    /// @param len Length of the substring to extract.
    /// @return The extracted substring.
    inline std::string take (size_t len) const {
      if (_idx + len < _size)
        return std::string { _ptr + _idx, len };

      return std::string {};
    }

    /// @brief Counts the number of characters satisfying a given condition from the current position.
    /// @param cb A callback function that takes a character as input and returns a boolean.
    /// @return The number of characters meeting the condition.
    inline  size_t count (const std::function<bool (char)> &cb) const {
      size_t counter { 0 };
      for (size_t i = _idx; i < _size; ++i) {
        if (!cb (_ptr[i]))
          break;

        ++counter;
      }

      return counter;
    }

    /// @brief Advances the current position by a given length.
    /// @param len Number of characters to move forward.
    inline void forward (size_t len) {
      _col += len;
      _idx += len;
    }

    /// @brief Retrieves the current line number.
    /// @return The current line number.
    inline size_t line() const { return _line; }

    /// @brief Retrieves the current column number.
    /// @return The current column number.
    inline size_t column() const { return _col; }

    /// @brief Gets a pointer to the current character.
    /// @return Pointer to the current character.
    inline const char * current() const { return _ptr + _idx; }

    /// @brief Gets a pointer to the end of the character data.
    /// @return Pointer to the end of the data.
    inline const char * end() const { return _ptr + _size; }

  private:
    std::unique_ptr<char []> _data; // Unique pointer to the character data.
    const char *_ptr { nullptr };  // Raw pointer to the current position in the character data.
    size_t _size { 0 }; // Size of the character data.
    size_t _idx { 0 }; // Current index in the character data.
    size_t _line { 0 }; // Current line number in the buffer.
    size_t _col { 0 }; // Current column number in the buffer.
};

}

#endif
