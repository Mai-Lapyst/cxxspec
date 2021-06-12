# cxxspec

cxxspec is an BDD/TDD framework that's like ruby's [rspec](https://rspec.info/), and heavily inspired by it.

compatible with:
- `c++14`
- `c++17`

## License

This project is licensed under the terms of the AGPL v3.0 .
For license details please see the file `LICENSE`.

## Todo / Roadmap

- implement more builtin matchers
    - support contains_all, contains_any and contains_none
    - has_key & has_value
    - ...
- enhance the system that allows us to access protected members of other classes (`ALLOW_SPEC`)
- implement `before`, `after` and `around` hooks
- formatters
    - xml
    - junit
    - yaml
    - ...
- default parsing of cli options
    - allow specifing more formatter options
- execution
    - honor more the order of definition (we currently first run all sub-groups and then the examples of a group)
    - allow 'random' execution of examples (with seed)

## Building

cxxspec can only work relaible either directly compiled into the specs of the project or via dynamic library.
This is because in order to "autoregister" all specs, we use `__attribute__((constructor))` which executes the function it is annoted to
before `main()`.

### xmake

To build using [xmake](https://xmake.io/) just do `xmake`.
To build and run the cxxspec specs, run `xmake -b specs && xmake run specs`.
To install cxxspec in your system, run `xmake i` (after you builded it ofcourse).
To uninstall cxxspec from your system, run `xmake uninstall`.

## Writing specs

### The boilerplate
To begin writing specs we need a tiny bit of boilerplate code, but this needs only be written once (in your main spec file):
```c++
#include "cxxsepc/cxxspec.hpp"              // includes the core classes and DSL (macros) of cxxspec

int main(int argc, char** argv) {
    cxxspec::runSpecs(--argc, ++argv);      // this runs your specs with the builtin commandline parser & options
}
```

### A simple full example

First the code:
```c++
#include <cxxspec/cxxspec.hpp>
#include <iostream>

// ----- this is our project's code -----
namespace mytest {
    int my_static_var = 42;
}
// --------------------------------------

namespace mytest {
    namespace specs {

        describe(mytest, $ {                // <1>
            explain("my_static_var", $ {    // <2>
                it("should equal 42", _ {   // <3>
                    expect(mytest::my_static_var).to_eq(42);    // <4>
                });

                it("should not equal 12", _ {
                    expect(mytest::my_static_var).to_not_eq(12);
                });

                it("should equal 12", _ {
                    expect(mytest::my_static_var).to_eq(12);
                });
            });
        });

    }
}

int main(int argc, char** argv) {
    cxxspec::runSpecs(--argc, ++argv);
}
```

To build & run this example, install cxxspec and then go into the `examples` folder and run: `xmake -P . -r && xmake run -P . simple`.

With `describe` you start an new specification (see `<1>`); note that the first argument (`mytest`) is the name of the
spec and must not be a string or contain spaces, because it's used internaly to create a new function calles `__initSpec_mytest`.
For an answer why, see [Private and protected class members](#private-and-protected-class-members).

To group examples inside an spec, you can use `explain` or `context` (see `<2>`); this can be viewed as an example group.
Note that you can use them also inside each other to go arbitarily deep.

To define an example, you simply use `it`. The first param here is the name of the example.
Inside it you can declare 'expectations'.

An expectation always starts with a call to `expect(...)`. As it's argument to give it the value that you 'got'; the value
that should be checked against.

To now define what is allowed and not you call `to_*` or `to_not_*` methods on the result of `expect`.
There are a bunch of them; see [Builtin matchers](#builtin-matchers)

You use them like so: `expect(value_got).to_eq(expected_value);`.
For example: `expect(mytest::my_static_var).to_eq(42);` (see `<3>`) declares an expectation, that `mytest::my_static_var` equals `42`.
If this is the case the expectation succeeds and your example continues; however if it is not the case, the whole example will fail immediately.

If you wish a negative expectation, you can use the `to_not_*` methods. For our example above this would be `to_not_eq`.
They are like their `to_*` counterpart but they fail if the check succeeds and succeed when the check fails.
For Example: `expect(mytest::my_static_var).to_not_eq(100);` declares an expectation, that `mytest::my_static_var` does not equals `100`.
If this is the case (the equal check fails) then the expectation succeeds; if not (the equal check succeeds) then the expectation fails, and with
it the whole example.

### Private and protected class members

As we descussied above, `describe(mytest)` generates a new method `__initSpec_mytest`.
This is important for the `ALLOW_SPEC` macro, wich is needed in order to allow the spec(s) to access protected and private members of classes.
To use it, simply write `ALLOW_SPEC(mytest);` in your class to allow `mytest` full access.
And you write into he global namespace `DEFINE_SPEC(mytest);` so the `friend`-clause generated by `ALLOW_SPEC` is referring to an defined symbol.
Otherwise your compiler maybe generates an error. Also note, that the spec MUST be defined in the global namespace as well. So calling `describe`
in an namespace will break this system.

Example:
```c++
DEFINE_SPEC(mytest);    // maybe it would be practical to create an 'spec_defines.h'
                        // to define all specs there and include it anywhere where needed

namespace mytest {
    class MyObject {
    protected:
        int doSome();
        ALLOW_SPEC(mytest);
    };
}
```

### Exceptions

In order to deal with code that can throw exceptions, you need to use `expect_throw` or `expect_no_throw`.

`expect_throw(type, block)` is used to express that the lambda `block` is expected to throw an exception of type `type`.
Example:
```c++
expect_throw(std::runtime_error, [] {
    throw std::runtime_error("some reason to be thrown");
});
```
If an exception of the given type is catched, then the example continues. However, if nonthing is catched or something other than the given type is catched then
the example will fail.

`expect_no_throw(block)` is used to express that the lamda `block` is expected to throw nothing. If it does, this means an failure of the example.

### Builtin matchers

Note: `bound arrays` are all arrays that have a fixed size at compile time; for example : `int my_array[] = {1,2,3,4}`,
        which is of type `int[4]` and 'bound' to four elements. `unbound arrays` are the opposite: their length is not determineable at compile time,
        such as `int* my_array = new int[len];`.

`to_eq(...)` compares bot values on equality, this means:
- for `T[]` that the number of elements and the content must equal. Only bounded arrays work correctly. For unbounded arrays, this compares addresses.
- for `char*` that the string is equal, including the zero-terminator. Uses `strcmp` under the hood.
- for everything else it uses the `==` operator to determine equality. Note that this operator can be overwritten.

`to_neq(...)` does the opposite of `to_eq(...)`; Can be used to test the `!=` operator on objects.
This is handy when your custom object implements `==` and `!=` differently.

`to_lt(...)`, `to_gt(...)`, `to_le(...)`, `to_ge(...)` are used to compare 'got' and 'expected' with their respective comparator (`<`, `>`, `<=`, `>=`).

`to_be(...)` compares by comparing the address of both values (trys to detect if 'exactly the same')

`to_be_a<T>()` (WIP) checks if the type of the value 'got' is the same type as the given type.
- when `T` is a pointer type (such as `int*`), then `dynamic_cast` is used
- when `T` is not a pointer type, then `std::is_same` is used

`to_contain(...)` is used to check if the 'got' value contains an expected value.
Supported (standard) types:
- `T[]` (bounded arrays)
- `char*` (zero-terminated)
- All C++17 sequence containers (`std::vector`, `std::deque`, `std::array`, `std::forward_list`, `std::list`)
- `std::string`, `std::string_view`
- `std::map`, `std::unordered_map`
Should generaly work on all types that implement `begin()` and `end()` iterators for use with `std::find`.

`to_match(...)` is used to compare strings against an regex
Versions:
- `to_match(<string> [,<regex flags> [,<match flags>]])`
- `to_match(<string> [,<match flags> [,<regex flags>]])`
Where `regex flags` are all flags that are used when creating an `std::regex` (see [here](https://en.cppreference.com/w/cpp/regex/syntax_option_type)),
while `match flags` are flags that are used to a call to `std::regex_match` or `std::regex_searc` (see [here](https://en.cppreference.com/w/cpp/regex/match_flag_type)).

To express a negative expectation, just replace `to` with `to_not`.

### Example cleanup

In your examples you can use `cleanup(...)` to add code to run as cleanup after the example has completed, regardless the result.
- `cleanup(std::function<void()>)` runs the given function at cleanup
- `cleanup(void*)` adds a cleanup function to free the given pointer. Can be used instead of `free` in your example's code

### Builtin formatters

- `cxxspec::TextFormatter` (`cxxspec/formatters/text_formatter.hpp`): Base formatter for text output. Has indent support
- `cxxspec::CliFormatter` (`cxxspec/formatters/cli_formatter.hpp`): Suitable for terminal output
- `cxxspec::JsonFormatter` (`cxxspec/formatters/json_formatter.hpp`): Prints json data to the given stream

## Similar projects

- [ccspec](https://github.com/zhangsu/ccspec.git)
- [cppspec](https://github.com/toroidal-code/cppspec)