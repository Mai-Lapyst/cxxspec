#include <cxxspec/cxxspec.hpp>
#include <iostream>

// ----- this is our project's code -----
namespace mytest {
    int my_static_var = 42;
}
// --------------------------------------

namespace mytest {
    namespace specs {

        describe(mytest, $ {
            explain("my_static_var", $ {
                it("should equal 42", _ {
                    expect(mytest::my_static_var).to_eq(42);
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