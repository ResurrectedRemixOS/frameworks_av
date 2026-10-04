// libcsextimpl.so was linked against fmt 11 and imports nine fmt::v11 symbols.
// The in-tree fmtlib is v12, and libbase exports only the five that survived
// --gc-sections, so dlopen fails. Rebuild fmtlib's compiled TU under the v11
// inline namespace to satisfy the blob.
#define FMT_BEGIN_NAMESPACE \
    namespace fmt {         \
    inline namespace v11 {
#define FMT_END_NAMESPACE \
    }                     \
    }

#include "src/format.cc"

// fmt 12 also hoisted locale_ref and assert_fail out of fmt::detail, so four of
// the blob's imports still mangle differently than the rebuilt TU emits.
// Re-emit those four under the fmt 11 names. locale_ref is a single type-erased
// pointer in both versions, so it passes as a plain pointer.
namespace fmt {
inline namespace v11 {
namespace detail {
namespace compat11 {

static_assert(sizeof(::fmt::v11::locale_ref) == sizeof(const void*),
              "locale_ref is no longer a single pointer");

static inline auto as_ref(const void* p) -> ::fmt::v11::locale_ref {
  ::fmt::v11::locale_ref r;
  __builtin_memcpy(&r, &p, sizeof p);
  return r;
}

FMT_NORETURN void assert_fail(const char* file, int line, const char* message)
    asm("_ZN3fmt3v116detail11assert_failEPKciS3_");
FMT_NORETURN void assert_fail(const char* file, int line, const char* message) {
  ::fmt::v11::assert_fail(file, line, message);
}

auto decimal_point(const void* loc) -> char
    asm("_ZN3fmt3v116detail18decimal_point_implIcEET_NS1_10locale_refE");
auto decimal_point(const void* loc) -> char {
  return ::fmt::v11::detail::decimal_point_impl<char>(as_ref(loc));
}

auto thousands_sep(const void* loc) -> thousands_sep_result<char>
    asm("_ZN3fmt3v116detail18thousands_sep_implIcEENS1_20thousands_sep_resultIT_EENS1_"
        "10locale_refE");
auto thousands_sep(const void* loc) -> thousands_sep_result<char> {
  return ::fmt::v11::detail::thousands_sep_impl<char>(as_ref(loc));
}

auto write_loc_compat(basic_appender<char> out, loc_value value,
                      const format_specs& specs, const void* loc) -> bool
    asm("_ZN3fmt3v116detail9write_locENS0_14basic_appenderIcEENS0_9loc_valueERKNS0_"
        "12format_specsENS1_10locale_refE");
auto write_loc_compat(basic_appender<char> out, loc_value value,
                      const format_specs& specs, const void* loc) -> bool {
  return ::fmt::v11::detail::write_loc(out, value, specs, as_ref(loc));
}

}  // namespace compat11
}  // namespace detail
}  // namespace v11
}  // namespace fmt
