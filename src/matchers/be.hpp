/*
 * cxxspec - a TDD/BDD framework for c++ projects
 * Copyright (C) 2021-2024 Mai-Lapyst
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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