#include "./core/core.hpp"
#include "./core/exceptions.hpp"

namespace cxxspec {

        void Example::run(Formatter& formatter) {
        this->formatter = formatter;

        formatter.onEnterExample(*this);

        try {
            this->block(*this);
            formatter.onExampleResult(*this, true, "");
        }
        catch (ExpectFailError e) {
            formatter.onExampleResult(*this, false, e.what());
        }

        formatter.onLeaveExample(*this);
    }

    //--------------------------------------------------------------------------------

    void Spec::run(Formatter& formatter) {
        formatter.onEnterSpec(*this);

        // this defines sub-specs and examples
        this->block(*this);

        for (Spec& spec : this->subspecs) {
            spec.run(formatter);
        }

        for (Example& ex : this->examples) {
            ex.run(formatter);
        }

        formatter.onLeaveSpec(*this);
    }

}