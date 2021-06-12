#include "./core/core.hpp"
#include "./core/exceptions.hpp"

namespace cxxspec {

    void Example::run(Formatter& formatter) {
        formatter.onEnterExample(*this);

        try {
            this->block(*this);
            formatter.onExampleResult(*this, true, "");
        }
        catch (ExpectFailError e) {
            formatter.onExampleResult(*this, false, e.what());
        }

        formatter.onLeaveExample(*this);

        for (CleanupBlock& block : this->cleanupBlocks) {
            block();
        }
    }

    void Example::expect_no_throw(ExBlock block) {
        try {
            block();
        }
        catch (const std::exception& e) {
            std::stringstream ss;
            ss << "Expected to not throw, but did: (" << util::demangle(typeid(e).name()) << ") => " << e.what();
            throw ExpectFailError(ss.str());
        }
        catch (const std::exception* e) {
            std::stringstream ss;
            ss << "Expected to not throw, but did: (" << util::demangle(typeid(e).name()) << ") => " << e->what();
            delete e;
            throw ExpectFailError(ss.str());
        }
        catch (const std::string& e) {
            std::stringstream ss;
            ss << "Expected to not throw, but did: \"" << e << '"';
            throw ExpectFailError(ss.str());
        }
        catch (const char* e) {
            std::stringstream ss;
            ss << "Expected to not throw, but did: \"" << e << '"';
            throw ExpectFailError(ss.str());
        }
        catch (...) {
            std::stringstream ss;
            ss << "Expected to not throw, but did (got unknown exception type)";
            throw ExpectFailError(ss.str());
        }
    }

    //--------------------------------------------------------------------------------

    void Spec::defineChilds() {
        if (this->defined) { return; }

        // this defines sub-specs and examples
        this->block(*this);
        this->defined = true;
    }

    void Spec::run(Formatter& formatter) {
        this->defineChilds();

        formatter.onEnterSpec(*this);

        for (Spec& spec : this->subspecs) {
            spec.run(formatter);
        }

        for (Example& ex : this->examples) {
            ex.run(formatter);
        }

        formatter.onLeaveSpec(*this);
        this->runs += 1;
    }

}