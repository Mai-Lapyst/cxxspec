#include "./cxxspec.hpp"

namespace cxxspec {

    std::vector<Spec> all_specs = std::vector<Spec>();
    void runAllSpecs(Formatter& formatter) {
        formatter.onBeginTesting();
        for (Spec& spec : all_specs) {
            spec.run(formatter);
        }
        formatter.onEndTesting();
    }

}

