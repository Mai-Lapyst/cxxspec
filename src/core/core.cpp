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

#include "./core/core.hpp"
#include "./core/exceptions.hpp"

#include <algorithm>
#include <chrono>

namespace cxxspec {

    void Example::run(Formatter& formatter, bool hasNextExample) {
        using std::chrono::high_resolution_clock;
        using time_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;

        formatter.onEnterExample(*this);

        time_point startPoint;
        time_point endPoint;
        try {
            startPoint = high_resolution_clock::now();
            this->block(*this);
            endPoint = high_resolution_clock::now();

            ExampleDuration diff = endPoint - startPoint;
            formatter.onExampleResult(*this, true, "", diff);
        }
        catch (ExpectFailError e) {
            endPoint = high_resolution_clock::now();

            ExampleDuration diff = endPoint - startPoint;
            formatter.onExampleResult(*this, false, e.what(), diff);
        }
        catch (const std::exception& e) {
            endPoint = high_resolution_clock::now();

            ExampleDuration diff = endPoint - startPoint;
            std::stringstream ss;
            ss << "Throwed uncatched & unexpected exception: (" << util::current_exception_typename() << ") => " << e.what();
            formatter.onExampleResult(*this, false, ss.str(), diff);
        }
        catch (const std::exception* e) {
            endPoint = high_resolution_clock::now();

            ExampleDuration diff = endPoint - startPoint;
            std::stringstream ss;
            ss << "Throwed uncatched & unexpected exception: (" << util::current_exception_typename() << ") => " << e->what();
            delete e;
            formatter.onExampleResult(*this, false, ss.str(), diff);
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
            ss << "Expected to not throw, but did (" << util::current_exception_typename() << ")";
            throw ExpectFailError(ss.str());
        }
    }

    //--------------------------------------------------------------------------------

    void Spec::run_spec_hooks(HookType type) {
        for (auto& e : this->spec_hooks) {
            if (e.first == type) {
                e.second();
            }
        }
    }

    void Spec::run_example_hooks(HookType type, Example& ex) {
        // when hook is for each, first run the hooks in the parent
        if (this->parent && dynamic_cast<Spec*>(this->parent)) {
            // TODO: this is an ugly hack, but I didn't want to extend DescribeAble with the run_hooks() method; needs to be changed
            ((Spec*)this->parent)->run_example_hooks(type, ex);
        }

        for (auto& e : this->example_hooks) {
            if (e.first == type) {
                e.second(ex);
            }
        }
    }

    void Spec::defineChilds() {
        if (this->defined) { return; }

        // this defines sub-specs and examples
        this->block(*this);
        this->defined = true;
    }

    void Spec::run(Formatter& formatter, bool hasNextSpec) {
        this->defineChilds();

        formatter.onEnterSpec(*this);
        this->run_spec_hooks(HOOK_BEFORE);

        int subspecLimit = this->subspecs.size() - 1;
        for (int i = 0; i <= subspecLimit; i++) {
            Spec& spec = this->subspecs.at(i);
            spec.run(formatter, i < subspecLimit || this->examples.size() > 0);
        }

        int exampleLimit = this->examples.size() - 1;
        for (int i = 0; i <= exampleLimit; i++) {
            Example& ex = this->examples.at(i);
            this->run_example_hooks(HOOK_BEFORE, ex);
            ex.run(formatter, i < exampleLimit);
            this->run_example_hooks(HOOK_AFTER, ex);
        }

        this->run_spec_hooks(HOOK_AFTER);
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