#pragma once
/** @file hasher.hpp
 *
 * This header provides the hashing API.
 * Currently, it is based on BPHash library.
 *
 */
#include <bphash/Hasher.hpp>
#include <bphash/types/All.hpp>
#include <functional>
#include <optional>
#include <typeindex>
#include <typeinfo>
namespace parallelzone {

using bphash::hash_to_string;
using bphash::Hasher;
using bphash::HashType;
using bphash::HashValue;
using bphash::make_hash;
using hash_type = std::string;

inline auto make_hasher() { return Hasher(HashType::Hash128); }

/** @brief Generates a hash string in a single call
 *
 * This can be used to easily obtain the hash string for several
 * objects at once without creating a Hasher object manually.
 *
 * @tparam Args variadic template for types of input objects
 * @param[in] args Objects to hash
 * @returns Hash string of the given object(s)
 */
template<typename... Args>
auto hash_objects(Args&&... args) {
    auto h = make_hasher();
    h(std::forward<Args>(args)...);
    return bphash::hash_to_string(h.finalize());
}

/** @brief Is type @p T the same as `hash_type`?
 *
 *  This is a compile-time constant indicating whether type @p T is the same
 *  type as `hash_type` (it's set to true if @p T is the same type and false
 *  otherwise). It is used primarily for TMP.
 *
 *  @tparam T The type we are comparing to `hash_type`
 */
template<typename T>
static constexpr bool is_hash_v = std::is_same_v<T, hash_type>;

/** @brief Disables an overload if @p T is the same as `hash_type`.
 *
 *  This type exploits SFINAE to disable templated overloads of functions
 *  when the user passes the hash instead of an object that needs hashing.
 */
template<typename T>
using disable_if_hash_t = std::enable_if_t<!is_hash_v<T>>;

} // namespace parallelzone

namespace std {
// Free function to make std::reference_wrapper hashable
template<typename T>
void hash_object(const reference_wrapper<T>& ref, bphash::Hasher& h) {
    h(ref.get());
}

// Free function to make std::optional hashable
template<typename T>
void hash_object(const optional<T>& opt, bphash::Hasher& h) {
    if(opt.has_value()) h(opt.value());
}

// Free function to make std::type_info hashable
// Note that the hash generated may differ for different compilers
inline void hash_object(const type_info& t, bphash::Hasher& h) { h(t.name()); }

// Free function to make std::type_index hashable
// Note that the hash generated may differ for different compilers
inline void hash_object(const type_index t, bphash::Hasher& h) { h(t.name()); }
} // namespace std
