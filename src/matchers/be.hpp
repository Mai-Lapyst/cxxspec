#pragma once

#include "../core/matcher.hpp"
#include "../core/util.hpp"
#include "../core/pretty_print.hpp"

#include <string>
#include <sstream>

namespace cxxspec {
    namespace matchers {

        template<typename T_got, typename T_expected>
        class BeMatcher : public ValueMatcher<T_got, T_expected> {
        public:
            using ValueMatcher<T_got, T_expected>::ValueMatcher;
            bool match(const T_got& got) {
                return (&got == &this->expected_value);
            }
            std::string reason(const T_got& got) {
                std::stringstream ss;
                ss << "Expected " << prettyprint::inspect(got);
                if (this->is_negative)
                    ss << " not";
                ss << " to be " << prettyprint::inspect(this->expected_value) << ", but was";
                if (!this->is_negative)
                    ss << " not";
                return ss.str();
            }
        };

        template<typename T_got, typename T_expected>
        class BeAMatcher : public Matcher<T_got> {
        public:
            using Matcher<T_got>::Matcher;

            bool match(const T_got& got) {
                //return std::is_same<T_got, T_expected>::value;
                return this->_match(got, std::is_pointer<T_expected>{});
            }

            std::string reason(const T_got& got) {
                std::stringstream ss;
                ss << "Expected " << prettyprint::inspect(got);
                if (this->is_negative)
                    ss << " not";
                ss << " to be a " << util::demangle(typeid(T_expected).name()) << ", but was";
                if (!this->is_negative)
                    ss << " not";
                return ss.str();
            }

        private:
            bool _match(const T_got& got, std::false_type) {
                return std::is_same<T_got, T_expected>::value;
            }
            bool _match(const T_got& got, std::true_type) {
                if (dynamic_cast<T_expected*>(&got)) {
                    return true;
                }
                return false;
            }
        };

    }
}