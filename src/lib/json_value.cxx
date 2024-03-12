// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023-2024 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <cppconfig/json_value.h>


namespace cppconfig::json {

// ----------------------------------------------------------------------------
// Config::merge
//
// This function recursively merges the contents of the source JSON value (@p src) into
// the destination JSON value (@p dst). The merge operation is performed based on the
// following rules:
// - If the types of source and destination are different, the merge operation fails, and
//   false is returned.
// - If both source and destination are objects, the function iterates through the key-value
//   pairs in the source object. For each key, if the key is not present in the destination
//   object, the key-value pair is inserted. If the key is already present, the function
//   recursively merges the corresponding values.
// - If both source and destination are arrays, the function appends each element of the
//   source array to the destination array.
// - If neither of the above cases applies, the destination is updated to match the source.
// ----------------------------------------------------------------------------
bool JsonValue::merge (const json::JsonValue &src, json::JsonValue &dst) {
  if (!src.isNull() && !dst.isNull() && (src.type() != dst.type()))
    return false;

  if (src.isObject()) {
    for (auto itSrc = src.asObject().begin(); itSrc != src.asObject().end(); ++itSrc) {
      const auto &key { itSrc->first };

      const auto itDst { dst.asObject().find (key) };
      if (itDst == dst.asObject().end()) { // source item is NOT present in destination -> insert
        if (!dst.asObject().emplace (key, itSrc->second).second)
          return false; // could not be inserted
      }
      else { // source item is present in destination -> update
        if (!merge (itSrc->second, itDst->second))
          return false;
      }
    }
  }
  else if (src.isArray()) {
    for (const auto &item: src.asArray())
      dst.asArray().push_back (item);
  }
  else {
    dst = src;
  }

  return true;
}

}