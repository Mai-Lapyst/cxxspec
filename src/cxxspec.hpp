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

    #define it  self._it

    #define expect      self.expect
    #define cleanup     self.cleanup

    #define expect_throw(type, block)   self.expect_throw<type>(block);
    #define expect_no_throw             self.expect_no_throw

    #define $ [] (cxxspec::Spec& self) -> void
    #define _ [] (cxxspec::Example& self) -> void

    #define CXXSPEC_MAIN    \
        int main(int argc, char** argv) { cxxspec::runSpecs(--argc, ++argv); return 0; }

    #define DEFINE_SPEC(name)       void __initSpec_##name();
    #define ALLOW_SPEC(name)        friend void ::__initSpec_##name();

}