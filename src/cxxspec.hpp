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

#include <vector>

#include "./core/core.hpp"
#include "./core/expect.hpp"
#include "./core/formatter.hpp"
#include "./core/exceptions.hpp"

namespace cxxspec {

    extern std::vector<Spec> all_specs;

    void runAllSpecs(Formatter& formatter, bool onlyMarked = false);

    void runSpecs(std::vector<std::string>& arguments);

    /**
     * Run all specs; uses supplied argc and argv to do commandline argument parsing.
     * Needs $0 / the programm name removed.
     * 
     * You can do this by simply do the following:
     *      runSpecs(--argc, ++argv);
     */
    void runSpecs(int argc, char** argv);

    // TODO: this only works on gcc; for alternatives see https://stackoverflow.com/questions/1113409/attribute-constructor-equivalent-in-vc
    #define cxxspec_autoload    __attribute__((constructor))

    #define describe(name, block)       \
        cxxspec_autoload                \
        void __initSpec_##name () {     \
            cxxspec::all_specs.push_back( cxxspec::Spec(#name, block) ); \
        }

    #define explain     self._context
    #define context     self._context

    #define it(name, ...)  self._it(name, __FILE__, __VA_ARGS__)

    #define expect      self.expect
    #define cleanup     self.cleanup

    #define expect_throw(type, block)   self.expect_throw<type>(block);
    #define expect_no_throw             self.expect_no_throw

    #define before_all(block)   self._add_spec_hook(cxxspec::Spec::HOOK_BEFORE, [] () { block });
    #define after_all(block)    self._add_spec_hook(cxxspec::Spec::HOOK_AFTER , [] () { block });

    #define before_each(block)  self._add_example_hook(cxxspec::Spec::HOOK_BEFORE, [] (cxxspec::Example& example) { block });
    #define after_each(block)   self._add_example_hook(cxxspec::Spec::HOOK_AFTER , [] (cxxspec::Example& example) { block });

    #define $ [] (cxxspec::Spec& self) -> void
    #define _ [] (cxxspec::Example& self) -> void

    #define CXXSPEC_MAIN    \
        int main(int argc, char** argv) { cxxspec::runSpecs(--argc, ++argv); return 0; }

    #define DEFINE_SPEC(name)       void __initSpec_##name();
    #define ALLOW_SPEC(name)        friend void ::__initSpec_##name();

}