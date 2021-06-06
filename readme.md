# cxxspec

cxxspec is an BDD/TDD framework that's like ruby's [rspec](https://rspec.info/), and heavily inspired by it.

## License

This project is licensed under the terms of the AGPL v3.0 .
For license details please see the file `LICENSE`.

## Todo / Roadmap

- implement more builtin matchers
    - for array/vector -like types
    - for map -like types
    - for exception catching
    - ...
- enhance the system that allows us to access protected members of other classes (`ALLOW_SPEC`)
- implement `before`, `after` and `around` hooks
- formatters
    - xml
    - junit
    - yaml
    - ...
- default parsing of cli options
    - would allow one shot's
    - would allow specifing formatter to output
    - would allow specifing formatter options
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
#include "cxxspec/matchers/matchers.hpp"    // includes all builtin matchers like 'eq', 'be'...

// includes the default cli formatter, suitable for output in an terminal
//  for more formatters, see below
#include "cxxspec/formatters/cli_formatter.hpp"

#include <iostream>     // for std::cout

int main() {
    using namespace cxxspec;
    CliFormatter formatter(std::cout);      // creates an instance of our formatter to operate on std::out
    runAllSpecs(formatter);                 // run all defined specs with our formatter
}
```

### A simple full example

First the code:
```c++
#include <cxxspec/cxxspec.hpp>
#include <cxxspec/matchers/matchers.hpp>
#include <cxxspec/formatters/cli_formatter.hpp>
#include <iostream>

// ----- this is our project's code -----
namespace mytest {
    int my_static_var = 42;
}
// --------------------------------------

namespace mytest {
    namespace specs {
        using cxxspec::matchers::eq;        // <1>

        describe(mytest, $ {                // <2>
            explain("my_static_var", $ {    // <3>
                it("should equal 42", _ {   // <4>
                    expect(mytest::my_static_var).to(eq(42));
                });

                it("should not equal 12", _ {
                    expect(mytest::my_static_var).to_not(eq(12));
                });

                it("should equal 12", _ {
                    expect(mytest::my_static_var).to(eq(12));
                });
            });
        });

    }
}

int main() {
    using namespace cxxspec;
    CliFormatter formatter(std::cout);
    runAllSpecs(formatter);
}
```

To build & run this example, install cxxspec and then go into the `examples` folder and run: `xmake -P . -r && xmake run -P . simple`.

In order to use matchers (thats are the things that determine if somthing is ok or not), we need to import them.
You can to that by using for example `using cxxspec::matchers::eq;` to import only the `eq` matcher (see `<1>`),
or you could alternatively use `using namespace cxx::matchers;` to import all builtin matchers.

With `describe` you start an new specification (see `<2>`); note that the first argument (`mytest`) is the name of the
spec and must not be a string or contain spaces, because it's used internaly to create a new function calles `__initSpec_mytest`.
For an answer why, see below: 'Private and protected class members'.

To group examples inside an spec, you can use `explain` or `context` (see `<3>`); this can be viewed as an example group.
Note that you can use them also inside each other to go arbitarily deep.

To define an example, you simply use `it`. The first param here is the name of the example.
Inside it you can declare 'expectations'.

An expectation always starts with a call to `expect(...)`. As it's argument to give it the value that you 'got'; the value
that should be checked against.

To now define what is allowed and not you call `to(...)` or `not_to(...)` (`to_not(...)` also works) on the result of `expect`.
The both variants with the `not` in their name negate the matcher; their first parmeter.
This means: if your matcher returns 'ok' when you used `to` then all is fine and only a 'not ok' will cause the example to fail.
With `not_to` and `to_not` this is reversed: 'ok' will cause an fail while with 'not ok' all is fine.

Matchers are the actual peace that decide if an example fails or not. For an list of builtin matchers see below.
For simplicity's sake we just use `eq` here:

`eq` will compare the value 'got' (parameter of `expect`) with an expected value (parameter of `eq`).
It compares by using the compare operator `==` (which can be overloaded in c++) to decide if both values are equal or not.
If they are indeed equal then it returns a 'ok' state; else it returns a 'not ok' state.

If you then use it with `to` and `not_to`, you can create this:
- `to(eq(12))`: fails only when the value is not equal with `12`
- `not_to(eq(12))`: fails only when the value is equal with `12`

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

### Builtin matchers

- `be(...)`: Compares by comparing the address of both values (trys to detect if 'exactly the same')
- `eq(...)`: Compares by using the `==` operator
- `neq(...)`: Compares by using the `!=` operator
- `lt(...)`: Compares by using the `<` operator
- `gt(...)`: Compares by using the `>` operator
- `le(...)`: Compares by using the `<=` operator
- `ge(...)`: Compares by using the `>=` operator

### Builtin formatters

- `cxxspec::TextFormatter` (`cxxspec/formatters/text_formatter.hpp`): Base formatter for text output. Has indent support
- `cxxspec::CliFormatter` (`cxxspec/formatters/cli_formatter.hpp`): Suitable for terminal output
- `cxxspec::JsonFormatter` (`cxxspec/formatters/json_formatter.hpp`): Prints json data to the given stream

## Similar projects

- [ccspec](https://github.com/zhangsu/ccspec.git)
- [cppspec](https://github.com/toroidal-code/cppspec)