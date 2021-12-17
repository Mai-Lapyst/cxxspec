#include "./core/core.hpp"
#include "./core/exceptions.hpp"

#include <algorithm>

namespace cxxspec {

    void Example::run(Formatter& formatter, bool hasNextExample) {
        formatter.onEnterExample(*this);

        try {
            this->block(*this);
            formatter.onExampleResult(*this, true, "");
        }
        catch (ExpectFailError e) {
            formatter.onExampleResult(*this, false, e.what());
        }

        formatter.onLeaveExample(*this, hasNextExample);

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

    void Spec::run(Formatter& formatter, bool hasNextSpec) {
        this->defineChilds();

        formatter.onEnterSpec(*this);

        int subspecLimit = this->subspecs.size() - 1;
        for (int i = 0; i <= subspecLimit; i++) {
            Spec& spec = this->subspecs.at(i);
            spec.run(formatter, i < subspecLimit || this->examples.size() > 0);
        }

        int exampleLimit = this->examples.size() - 1;
        for (int i = 0; i <= exampleLimit; i++) {
            Example& ex = this->examples.at(i);
            ex.run(formatter, i < exampleLimit);
        }

        formatter.onLeaveSpec(*this, hasNextSpec);
        this->runs += 1;
    }

    void Spec::runMarkedOnly(Formatter& formatter, bool hasNextSpec) {
        if (this->marked) {
            // run normaly all subspecs & examples
            this->run(formatter, hasNextSpec);
        }
        else if (this->markedSubSpecs) {
            // run runMarkedOnly on subspecs that are either isMarked() or hasMarkedSubSpecs()

            this->subspecs.erase(
                std::remove_if(this->subspecs.begin(), this->subspecs.end(), [] (Spec& spec) -> bool {
                    return !spec.isMarked() && !spec.hasMarkedSubSpecs();
                }),
                this->subspecs.end()
            );

            int subspecLimit = this->subspecs.size() - 1;
            for (int i = 0; i <= subspecLimit; i++) {
                Spec& spec = this->subspecs.at(i);
                spec.run(formatter, i < subspecLimit);
            }

        }
    }

}