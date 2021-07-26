# cs3p01-proyecto

For C++ coding guidelines, see the
[C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines).

## Requirements

### Ubuntu 18.04 LTS

Install required packages.

> On Ubuntu 16.04 LTS, omit the `clang-tools` package, which is included in the `clang` package on Ubuntu 16.04 LTS.

```bash
$ sudo apt install gcc g++ clang clang-format cmake
```

## Build

Append `-DCMAKE_BUILD_TYPE=Release` or `-DCMAKE_BUILD_TYPE=Debug` to the `cmake` command
arguments to specify release or debug builds.

### GCC

```bash
$ cd build
$ cmake ..
$ make
```

### Treat warnings as errors

Use `-DWERROR=On` option to treat compile warnings as errors.

```bash
$ cd build
$ cmake -DWERROR=On ..
$ make
/home/user/GitHub/cs3p01-proyecto/src/danger/bad_examples.cpp:13:18: error: array index
      3 is past the end of the array (which contains 2 elements)
      [-Werror,-Warray-bounds]
    std::cout << a[3];
                 ^ ~
...
```

## Run

First, perform a build as described in the **Build** section, then run the following
commands in the `build` directory.

### Application

```bash
$ ./cs3p01-proyecto
```

### Unit tests

Unit tests are written using the [Catch2](https://github.com/catchorg/Catch2/) unit testing
framework.

```bash
$ ./unit_test
```

## Clang Tools

### Clang-Format

[Clang-Format](https://clang.llvm.org/docs/ClangFormat.html) is a tool that can automically
format your source code accordiing to a specific style guide, saving developers time. It is
configured using the [`.clang-format`](.clang-format) configuration file. Modify this file to
control how source files should be formatted.

To demonstrate `clang-format` in action, first modify a line from [`src/main.cpp`](src/main.cpp)

```cpp
    return EXIT_SUCCESS;
```

To

```cpp
    return           EXIT_SUCCESS;
```

Next, run `clang-format` on the project.

```bash
$ cd build
$ cmake ..
$ make clang-format
```

[`src/main.cpp`](src/main.cpp) will be reformatted properly to

```cpp
    return EXIT_SUCCESS;
```
