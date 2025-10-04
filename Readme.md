*Errantibus* — For those who make errors
========================================

*Errantibus* is an assertion library for C++, with better 
error messages than a plain `assert`.

# Usage 

```cpp  
#include <errantibus.hpp>

//... 

assertDbg(index < items.size(), "index out of bounds", items);
```

The following macros are provided:

| Debug | Always |
| ----- | ------ | 
| `assertDbg(condition, message, ...)` | `assertAlways(condition, message, ...)` |
| `assertDbgEq(a, b, message, ...)` | `assertAlwaysEq(a, b, message, ...)` |
| `assertDbgNeq(a, b, message, ...)` | `assertAlwaysNeq(a, b, message, ...)` |
| `failDbg(message, ...)` | `failAlways(message, ...)` | 
| `debug(...)` | — |

For Debug builds, *Errantibus* will try to provide as much information 
as possible: It displays a stacktrace (via Boost Stacktrace) and attempts
to output all the additional information provided to the macro. For the best 
results, enable debug options with `-g`.

If you want to build for release, you can define `ERRANTIBUS_NODEBUG`. If it is
defined, the debug versions won't create asserts. Instead, it will inform the 
compiler that the asserted condition will always hold, so it can optimized based 
on, e.g. the unreachability of a `failDbg`. The assertions that are always active
will still be active, if they fail the program will be terminated with a minimal
notice.

# Setup

With CMake, you can use Errantibus via FetchContent:

```cmake
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -DERRANTIBUS_NODEBUG")

include(FetchContent)
FetchContent_Declare(
    Errantibus
    GIT_REPOSITORY https://github.com/jakobteuber/errantibus.git
    GIT_TAG v0.1.0
)
FetchContent_MakeAvailable(Errantibus)

target_link_libraries(MyApp PRIVATE Errantibus)
```
```
```

# License

Copyright 2025 Jakob Teuber

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
