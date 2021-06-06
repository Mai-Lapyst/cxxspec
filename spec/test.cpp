#include "cxxspec.hpp"
#include "formatters/cli_formatter.hpp"
#include "matchers/matchers.hpp"

#include <iostream>

DEFINE_SPEC(MyKlazz)

namespace mytest {

    int my_static_var = 42;

    class MyKlazz {
    public:
        int i;

        MyKlazz(int i) : i(i) {}

        bool operator==(MyKlazz other) const {
            return true;
        }

    protected:
        int doSome() { return i; }
        ALLOW_SPEC(MyKlazz);
    };

    std::ostream& operator<< (std::ostream& stream, MyKlazz& obj) {
        stream << "MyKlazz" << &obj << "< i=" << obj.i << " >";
        return stream;
    }

    class MyOtherKlazz : public MyKlazz {
    public:
        MyOtherKlazz(int i) : MyKlazz(i) {}
    };
}

using namespace cxxspec::matchers;
describe(MyKlazz, $ {
    it("should be equal", _ {
        mytest::MyKlazz klazz_1(12);
        mytest::MyKlazz klazz_2(13);
        expect(klazz_1).to(eq(klazz_2));
    });

    explain("doSome", $ {
        it("should return input value", _ {
            mytest::MyKlazz klazz(12);
            expect(klazz.doSome()).to(eq(12));
        });
    });
});

describe(mytest, $ {
    explain("my_static_var", $ {
        it("should equal 42", _ {
            expect(mytest::my_static_var).to(eq(42));
        });
        it("should equal 12", _ {
            expect(mytest::my_static_var).to(eq(12));
        });

        it("should not equal 12", _ {
            expect(mytest::my_static_var).to(neq(12));
        });
        it("should not equal 42", _ {
            expect(mytest::my_static_var).to(neq(42));
        });

        it("should be lower than 100", _ {
            expect(mytest::my_static_var).to(lt(100));
        });
        it("should be lower than 10", _ {
            expect(mytest::my_static_var).to(lt(10));
        });

        it("should be greater than 10", _ {
            expect(mytest::my_static_var).to(gt(10));
        });
        it("should be greater than 100", _ {
            expect(mytest::my_static_var).to(gt(100));
        });

    });
});

int main() {
    using namespace cxxspec;
    CliFormatter formatter(std::cout);
    runAllSpecs(formatter);
}