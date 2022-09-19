#include "cxxspec.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <map>
#include <unordered_map>
#include <cstring>
#include <deque>
#include <forward_list>
#include <list>
#include <array>
#include <thread>
#include <chrono>

DEFINE_SPEC(MyKlazz)

namespace mytest {

    int my_static_var = 42;
    std::string my_stdstr = "hello world";
    const char* my_cstr = "hello world";
    std::vector<int> my_int_vec({1, 2, 3, 4});
    std::vector<char> my_char_vec({'a', 'b', 'c'});
    std::map<int, int> my_ii_map({ {1,11}, {2,22}, {3,33} });
    std::unordered_map<int, int> my_ii_umap({ {1,11}, {2,22}, {3,33} });
    std::deque<int> my_int_deque = {1, 2, 3, 4};
    int my_int_carray[] = {1, 2, 3, 4};
    std::array<int, 4> my_int_array({1, 2, 3, 4});
    std::forward_list<int> my_int_forward_list({1, 2, 3, 4});
    std::list<int> my_int_list({1, 2, 3, 4});

    #if __cplusplus >= 201703L
        std::string_view my_strview("hello world", 5);
    #endif

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

    std::ostream& operator<< (std::ostream& stream, const MyKlazz& obj) {
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
    before_all({
        std::cout << "Display this at the beginning!\n";
    });
    before_each({
        std::cout << "Display this before every example!\n";
    });

    it("should be equal", _ {
        mytest::MyKlazz klazz_1(12);
        mytest::MyKlazz klazz_2(13);
        expect(klazz_1).to_eq(klazz_2);
    });

    it("should be same", _ {
        mytest::MyKlazz klazz_1(12);
        mytest::MyKlazz klazz_2(13);
        expect(klazz_1).to_be(klazz_2);
    });

    explain("doSome", $ {
        before_each({
            std::cout << "Second!\n";
        });

        it("should return input value", _ {
            mytest::MyKlazz klazz(12);
            expect(klazz.doSome()).to_eq(12);
        });
    });

    it("should be a MyKlazz", _ {
        mytest::MyKlazz klazz(12);
        expect(klazz).to_be_a<mytest::MyKlazz>();
    });

    it("should be a MyOtherKlazz", _ {
        mytest::MyKlazz klazz(12);
        expect(klazz).to_be_a<mytest::MyOtherKlazz>();
    });
});

describe(MyOtherKlazz, $ {
    it("should be a MyOtherKlazz", _ {
        mytest::MyOtherKlazz klazz(12);
        expect(klazz).to_be_a<mytest::MyOtherKlazz>();
    });
    it("should be a MyKlazz", _ {
        mytest::MyOtherKlazz klazz(12);
        expect(klazz).to_be_a<mytest::MyKlazz>();
    });
});

describe(mytest, $ {
    // test numeric type

    explain("my_static_var", $ {
        it("should equal 42", _ {
            expect(mytest::my_static_var).to_eq(42);
        });
        it("should equal 12", _ {
            expect(mytest::my_static_var).to_eq(12);
        });

        it("should not equal 12", _ {
            expect(mytest::my_static_var).to_neq(12);
        });
        it("should not equal 42", _ {
            expect(mytest::my_static_var).to_neq(42);
        });

        it("should be lower than 100", _ {
            expect(mytest::my_static_var).to_lt(100);
        });
        it("should be lower than 10", _ {
            expect(mytest::my_static_var).to_lt(10);
        });

        it("should be greater than 10", _ {
            expect(mytest::my_static_var).to_gt(10);
        });
        it("should be greater than 100", _ {
            expect(mytest::my_static_var).to_gt(100);
        });
    });

    // test string types

    explain("my_stdstr", $ {
        it("should be equal to 'hello world'", _ {
            std::string str = "hello world";
            expect(mytest::my_stdstr).to_eq(str);
        });
        it("should be equal to 'good morning'", _ {
            std::string str = "good morning";
            expect(mytest::my_stdstr).to_eq(str);
        });
        it("should be equal to 'hello ' + 'world'", _ {
            std::string str = "hello ";
            str += "world";
            expect(mytest::my_stdstr).to_eq(str);
        });
        it("should contain the letter 'w'", _ {
            expect(mytest::my_stdstr).to_contain('w');
        });
        it("should contain the letter 'x'", _ {
            expect(mytest::my_stdstr).to_contain('x');
        });
        it("should match /hel+o/", _ {
            std::string re_str("hEl+o");
            expect(mytest::my_stdstr).to_match(re_str, std::regex_constants::icase);
        });
        it("should match /helo/", _ {
            std::string re_str("helo");
            expect(mytest::my_stdstr).to_match(re_str);
        });
    });

    explain("my_cstr", $ {
        it("should be equal to 'hello world'", _ {
            const char* str = "hello world";
            expect(mytest::my_cstr).to_eq(str);
        });
        it("should be equal to 'good morning'", _ {
            const char* str = "good morning";
            expect(mytest::my_cstr).to_eq(str);
        });
        it("should be equal to 'hello world' (hand crafted)", _ {
            char* str = (char*) malloc(sizeof(char) * 12);
            std::strcpy(str, "hello world");
            str[11] = '\0';
            cleanup(str);

            expect(mytest::my_cstr).to_eq(str);
        });
        it("should contain the letter 'w'", _ {
            expect(mytest::my_cstr).to_contain('w');
        });
        it("should contain the letter 'x'", _ {
            expect(mytest::my_cstr).to_contain('x');
        });
        it("should match /hel+o/", _ {
            std::string re_str("hEl+o");
            expect(mytest::my_cstr).to_match(re_str, std::regex_constants::icase);
        });
        it("should match /helo/", _ {
            std::string re_str("helo");
            expect(mytest::my_cstr).to_match(re_str);
        });
    });

    #if __cplusplus >= 201703L
        explain("my_strview", $ {
            it("should be equal to (std::string) 'hello'", _ {
                std::string mystr = "hello";
                expect(mytest::my_strview).to_eq(mystr);
            });
            it("should be equal to (std::string) 'hello world'", _ {
                std::string mystr = "hello world";
                expect(mytest::my_strview).to_eq(mystr);
            });
            it("should be equal to (const char*) 'hello'", _ {
                const char* mystr = "hello";
                expect(mytest::my_strview).to_eq(mystr);
            });
            it("should be equal to (const char*) 'hello world'", _ {
                const char* mystr = "hello world";
                expect(mytest::my_strview).to_eq(mystr);
            });
            it("should be equal to (handcrafted char*) 'hello'", _ {
                char* mystr = (char*) malloc(sizeof(char) * 6);
                strcpy(mystr, "hello");
                mystr[5] = '\0';
                cleanup(mystr);
                expect(mytest::my_strview).to_eq(mystr);
            });
            it("should be equal to (handcrafted char*) 'hello world'", _ {
                char* mystr = (char*) malloc(sizeof(char) * 12);
                strcpy(mystr, "hello world");
                mystr[11] = '\0';
                cleanup(mystr);
                expect(mytest::my_strview).to_eq(mystr);
            });
            it("should be equal to (std::string_view) 'hello'", _ {
                std::string_view strview("hello world", 5);
                expect(mytest::my_strview).to_eq(strview);
            });
            it("should be equal to (std::string_view) 'hello world'", _ {
                std::string_view strview("hello world");
                expect(mytest::my_strview).to_eq(strview);
            });
            it("should contain 'h'", _ {
                expect(mytest::my_strview).to_contain('h');
            });
            it("should contain 'w'", _ {
                expect(mytest::my_strview).to_contain('w');
            });
            it("should contain 'g'", _ {
                expect(mytest::my_strview).to_contain('g');
            });
            it("should match /hel+o/", _ {
                std::string re_str("hEl+o");
                expect(mytest::my_strview).to_match(re_str, std::regex_constants::icase);
            });
            it("should match /helo/", _ {
                std::string re_str("helo");
                expect(mytest::my_strview).to_match(re_str);
            });
        });
    #endif

    // test container types: https://en.cppreference.com/w/cpp/container

    explain("my_int_vec", $ {
        it("should wait 3secs", _ {
            std::this_thread::sleep_for( std::chrono::seconds(3) + std::chrono::milliseconds(10) );
        });
        it("should be equal to [1,2,3,4]", _ {
            std::vector<int> vec({1, 2, 3, 4});
            expect(mytest::my_int_vec).to_eq(vec);
        });
        it("should not be equal to [1,2,3,5]", _ {
            std::vector<int> vec({1, 2, 3, 5});
            expect(mytest::my_int_vec).to_neq(vec);
        });
        it("should contain '2'", _ {
            expect(mytest::my_int_vec).to_contain(2);
        });
        it("should contain '5'", _ {
            expect(mytest::my_int_vec).to_contain(5);
        });
        it("should not contain '5'", _ {
            expect(mytest::my_int_vec).to_not_contain(5);
        });
        it("should not contain '2'", _ {
            expect(mytest::my_int_vec).to_not_contain(2);
        });
    });

    explain("my_int_deque", $ {
        it("should be equal with {1,2,3,4}", _ {
            std::deque<int> d = {1, 2, 3, 4};
            expect(mytest::my_int_deque).to_eq(d);
        });
        it("should be equal with {1,2,3,4,5}", _ {
            std::deque<int> d = {1, 2, 3, 4, 5};
            expect(mytest::my_int_deque).to_eq(d);
        });
        it("should contain '3'", _ {
            expect(mytest::my_int_deque).to_contain(3);
        });
        it("should contain '5'", _ {
            expect(mytest::my_int_deque).to_contain(5);
        });
    });

    explain("my_int_carray", $ {
        it("should equal to {1,2,3,4}", _ {
            int my_cary[] = {1, 2, 3, 4};
            expect(mytest::my_int_carray).to_eq(my_cary);
        });
        it("should equal to {1,2,3,5}", _ {
            int my_cary[] = {1, 2, 3, 5};
            expect(mytest::my_int_carray).to_eq(my_cary);
        });
        it("should contain '4'", _ {
            expect(mytest::my_int_carray).to_contain(4);
        });
        it("should contain '5'", _ {
            expect(mytest::my_int_carray).to_contain(5);
        });
    });

    explain("my_int_array", $ {
        it("should equal to {1,2,3,4}", _ {
            std::array<int, 4> ary({1, 2, 3, 4});
            expect(mytest::my_int_array).to_eq(ary);
        });
        it("should equal to {1,2,3,5}", _ {
            std::array<int, 4> ary({1, 2, 3, 5});
            expect(mytest::my_int_array).to_eq(ary);
        });
        it("should contain '4'", _ {
            expect(mytest::my_int_array).to_contain(4);
        });
        it("should contain '5'", _ {
            expect(mytest::my_int_array).to_contain(5);
        });
    });

    explain("my_int_forward_list", $ {
        it("should contain '4'", _ {
            expect(mytest::my_int_forward_list).to_contain(4);
        });
        it("should contain '5'", _ {
            expect(mytest::my_int_forward_list).to_contain(5);
        });
    });

    explain("my_int_list", $ {
        it("should contain '4'", _ {
            expect(mytest::my_int_list).to_contain(4);
        });
        it("should contain '5'", _ {
            expect(mytest::my_int_list).to_contain(5);
        });
    });

    explain("my_ii_map", $ {
        it("should be equal to { {1,11}, {2,22}, {3,33} }", _ {
            std::map<int, int> mymap({ {1,11}, {2,22}, {3,33} });
            expect(mytest::my_ii_map).to_eq(mymap);
        });
        it("should be equal to { {1,11}, {2,22}, {3,44} }", _ {
            std::map<int, int> mymap({ {1,11}, {2,22}, {3,44} });
            expect(mytest::my_ii_map).to_eq(mymap);
        });
        it("should contain the pair {2,22}", _ {
            std::pair<int, int> mymap_pair({2, 22});
            expect(mytest::my_ii_map).to_contain(mymap_pair);
        });
        it("should contain the pair {2,33}", _ {
            std::pair<int, int> mymap_pair({2, 33});
            expect(mytest::my_ii_map).to_contain(mymap_pair);
        });
    });

    explain("my_ii_umap", $ {
        it("should be equal to { {1,11}, {2,22}, {3,33} }", _ {
            std::unordered_map<int, int> myumap({ {1,11}, {2,22}, {3,33} });
            expect(mytest::my_ii_umap).to_eq(myumap);
        });
        it("should be equal to { {1,11}, {2,22}, {3,44} }", _ {
            std::unordered_map<int, int> myumap({ {1,11}, {2,22}, {3,44} });
            expect(mytest::my_ii_umap).to_eq(myumap);
        });
        it("should contain the pair {2,22}", _ {
            std::pair<int, int> mymap_pair = std::pair<int, int>({2, 22});
            expect(mytest::my_ii_umap).to_contain(mymap_pair);
        });
        it("should contain the pair {2,33}", _ {
            std::pair<int, int> mymap_pair({2, 33});
            expect(mytest::my_ii_umap).to_contain(mymap_pair);
        });
    });

    // test exception throwing

    explain("test expect_throw", $ {
        it("should expect throws (1)", _ {
            expect_throw(std::runtime_error, [] { throw std::runtime_error(""); });
        });
        it("should expect throws (2)", _ {
            expect_throw(std::runtime_error, [] { });
        });
        it("should expect throws (3)", _ {
            expect_throw(std::runtime_error, [] { throw "i am a string"; });
        });

        it("should not expect throws (1)", _ {
            expect_no_throw([] { });
        });
        it("should not expect throws (2)", _ {
            expect_no_throw([] { throw new std::runtime_error("test what of exception"); });
        });
    });
});

int main(int argc, char** argv) {
    using namespace cxxspec;
    runSpecs(--argc, ++argv);
}