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

#include "./json_formatter.hpp"

#include <ctime>

namespace cxxspec {

    void JsonFormatter::onBeginTesting() {
        stream << "[" << endl;
        chi(1);
    }
    void JsonFormatter::onEndTesting() {
        chi(-1);
        stream << "]" << endl;
    }

    void JsonFormatter::onEnterSpec(Spec& spec) {
        i(); stream << "{" << endl;
        chi(1);
            i(); stream << "\"type\": \"spec\"," << endl;
            i(); stream << "\"desc\": \"" << spec.desc() << "\"," << endl;
            i(); stream << "\"body\": [" << endl;
            chi(1);
    }
    void JsonFormatter::onLeaveSpec(Spec& spec, bool hasNextElement) {
            chi(-1);
            i(); stream << "]" << endl;
        chi(-1);
        i(); stream << "}" << (hasNextElement ? "," : "") << endl;
    }

    void JsonFormatter::onEnterExample(Example& example) {
        i(); stream << "{" << endl;
        chi(1);
            i(); stream << "\"type\": \"example\"," << endl;
            i(); stream << "\"name\": \"" << example.name() << "\"," << endl;
    }

    void JsonFormatter::onExampleResult(Example& example, bool result, std::string reason, ExampleDuration timeTaken) {
            i(); stream << "\"result\": " << (result ? "\"success\"" : "\"failed\"") << "," << endl;
            i(); stream << "\"reason\": \"" << reason << "\"," << endl;
            i(); stream << "\"time_ns\": " << timeTaken.count() << endl;
    }

    void JsonFormatter::onLeaveExample(Example& example, bool hasNextElement) {
        chi(-1);
        i(); stream << "}" << (hasNextElement ? "," : "") << endl;
    }

}