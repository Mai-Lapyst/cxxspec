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

#include "../core/core.hpp"
#include "./prettyable_formatter.hpp"

#include <ostream>

namespace cxxspec {
    typedef std::chrono::duration<double> JunitDuration;

    class JunitTestcase {
    private:
        std::string name;
        std::string sourcefile;
        JunitDuration timeTaken;
        bool result;
        std::string reason;

        friend class JunitFormatter;
    public:
        JunitTestcase(std::string name, std::string sourcefile, bool result, std::string reason, JunitDuration timeTaken)
            : name(name), sourcefile(sourcefile), result(result), reason(reason), timeTaken(timeTaken)
        {}
    };

    class JunitFormatter : public PrettyableFormatter {
    protected:
        std::vector<JunitTestcase> testcases;
        ExampleDuration timeSum;
        std::size_t failures = 0;

    public:
        JunitFormatter(std::ostream& stream, bool pretty = true) : PrettyableFormatter(stream, pretty) {}

        void onBeginTesting();
        void onEndTesting();

        void onEnterSpec(Spec& spec);
        void onLeaveSpec(Spec& spec, bool hasNextElement);

        void onEnterExample(Example& example);
        void onExampleResult(Example& example, bool result, std::string reason, ExampleDuration timeTaken);
        void onLeaveExample(Example& example, bool hasNextElement);
    };
}