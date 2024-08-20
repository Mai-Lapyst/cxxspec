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

#include "./util.hpp"

#include <string>
#include <regex>
#include <sstream>
#include <map>

#if __cplusplus >= 201703L
    #include <string_view>
#endif

namespace cxxspec {
    namespace prettyprint {

        // forward decl's
        template<typename T1, typename T2>
        std::ostream& operator<<(std::ostream& stream, std::pair<T1, T2> pair);
        // --------------

        template<typename T>
        typename std::enable_if<
            util::is_streamable<T>::value && !util::is_c_str<T>::value && !util::is_bounded_array<T>::value,
            std::string
        >::type
        inline inspect_body(T& obj) {
            std::stringstream ss;
            ss << obj;
            return ss.str();
        }

        template<typename T>
        typename std::enable_if<util::is_bounded_array<T>::value, std::string>::type
        inline inspect_body(T& obj) {
            std::stringstream ss;
            ss << '{';
            for (int i = 0; i < util::get_array_bound<T>::value; i++) {
                ss << obj[i] << ',';
            }
            ss.seekp(-1, ss.cur);
            ss << '}';
            return ss.str();
        }

        template<typename T>
        typename std::enable_if<util::is_c_str<T>::value, std::string>::type
        inline inspect_body(T& obj) {
            if (obj == nullptr) {
                return "nullptr";
            }
            std::stringstream ss;
            ss << '"' << obj << '"';
            return ss.str();
        }

        template<typename T>
        typename std::enable_if<util::is_pairish<T>::value, std::string>::type
        inline inspect_body(T& obj) {
            std::stringstream ss;
            ss << obj;
            return ss.str();
        }

        template<typename T>
        typename std::enable_if<
            !util::is_streamable<T>::value && !util::is_iterateable<T>::value && !util::is_c_str<T>::value && !util::is_pairish<T>::value,
            std::string
        >::type
        inline inspect_body(T& obj) {
            std::stringstream ss;
            ss << "#<" << util::demangle(typeid(obj).name()) << ":" << &obj << ">";
            return ss.str();
        }

        template<typename T>
        typename std::enable_if<util::is_iterateable<T>::value, std::string>::type
        inline inspect_body(T& obj) {
            std::stringstream ss;
            ss << "{";
            for (auto it = obj.begin(); it != obj.end(); it++) {
                ss << inspect_body(*it);
                ss << ",";
            }
            ss.seekp(-1, ss.cur);
            ss << "}";
            return ss.str();
        }

        template<typename T>
        std::string inspect(T& obj) {
            std::stringstream ss;
            ss << "(" << util::demangle(typeid(T).name()) << ") " << inspect_body(obj);
            return ss.str();
        }

        template<>
        inline std::string inspect<const char *>(const char *& o) {
            if (o == nullptr) {
                return "(const char*) nullptr";
            }
            std::stringstream ss;
            ss << "(const char*) \"" << o << '"';
            return ss.str();
        }

        template<>
        inline std::string inspect<std::string>(std::string& o) {
            std::stringstream ss;
            ss << "(std::string) \"" << o << '"';
            return ss.str();
        }

        template<>
        inline std::string inspect<const std::string>(const std::string& o) {
            std::stringstream ss;
            ss << "(std::string) \"" << o << '"';
            return ss.str();
        }

        #if __cplusplus >= 201703L
            template<>
            inline std::string inspect<std::string_view>(std::string_view& o) {
                std::stringstream ss;
                ss << "(std::string_view) \"" << o << '"';
                return ss.str();
            }

            template<>
            inline std::string inspect<const std::string_view>(const std::string_view& o) {
                std::stringstream ss;
                ss << "(std::string_view) \"" << o << '"';
                return ss.str();
            }
        #endif

        //----------------------------------------

        template<typename T1, typename T2>
        inline std::ostream& operator<<(std::ostream& stream, std::pair<T1, T2> pair) {
            stream << '{' << inspect_body<T1>(pair.first) << ", " << inspect_body<T2>(pair.second) << "}";
            return stream;
        };

    }
}