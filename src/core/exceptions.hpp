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