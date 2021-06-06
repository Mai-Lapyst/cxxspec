#pragma once

#include <vector>

#include "./core/core.hpp"
#include "./core/formatter.hpp"
#include "./core/exceptions.hpp"

namespace cxxspec {

    extern std::vector<Spec> all_specs;
    void runAllSpecs(Formatter& formatter);

    // TODO: this only works on gcc; for alternatives see https://stackoverflow.com/questions/1113409/attribute-constructor-equivalent-in-vc
    #define cxxspec_autoload    __attribute__((constructor))

    #define describe(name, block)       \
        cxxspec_autoload                \
        void __initSpec_##name () {     \
            cxxspec::all_specs.push_back( cxxspec::Spec(#name, block) ); \
        }

    #define explain(name, block)    self._context(name, block)
    #define context(name, block)    self._context(name, block)

    #define it(text, block)         self.it(text, block)

    #define expect(what)            self.expect(what)

    #define not_to(param)           to_not(param)

    #define $ [] (cxxspec::Spec& self) -> void
    #define _ [] (cxxspec::Example& self) -> void

    #define DEFINE_SPEC(name)       void __initSpec_##name();
    #define ALLOW_SPEC(name)        friend void ::__initSpec_##name();

}