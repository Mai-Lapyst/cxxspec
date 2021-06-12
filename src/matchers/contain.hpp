#pragma once

#include "../core/matcher.hpp"
#include "../core/util.hpp"
#include "../core/pretty_print.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>
#include <unordered_map>

#include <iostream>

namespace cxxspec {
    namespace matchers {

        template<typename T_got, typename T_expected = T_got>
        class IncludeMatcher : public ValueMatcher<T_got, T_expected> {
        protected:
            typedef std::integral_constant<int, 0> mapping_type;
            typedef std::integral_constant<int, 1> carray_type;
            typedef std::integral_constant<int, 2> cstring_type;
            typedef std::integral_constant<int, 3> iterator_type;

            struct check_type : std::conditional<
                util::is_mappish<T_got>::value,
                mapping_type,
                typename std::conditional<
                    util::is_bounded_array<T_got>::value,
                    carray_type,
                    typename std::conditional<util::is_c_str<T_got>::value, cstring_type, iterator_type>::type
                >::type
            >::type {};

        public:
            using ValueMatcher<T_got, T_expected>::ValueMatcher;

            bool match(const T_got& got) {
                return this->_match(got, check_type{});
            }

            std::string reason(const T_got& got) {
                std::stringstream ss;
                ss << "Expected " << prettyprint::inspect(got);
                if (this->is_negative)
                    ss << " not";
                ss << " to contain " << prettyprint::inspect(this->expected_value) << ", but has";
                if (!this->is_negative)
                    ss << " not";
                return ss.str();
            }

        private:
            bool _match(const T_got& got, iterator_type) {
                return std::find(got.begin(), got.end(), this->expected_value) != got.end();
            }

            bool _match(const T_got& got, carray_type) {
                for (int i = 0; i < util::get_array_bound<T_got>::value; i++) {
                    if (got[i] == this->expected_value) {
                        return true;
                    }
                }
                return false;
            }

            bool _match(const T_got& got, cstring_type) {
                for (int i = 0; i < strlen(got); i++) {
                    if (got[i] == this->expected_value) {
                        return true;
                    }
                }
                return false;
            }

            bool _match(const T_got& got, mapping_type) {
                for (auto it = got.begin(); it != got.end(); it++) {
                    if ((*it).first == this->expected_value.first) {
                        return ((*it).second == this->expected_value.second);
                    }
                }
                return false;
            }
        };

    }
}