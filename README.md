# JSORON

**JSORON** is a JSON parser and object model written in C++ as part of the **"Performance-Aware Programming"** course by Casey Muratori.

## Features

- **Single-pass parser** — no separate lexer/tokenizer; parses directly into the object tree.
- **Implicit type casting** — `JSONValue` supports cast operators to `bool&`, `int&`, `double&`, `std::string_view`, `std::string&`, `JSONObject&`, and `JSONArray&`.
- **Mutable string promotion** — accessing a parsed string as `std::string&` transparently copies it into an owned `std::string` for mutation.
- **Insertion-order iteration** — `operator<<` prints keys in the order they were parsed or inserted.
- Self-contained library — copy `include/` and `src/` into your project and build.
- Written entirely in C++20, no external dependencies.

## Directory Structure

```
JSORON/
├── include/        # JSONObject.h (API) + JSORONdef.h (integer typedefs)
├── src/            # JSONObject.cpp — implementation
├── test/           # Unit tests (depends on perfaware repo)
├── build/          # Compiled binaries (debug, release, profiled)
├── Makefile
└── README.md
```

## Usage

### Parsing from a string

```cpp
using namespace JSORON;

JSONObject obj;
obj.Parse(R"({"name": "JSORON", "version": 1.0, "active": true})");

std::string_view name = obj["name"];   // zero-copy view into source buffer
double version        = obj["version"];
bool active           = obj["active"];
```

### Parsing from a file

```cpp
using namespace JSORON;

std::ifstream file("data.json");
JSONObject obj;
int err = obj.Parse(file);  // 0 on success, 1 on error
```

### Building objects programmatically

```cpp
using namespace JSORON;

JSONObject obj;
obj["name"]    = "JSORON";   // stored as owned MUT_STR
obj["version"] = 1.0;
obj["count"]   = 42;

std::cout << obj << std::endl;
```

### Nested access and arrays

```cpp
using namespace JSORON;

JSONObject obj;
obj.Parse(R"({"points": [{"x": 1.0, "y": 2.0}, {"x": 3.0, "y": 4.0}]})");

JSONArray& pts = obj["points"];
double x0 = pts[0]["x"];  // 1.0

for (const auto& pt : pts) {
    double x = pt.At("x");
    double y = pt.At("y");
}
```

## Building

```bash
cd JSORON
make debug      # debug build (with -g3)
make release    # optimized build (-O3, NDEBUG)
make profiled   # build with profiling enabled
make            # all three
```

Binaries are placed in `build/`.

### Using JSORON as a library

The library is self-contained in `include/` and `src/`. Copy both directories into your project, add `include/` to your header search path, and compile `src/JSONObject.cpp` alongside your code.

**Note:** The test binary (`test/JSONObject_main.cpp`) and the Makefile still depend on the [perfaware](https://github.com/cmuratori/computer_enhance) repo via relative paths:

| Dependency | Path | Used by |
|---|---|---|
| `generic_test.h` / `.o` | `../utils/` | Test code |
| `profiler.h` / `.o` | `../profiler/` | Test binary (linked when built with `-DPROFILING`) |
