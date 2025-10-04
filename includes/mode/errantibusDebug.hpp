/*
 * Errantibus, by Jakob Teuber
 * See <https://github.com/jakobteuber/errantibus>
 */

#ifndef ERRANTIBUS_DEFINITIONS_HPP
#define ERRANTIBUS_DEFINITIONS_HPP

#include <cstddef>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace errantibus::internal {

template <typename T>
concept StreamPrintable =
    requires(T obj, std::ostringstream &out) { out << obj; };

template <typename T>
concept Iterable = requires(T iterable) {
  std::cbegin(iterable);
  std::cend(iterable);
} && !StreamPrintable<T>;

void stringify(std::ostringstream &out, const auto &obj);

void stringify(std::ostringstream &out, char obj);
void stringify(std::ostringstream &out, unsigned char obj);
void stringify(std::ostringstream &out, signed char obj);
void stringify(std::ostringstream &out, bool obj);

template <StreamPrintable T>
inline void stringify(std::ostringstream &out, const T &obj) {
  out << '`' << obj << "`";
}

template <Iterable T>
inline void stringify(std::ostringstream &out, const T &obj) {
  auto contents = std::ostringstream();
  bool first = true;
  for (const auto &x : obj) {
    if (first) {
      first = false;
    } else {
      contents << ", ";
    }
    stringify(contents, x);
  }
  out << '{' << contents.str() << '}';
}

inline void stringify(std::ostringstream &out,
                      [[maybe_unused]] const auto &obj) {
  out << "not printable\n";
}

inline auto toString(const auto &obj) -> std::string {
  auto out = std::ostringstream();
  stringify(out, obj);
  return out.str();
}

inline void report([[maybe_unused]] const std::vector<std::string> &strings) {}

template <typename... Args>
void report(std::vector<std::string> &strings, auto first, Args... arguments) {
  strings.push_back(toString(first));
  report(strings, arguments...);
}

template <typename... Args>
auto generateReport(Args... args) -> std::vector<std::string> {
  auto strings = std::vector<std::string>();
  strings.reserve(sizeof...(args));
  report(strings, args...);
  return strings;
}

void printDebug(std::string_view file, std::size_t line,
                std::string_view expressions,
                const std::vector<std::string> &values);
[[noreturn]] void fail(std::string_view message, std::string_view file,
                       std::size_t line, std::string_view expressions,
                       const std::vector<std::string> &values);
[[noreturn]] void failAssert(std::string_view message,
                             std::string_view condition, std::string_view file,
                             std::size_t line, std::string_view expressions,
                             const std::vector<std::string> &values);
[[noreturn]] void failEq(std::string_view message, std::string_view firstExpr,
                         std::string_view firstValue,
                         std::string_view secondExpr,
                         std::string_view secondValue, std::string_view file,
                         std::size_t line, std::string_view expressions,
                         const std::vector<std::string> &values);
[[noreturn]] void failNeq(std::string_view message, std::string_view firstExpr,
                          std::string_view firstValue,
                          std::string_view secondExpr,
                          std::string_view secondValue, std::string_view file,
                          std::size_t line, std::string_view expressions,
                          const std::vector<std::string> &values);

#define assertAlways(cond, msg, ...)                                           \
  do {                                                                         \
    bool condition = (cond);                                                   \
    if (!condition) [[unlikely]] {                                             \
      errantibus::internal::failAssert(                                        \
          msg, #cond, __FILE__, __LINE__, #__VA_ARGS__,                        \
          errantibus::internal::generateReport(__VA_ARGS__));                  \
    }                                                                          \
  } while (false)

#define assertDbg(cond, msg, ...) assertAlways(cond, msg, __VA_ARGS__)

#define assertAlwaysEq(a, b, msg, ...)                                         \
  do {                                                                         \
    auto aObj = (a);                                                           \
    auto bObj = (b);                                                           \
    bool condition = aObj == bObj;                                             \
    if (!condition) [[unlikely]] {                                             \
      errantibus::internal::failEq(                                            \
          msg, #a, errantibus::internal::toString(aObj), #b,                   \
          errantibus::internal::toString(bObj), __FILE__, __LINE__,            \
          #__VA_ARGS__, errantibus::internal::generateReport(__VA_ARGS__));    \
    }                                                                          \
  } while (false)

#define assertDbgEq(a, b, msg, ...) assertAlwaysEq(a, b, msg, __VA_ARGS__)

#define assertAlwaysNeq(a, b, msg, ...)                                        \
  do {                                                                         \
    auto aObj = (a);                                                           \
    auto bObj = (b);                                                           \
    bool condition = aObj != bObj;                                             \
    if (!condition) [[unlikely]] {                                             \
      errantibus::internal::failNeq(                                           \
          msg, #a, errantibus::internal::toString(aObj), #b,                   \
          errantibus::internal::toString(bObj), __FILE__, __LINE__,            \
          #__VA_ARGS__, errantibus::internal::generateReport(__VA_ARGS__));    \
    }                                                                          \
  } while (false)

#define assertDbgNeq(a, b, msg, ...) assertAlwaysNeq(a, b, msg, __VA_ARGS__)

#define failAlways(msg, ...)                                                   \
  do {                                                                         \
    errantibus::internal::fail(                                                \
        msg, __FILE__, __LINE__, #__VA_ARGS__,                                 \
        errantibus::internal::generateReport(__VA_ARGS__));                    \
  } while (false)

#define failDbg(msg, ...) failAlways(msg, __VA_ARGS__)

#define debug(...)                                                             \
  do {                                                                         \
    errantibus::internal::printDebug(                                          \
        __FILE__, __LINE__, #__VA_ARGS__,                                      \
        errantibus::internal::generateReport(__VA_ARGS__));                    \
  } while (false)

} // namespace errantibus::internal

#endif
