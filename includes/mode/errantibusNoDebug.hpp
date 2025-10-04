/*
 * Errantibus, by Jakob Teuber
 * See <https://github.com/jakobteuber/errantibus>
 */

#ifndef ERRANTIBUS_DEFINITIONS_HPP
#define ERRANTIBUS_DEFINITIONS_HPP

namespace errantibus::internal {

/**
 * @brief Marks an unreachable program location. In your own program
 * prefer std::unreachable from <utility>.
 */
[[noreturn]] inline void unrearchable() {
  // Uses compiler specific extensions if possible.
  // Even if no extension is used, undefined behavior is still raised by
  // an empty function body and the noreturn attribute.
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
  __assume(false);
#else // GCC, Clang
  __builtin_unreachable();
#endif
}

/**
 * @brief Provide a minimal failure notice and terminate the program.
 */
[[noreturn]] void failNote(const char *message, const char *file,
                           unsigned line);

#define assertAlways(cond, msg, ...)                                           \
  do {                                                                         \
    bool condition = (cond);                                                   \
    if (!condition) [[unlikely]] {                                             \
      errantibus::internal::failNote(msg, __FILE__, __LINE__);                 \
    }                                                                          \
  } while (false)

#define assertDbg(cond, msg, ...)                                              \
  do {                                                                         \
    bool condition = (cond);                                                   \
    if (!condition) {                                                          \
      errantibus::internal::unreachable();                                     \
    }                                                                          \
  } while (false)

#define assertAlwaysEq(a, b, msg, ...)                                         \
  do {                                                                         \
    auto aObj = (a);                                                           \
    auto bObj = (b);                                                           \
    bool condition = aObj == bObj;                                             \
    if (!condition) [[unlikely]] {                                             \
      errantibus::internal::failNote(msg, __FILE__, __LINE__);                 \
    }                                                                          \
  } while (false)

#define assertDbgEq(a, b, msg, ...)                                            \
  do {                                                                         \
    bool condition = (a) == (b);                                               \
    if (!condition) {                                                          \
      errantibus::internal::unreachable();                                     \
    }                                                                          \
  } while (false)

#define assertAlwaysNeq(a, b, msg, ...)                                        \
  do {                                                                         \
    auto aObj = (a);                                                           \
    auto bObj = (b);                                                           \
    bool condition = aObj != bObj;                                             \
    if (!condition) [[unlikely]] {                                             \
      errantibus::internal::failNote(msg, __FILE__, __LINE__);                 \
    }                                                                          \
  } while (false)

#define assertDbgNeq(a, b, msg, ...)                                           \
  do {                                                                         \
    bool condition = (a) != (b);                                               \
    if (!condition) {                                                          \
      errantibus::internal::unreachable();                                     \
    }                                                                          \
  } while (false)

#define failAlways(msg, ...)                                                   \
  do {                                                                         \
    errantibus::internal::failNote(msg, __FILE__, __LINE__);                   \
  } while (false)

#define failDbg(msg, ...)                                                      \
  do {                                                                         \
    errantibus::internal::unreachable();                                       \
  } while (false)

#define debug(...)                                                             \
  do {                                                                         \
  } while (false)

} // namespace errantibus::internal

#endif
