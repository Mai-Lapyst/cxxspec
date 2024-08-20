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
#include "./text_formatter.hpp"

#include <ostream>

namespace cxxspec {

    class CliFormatter : public TextFormatter {
    private:
        void put_time();
        bool last_line_empty = false;
        bool display_time = false;

    public:
        CliFormatter(std::ostream& stream, bool display_time) : TextFormatter(stream), display_time(display_time) {}

        void onBeginTesting();
        void onEndTesting();

        void onEnterSpec(Spec& spec);
        void onLeaveSpec(Spec& spec, bool hasNextElement);

        void onEnterExample(Example& example);
        void onExampleResult(Example& example, bool result, std::string reason, ExampleDuration timeTaken);
        void onLeaveExample(Example& example, bool hasNextElement);
    };

}