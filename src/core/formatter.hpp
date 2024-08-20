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

#include <string>
#include <chrono>

namespace cxxspec {
    typedef std::chrono::duration<int64_t, std::nano> ExampleDuration;

    // forward decl's
    class Spec;
    class Example;
    class ExpectationFailException;

    class Formatter {
    public:
        virtual ~Formatter() {}

        // called when the testing starts (before first spec)
        virtual void onBeginTesting() = 0;

        // called when the testing ends (after last spec)
        virtual void onEndTesting() = 0;

        // called when we enter a spec to be executed
        virtual void onEnterSpec(Spec& spec) = 0;
        // called before we leave a spec we have been executed
        virtual void onLeaveSpec(Spec& spec, bool hasNextElement) = 0;

        virtual void onEnterExample(Example& example) = 0;
        virtual void onExampleResult(Example& example, bool result, std::string reason, ExampleDuration timeTaken) = 0;
        virtual void onLeaveExample(Example& example, bool hasNextElement) = 0;

        //virtual void onExpectationFail(ExpectationFailException& ex) = 0;
    };

}