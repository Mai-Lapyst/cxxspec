#pragma once

#include "../core/matcher.hpp"
#include "../core/util.hpp"
#include "../core/pretty_print.hpp"

#include <sstream>
#include <string>
#include <cstring>
#include <iostream>

namespace cxxspec {
    namespace matchers {

        template<typename T_got, typename T_expected = T_got>
        class CompareMatcher : public ValueMatcher<T_got, T_expected> {
        public:
            using ValueMatcher<T_got, T_expected>::ValueMatcher;
            std::string reason(const T_got& got) {
                std::stringstream ss;
                ss << "Expected " << prettyprint::inspect(got);
                if (this->is_negative)
                    ss << " not";
                ss << ' ' << this->compare_text() << ' ' << prettyprint::inspect(this->expected_value) << ", but was";
                if (!this->is_negative)
                    ss << " not";
                return ss.str();
            }
        protected:
            virtual std::string compare_text() = 0;
        };

        /**
         * Matcher to check if 'got' equals 'expected' through use of the `==` operator
         */
        template<typename T_got, typename T_expected = T_got>
        class EqualMatcher : public CompareMatcher<T_got, T_expected> {
        protected:
            typedef std::integral_constant<int, 0> cstring_type;
            typedef std::integral_constant<int, 1> carray_type;
            typedef std::integral_constant<int, 2> other_type;

            struct check_type : std::conditional<
                util::is_c_str<T_got>::value,
                cstring_type,
                typename std::conditional<
                    util::is_bounded_array<T_got>::value && util::is_bounded_array<T_expected>::value,
                    carray_type,
                    other_type
                >::type
            >::type {};

        public:
            using CompareMatcher<T_got, T_expected>::CompareMatcher;
            bool match(const T_got& got) {
                return this->_match(got, check_type{});
            }
        private:
            std::string compare_text() {
                switch ( check_type::value ) {
                    case cstring_type::value:
                        return "to be equal (`strcmp() == 0`) with";
                    case carray_type::value:
                        return "to be equal with";
                    default:
                        return "to be equal (`==`) with";
                }
            }

            bool _match(const T_got& got, cstring_type) {
                if (got == nullptr) { return this->expected_value == nullptr; }
                return (std::strcmp(got, this->expected_value) == 0);
            }

            bool _match(const T_got& got, carray_type) {
                int got_len = util::get_array_bound<T_got>::value;
                int expected_len = util::get_array_bound<T_expected>::value;
                if (got_len != expected_len) {
                    return false;
                }
                for (int i = 0; i < got_len; i++) {
                    if (got[i] != this->expected_value[i]) {
                        return false;
                    }
                }
                return true;
            }

            bool _match(const T_got& got, other_type) {
                return (got == this->expected_value);
            }
        };

        /**
         * Matcher to check if 'got' not equals 'expected' through use of the `!=` operator
         */
        template<typename T_got, typename T_expected = T_got>
        class NotEqualMatcher : public CompareMatcher<T_got, T_expected> {
        protected:
            typedef std::integral_constant<int, 0> cstring_type;
            typedef std::integral_constant<int, 1> carray_type;
            typedef std::integral_constant<int, 2> other_type;

            struct check_type : std::conditional<
                util::is_c_str<T_got>::value,
                cstring_type,
                typename std::conditional<
                    util::is_bounded_array<T_got>::value && util::is_bounded_array<T_expected>::value,
                    carray_type,
                    other_type
                >::type
            >::type {};

        public:
            using CompareMatcher<T_got, T_expected>::CompareMatcher;
            bool match(const T_got& got) {
                return this->_match(got, check_type{});
            }
        private:
            std::string compare_text() {
                switch ( check_type::value ) {
                    case cstring_type::value:
                        return "to be not equal (`strcmp() != 0`) with";
                    case carray_type::value:
                        return "to be not equal with";
                    default:
                        return "to be not equal (`!=`) with";
                }
            }

            bool _match(const T_got& got, cstring_type) {
                if (got == nullptr) { return this->expected_value == nullptr; }
                return (std::strcmp(got, this->expected_value) != 0);
            }

            bool _match(const T_got& got, carray_type) {
                int got_len = util::get_array_bound<T_got>::value;
                int expected_len = util::get_array_bound<T_expected>::value;
                if (got_len != expected_len) {
                    // if we did not have the same length on both sides we are definitifly unequal
                    return true;
                }
                for (int i = 0; i < got_len; i++) {
                    if (got[i] != this->expected_value[i]) {
                        return true;
                    }
                }
                return false;
            }

            bool _match(const T_got& got, other_type) {
                return (got != this->expected_value);
            }
        };

        /**
         * Matcher to check if 'got' is lower than 'expected' through use of the `<` operator
         */
        template<typename T_got, typename T_expected = T_got>
        class LowerThanMatcher : public CompareMatcher<T_got, T_expected> {
        public:
            using CompareMatcher<T_got, T_expected>::CompareMatcher;
            bool match(const T_got& got) {
                return (got < this->expected_value);
            }
        protected:
            std::string compare_text() { return "to be lower (`<`) than"; }
        };

        /**
         * Matcher to check if 'got' is greater than 'expected' through use of the `>` operator
         */
        template<typename T_got, typename T_expected = T_got>
        class GreaterThanMatcher : public CompareMatcher<T_got, T_expected> {
        public:
            using CompareMatcher<T_got, T_expected>::CompareMatcher;
            bool match(const T_got& got) {
                return (got > this->expected_value);
            }
        protected:
            std::string compare_text() { return "to be greater (`>`) than"; }
        };

        /**
         * Matcher to check if 'got' is lower or equal than 'expected' through use of the `<=` operator
         */
        template<typename T_got, typename T_expected = T_got>
        class LowerEqualMatcher : public CompareMatcher<T_got, T_expected> {
        public:
            using CompareMatcher<T_got, T_expected>::CompareMatcher;
            bool match(const T_got& got) {
                return (got <= this->expected_value);
            }
        protected:
            std::string compare_text() { return "to be lower or equal (`<=`) than"; }
        };

        /**
         * Matcher to check if 'got' is greater or equal than 'expected' through use of the `>=` operator
         */
        template<typename T_got, typename T_expected = T_got>
        class GreaterEqualMatcher : public CompareMatcher<T_got, T_expected> {
        public:
            using CompareMatcher<T_got, T_expected>::CompareMatcher;
            bool match(const T_got& got) {
                return (got >= this->expected_value);
            }
        protected:
            std::string compare_text() { return "to be greater or equal (`>=`) than"; }
        };

    }
}