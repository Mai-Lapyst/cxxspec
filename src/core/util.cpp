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

#include "./util.hpp"

#if defined(__GNUG__) || defined(__clang__)
    #include <cxxabi.h>
    #include <memory>
#else
    #warning "Platform not fully supported; missing symbol demangling support!"
#endif

namespace cxxspec {
    namespace util {

        #if defined(__GNUG__) || defined(__clang__)
            std::string demangle(const char* mangledName) {
                int status;
                std::unique_ptr<char[], void (*)(void *)> result{
                    abi::__cxa_demangle(mangledName, NULL, NULL, &status), std::free};
                return (status == 0) ? result.get() : mangledName;
            }

            std::string current_exception_typename() {
                return demangle(abi::__cxa_current_exception_type()->name());
            }
        #else
            std::string demangle(const char* mangledName) {
                return mangledName;
            }
            std::string current_exception_typename() {
                return "got unknown exception type";
            }
        #endif

    }
}