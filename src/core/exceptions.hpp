#pragma once

#include <string>
#include <stdexcept>
#include <sstream>

namespace cxxspec {

    class ExpectFailError : public std::runtime_error {
    public:
        ExpectFailError(std::string msg) : std::runtime_error(msg) {}
    };

    class ExpectationFailException : public std::runtime_error {
    public:
        ExpectationFailException(std::string msg)
            : std::runtime_error(msg)
        {}

        template<typename T_got, typename T_expected>
        static ExpectationFailException newExEqual(T_got got, T_expected expected) {
            std::stringstream ss;
            ss << "Expected " << got << " to be equal to " << expected << ", but was not";
            return ExpectationFailException(ss.str());
        }

        template<typename T_got, typename T_expected>
        static ExpectationFailException newExBe(T_got got, T_expected expected) {
            std::stringstream ss;
            ss << "Expected " << got << " to be " << expected << ", but was not";
            return ExpectationFailException(ss.str());
        }

        template<typename T_got>
        static ExpectationFailException newExBeA(T_got got) {
            std::stringstream ss;
            ss << "Expected " << got << " to be a ???, but was not";
            return ExpectationFailException(ss.str());
        }
    };

}