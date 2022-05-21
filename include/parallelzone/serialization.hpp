#pragma once
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/details/traits.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/atomic.hpp>
#include <cereal/types/bitset.hpp>
#include <cereal/types/chrono.hpp>
#include <cereal/types/deque.hpp>
#include <cereal/types/forward_list.hpp>
#include <cereal/types/functional.hpp>
#include <cereal/types/list.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/queue.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/stack.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/valarray.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/vector.hpp>
#include <madness/world/archive.h>
#include <madness/world/buffer_archive.h>
#include <madness/world/cereal_archive.h>
#include <madness/world/type_traits.h>

namespace parallelzone {
using XMLOutputArchive =
  madness::archive::CerealOutputArchive<cereal::XMLOutputArchive>;
using XMLInputArchive =
  madness::archive::CerealInputArchive<cereal::XMLInputArchive>;
using JSONOutputArchive =
  madness::archive::CerealOutputArchive<cereal::JSONOutputArchive>;
using JSONInputArchive =
  madness::archive::CerealInputArchive<cereal::JSONInputArchive>;
using BinaryOutputArchive =
  madness::archive::CerealOutputArchive<cereal::BinaryOutputArchive>;
using BinaryInputArchive =
  madness::archive::CerealInputArchive<cereal::BinaryInputArchive>;
using PortableBinaryOutputArchive =
  madness::archive::CerealOutputArchive<cereal::PortableBinaryOutputArchive>;
using PortableBinaryInputArchive =
  madness::archive::CerealInputArchive<cereal::PortableBinaryInputArchive>;
template<typename T, typename Archive>
using is_serializable = cereal::traits::is_output_serializable<T, Archive>;
template<typename T, typename Archive>
using is_deserializable = cereal::traits::is_input_serializable<T, Archive>;
using cereal::traits::detail::get_input_from_output;
using cereal::traits::detail::get_output_from_input;
using madness::is_input_archive;
using madness::is_input_archive_v;
using madness::is_output_archive;
using madness::is_output_archive_v;
} // namespace parallelzone

CEREAL_SETUP_ARCHIVE_TRAITS(madness::archive::BufferInputArchive,
                            madness::archive::BufferOutputArchive)

CEREAL_SETUP_ARCHIVE_TRAITS(parallelzone::XMLInputArchive,
                            parallelzone::XMLOutputArchive)
CEREAL_SETUP_ARCHIVE_TRAITS(parallelzone::JSONInputArchive,
                            parallelzone::JSONOutputArchive)
CEREAL_SETUP_ARCHIVE_TRAITS(parallelzone::BinaryInputArchive,
                            parallelzone::BinaryOutputArchive)
CEREAL_SETUP_ARCHIVE_TRAITS(parallelzone::PortableBinaryInputArchive,
                            parallelzone::PortableBinaryOutputArchive)
