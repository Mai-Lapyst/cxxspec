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
        using cxxspec::matchers::eq;

        describe(mytest, $ {
            explain("my_static_var", $ {
                it("should equal 42", _ {
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