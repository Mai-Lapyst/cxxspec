#pragma once

#include "./util.hpp"

#include <string>
#include <sstream>

namespace cxxspec {
    namespace prettyprint {

        template<typename T>
        typename std::enable_if<util::is_streamable<T>::value, std::string>::type
        inline inspect_body(T& obj) {
            std::stringstream ss;
            ss << obj;
            return ss.str();
        }

        template<typename T>
        typename std::enable_if<!util::is_streamable<T>::value, std::string>::type
        inline inspect_body(T& obj) {
            std::stringstream ss;
            ss << "#<" << util::demangle(typeid(obj).name()) << ":" << &obj << ">";
            return ss.str();
        }

        template<typename T>
        std::string inspect(T& obj) {
            std::stringstream ss;
            ss << "(" << util::demangle(typeid(obj).name()) << ") " << inspect_body(obj);
            return ss.str();
        }

        template<>
        std::string inspect<const char *>(const char *&o) {
            std::stringstream ss;
            ss << "(const char*) \"" << o << '"';
            return ss.str();
        }

        template<>
        std::string inspect<std::string>(std::string&o) {
            std::stringstream ss;
            ss << "(std::string) \"" << o << '"';
            return ss.str();
        }

    }
}