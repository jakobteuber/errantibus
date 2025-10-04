/*
 * Errantibus, by Jakob Teuber
 * See <https://github.com/jakobteuber/errantibus>
 */

#include "mode/errantibusDebug.hpp"
#include "mode/errantibusNoDebug.hpp"

#include <boost/stacktrace/stacktrace.hpp>

#include <cstddef>
#include <cstdint>
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace errantibus::internal {

namespace {

constexpr std::array specialChars = {
    "\\0", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "\\a", "\\b", "\\t", "\\n",
    "\\v", "\\f", "\\r", "SO",  "SI",  "DLE", "DC1", "DC2", "DC3", "DC4", "NAK",
    "SYN", "ETB", "CAN", "EM",  "SUB", "ESC", "FS",  "GS",  "RS",  "US"};

constexpr unsigned del = 0x7f;

void writeChar(std::ostringstream &out, unsigned u, int i, bool isSigned) {
  if (u < specialChars.size()) {
    out << '`' << specialChars[u] << '`';
  } else if (u == del) {
    out << "`DEL`";
  } else if (u < del) {
    out << '`' << static_cast<char>(u) << '`';
  }

  if (isSigned) {
    out << " numeric: " << i << " (signed)";
  } else {
    out << " numeric: " << u << " (unsigned)";
  }
}

} // namespace

void stringify(std::ostringstream &out, char obj) {
  writeChar(out, static_cast<unsigned char>(obj), static_cast<int>(obj),
            std::numeric_limits<char>::is_signed);
}

void stringify(std::ostringstream &out, unsigned char obj) {
  writeChar(out, obj, static_cast<int>(obj), false);
}

void stringify(std::ostringstream &out, signed char obj) {
  writeChar(out, static_cast<unsigned char>(obj), static_cast<int>(obj), true);
}

namespace {

constexpr std::string_view yellow = "\033[33m";
constexpr std::string_view blue = "\033[34m";
constexpr std::string_view red = "\033[31m";
constexpr std::string_view bold = "\033[1m";
constexpr std::string_view reset = "\033[0m";

auto loadSourceContext(const std::string &filename, std::int64_t lineNo,
                       std::int64_t before, std::int64_t after)
    -> std::vector<std::string> {
  auto lines = std::vector<std::string>();
  lines.reserve(before + 1 + after);
  auto file = std::ifstream{std::string{filename}};
  if (!file) {
    return {};
  }

  std::int64_t start = std::max(static_cast<std::int64_t>(1), lineNo - before);
  std::int64_t end = lineNo + after;
  for (std::int64_t l = 1; l < start; ++l) {
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  for (std::int64_t l = start; l < end; ++l) {
    if (!file) {
      break;
    }
    auto line = std::string();
    std::getline(file, line);
    lines.push_back(line);
  }
  return lines;
}

auto printSourceContext(const std::string &filename, std::size_t lineNo) {
  std::int64_t before = 2;
  std::int64_t after = 2;
  auto loc = static_cast<std::int64_t>(lineNo);
  auto context = loadSourceContext(filename, loc, before, after);
  for (std::int64_t i = 0; i < std::ssize(context); ++i) {
    std::int64_t l = loc - before + i;
    std::int64_t padding = 10 - std::ssize(toString(l));
    for (int k = 0; k < padding; ++k) {
      std::cerr << ' ';
    }
    if (i == before) {
      std::cerr << blue;
      std::cerr << "> " << l << " |\t";
    } else {
      std::cerr << "  " << l << " |\t";
    }
    std::cerr << context[i];
    if (i == before) {
      std::cerr << reset;
    }
    std::cerr << '\n';
  }
}

void printStackTrace() {
  std::cerr << '\n';
  std::cerr << yellow << bold << "Stacktrace (most recent call last):" << reset
            << '\n';
  auto trace = boost::stacktrace::stacktrace();
  int size = static_cast<int>(trace.size());
  int skipBottom = 3;
  int skipTop = 2;
  for (int i = size - skipBottom - 1; i >= skipTop; --i) {
    auto frame = trace[i];
    if (!frame) {
      continue;
    }
    std::cerr << yellow << " #" << i << " " << frame.name() << reset << "\n";
    std::cerr << '\t' << "at " << frame.source_file() << ':'
              << frame.source_line() << " at " << frame.address() << '\n';
    printSourceContext(frame.source_file(), frame.source_line());
  }
  std::cerr << '\n';
}

[[noreturn]] void terminate() {
  std::cerr << '\n';
  std::terminate();
}

auto trim(std::string_view sv) -> std::string_view {
  while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.front()))) {
    sv.remove_prefix(1);
  }
  while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.back()))) {
    sv.remove_suffix(1);
  }
  return sv;
}

auto splitExpressions(std::string_view input) -> std::vector<std::string_view> {
  auto result = std::vector<std::string_view>();
  std::size_t start = 0;
  std::size_t parenDepth = 0;

  for (size_t i = 0; i < input.size(); ++i) {
    char c = input[i];

    if (c == '(') {
      ++parenDepth;
    } else if (c == ')' && parenDepth > 0) {
      --parenDepth;
    } else if (c == ',' && parenDepth == 0) {
      std::string_view part = input.substr(start, i - start);
      result.push_back(trim(part));
      start = i + 1;
    }
  }

  if (start < input.size()) {
    result.push_back(trim(input.substr(start)));
  }
  return result;
}

void printHeader(std::string_view file, std::size_t line,
                 std::string_view msg) {
  std::cerr << red << bold << file << ":" << line;
  if (!msg.empty()) {
    std::cerr << " - " << msg;
  }
  std::cerr << reset << "\n";
}

void printValues(std::string_view expressions,
                 const std::vector<std::string> &values) {
  auto exprs = splitExpressions(expressions);
  for (std::size_t i = 0; i < values.size(); ++i) {
    std::cerr << "\t(" << i << ") " << exprs[i] << " = " << values[i] << "\n";
  }
}

void printArguments(std::string_view msg, std::string_view firstExpr,
                    std::string_view firstValue, std::string_view secondExpr,
                    std::string_view secondValue) {
  std::cerr << "   " << msg << '\n';
  std::cerr << "   Left value:  " << firstExpr << '\n';
  std::cerr << "           is:  " << firstValue << '\n';
  std::cerr << "   Right value: " << secondExpr << '\n';
  std::cerr << "            is: " << secondValue << '\n';
}

} // namespace

void printDebug(std::string_view file, std::size_t line,
                std::string_view expressions,
                const std::vector<std::string> &values) {
  printHeader(file, line, "");
  printValues(expressions, values);
}

[[noreturn]] void fail(std::string_view message, std::string_view file,
                       std::size_t line, std::string_view expressions,
                       const std::vector<std::string> &values) {
  printStackTrace();
  printHeader(file, line, message);
  printValues(expressions, values);
  terminate();
}

[[noreturn]] void failAsset(std::string_view message,
                            std::string_view condition, std::string_view file,
                            std::size_t line, std::string_view expressions,
                            const std::vector<std::string> &values) {
  printStackTrace();
  printHeader(file, line, message);
  std::cerr << "Expected true, but was false: " << condition << '\n';
  printValues(expressions, values);
  terminate();
}

[[noreturn]] void failEq(std::string_view message, std::string_view firstExpr,
                         std::string_view firstValue,
                         std::string_view secondExpr,
                         std::string_view secondValue, std::string_view file,
                         std::size_t line, std::string_view expressions,
                         const std::vector<std::string> &values) {
  printStackTrace();
  printHeader(file, line, message);
  printArguments("Should be equal, but was different:", firstExpr, firstValue,
                 secondExpr, secondValue);
  printValues(expressions, values);
  terminate();
}

[[noreturn]] void failNeq(std::string_view message, std::string_view firstExpr,
                          std::string_view firstValue,
                          std::string_view secondExpr,
                          std::string_view secondValue, std::string_view file,
                          std::size_t line, std::string_view expressions,
                          const std::vector<std::string> &values) {
  printStackTrace();
  printHeader(file, line, message);
  printArguments("Should be different, but was equal:", firstExpr, firstValue,
                 secondExpr, secondValue);
  printValues(expressions, values);
  terminate();
}

[[noreturn]] void failNote(const char *message, const char *file,
                           unsigned line) {
  printHeader(file, line, message);
  std::cerr << "Compilerd without debug assertions. Terminating...";
  terminate();
}

} // namespace errantibus::internal
