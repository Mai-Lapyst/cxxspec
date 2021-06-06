#pragma once

#include "../core/matcher.hpp"
#include "../core/util.hpp"
#include "../core/pretty_print.hpp"

#include <sstream>
#include <iostream>

namespace cxxspec {
    namespace matchers {

        template<typename T>
        class OpMatcher : public ValueMatcher<T> {
        public:
            OpMatcher(T& expected) : ValueMatcher<T>(expected) {}
            std::string reason(const T& got) {
                std::stringstream ss;
                ss << "Expected " << prettyprint::inspect(got);
                if (this->is_negative)
                    ss << " not";
                ss << this->op_text() << prettyprint::inspect(this->expected_value) << ", but was";
                if (!this->is_negative)
                    ss << " not";
                return ss.str();
            }
        protected:
            virtual std::string op_text() = 0;
        };

        #define OPM(cn, n)   \
            template<typename T> cn<T> n(T& expected) { return cn<T>(expected); } \
            template<typename T> cn<T> n(T&& expected) { T& _expected = expected; return cn<T>(_expected); }

        //--------------------------------------------------------------------------------

        template<typename T>
        class Eq : public ValueMatcher<T> {
        public:
            Eq(T& expected_value) : ValueMatcher<T>(expected_value) {}
            bool match(const T& got) {
                return (got == this->expected_value);
            }
            std::string reason(const T& got) {
                std::stringstream ss;
                ss << "Expected " << prettyprint::inspect(got);
                if (this->is_negative)
                    ss << " not";
                ss << " to be equal (`==`) with " << prettyprint::inspect(this->expected_value) << ", but was";
                if (!this->is_negative)
                    ss << " not";
                return ss.str();
            }
        };

        OPM(Eq, eq);

        template<typename T>
        class Neq : public OpMatcher<T> {
        public:
            using OpMatcher<T>::OpMatcher;
            bool match(const T& got) {
                return (got != this->expected_value);
            }
        protected:
            std::string op_text() { return " to be not equal (`!=`) with "; }
        };

        OPM(Neq, neq);

        //--------------------------------------------------------------------------------

        template<typename T>
        class Lt : public OpMatcher<T> {
        public:
            using OpMatcher<T>::OpMatcher;
            bool match(const T& got) {
                return (got < this->expected_value);
            }
        protected:
            std::string op_text() { return " to be lower (`<`) than "; }
        };

        OPM(Lt, lt);

        template<typename T>
        class Gt : public OpMatcher<T> {
        public:
            using OpMatcher<T>::OpMatcher;
            bool match(const T& got) {
                return (got > this->expected_value);
            }
        protected:
            std::string op_text() { return " to be greater (`>`) than "; }
        };

        OPM(Gt, gt);

        template<typename T>
        class Le : public OpMatcher<T> {
        public:
            using OpMatcher<T>::OpMatcher;
            bool match(const T& got) {
                return (got <= this->expected_value);
            }
        protected:
            std::string op_text() { return " to be lower or equal (`<=`) than "; }
        };

        OPM(Le, le);

        template<typename T>
        class Ge : public OpMatcher<T> {
        public:
            using OpMatcher<T>::OpMatcher;
            bool match(const T& got) {
                return (got >= this->expected_value);
            }
        protected:
            std::string op_text() { return " to be greater or equal (`>=`) than "; }
        };

        OPM(Ge, ge);

        //--------------------------------------------------------------------------------

        template<typename T>
        LamdaValueMatcher<T> be(T& expected) {
            return LamdaValueMatcher<T>(
                expected,
                lvm_do {
                    return (&got == &expected);
                },
                lvm_reson {
                    std::stringstream ss;
                    ss << "Expected " << prettyprint::inspect(got);
                    if (is_negative)
                        ss << " not";
                    ss << " to be " << prettyprint::inspect(expected) << ", but was";
                    if (!is_negative)
                        ss << " not";
                    return ss.str();
                }
            );
        }

        /* template<typename C, typename T>
        LamdaMatcher<T> be_a() {
            return LamdaMatcher<T>(
                lm_do {
                    if (dynamic_cast<C*>(&got)) {
                        return true;
                    }
                    return false;
                },
                lm_reson {
                    std::stringstream ss;
                    ss << "Expected " << prettyprint::inspect(got);
                    if (is_negative)
                        ss << " not";
                    ss << " to be a " << util::demangle(typeid(C).name()) << ", but was";
                    if (!is_negative)
                        ss << " not";
                    return ss.str();
                }
            );
        } */

    }
}