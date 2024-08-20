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

#include "../core/formatter.hpp"
#include "./fileno.hpp"

#include <ostream>
#include <iostream>
#include <unistd.h>

namespace cxxspec {

    /**
     * This is a abstract class that deals with an output stream as well as
     * indention of lines
     */
    class TextFormatter : public Formatter {
    public:
        TextFormatter(std::ostream& stream)
            : stream(stream)
        {}

        bool isAtty() {
            return isatty( fileno(this->stream) );
        }

        bool force_colors = false;
        bool useColors() {
            return this->force_colors || this->isAtty();
        }

    protected:
        std::ostream& stream;

        int indention = 0;

        /**
         * Changes indention level
         */
        void chi(int i = 0);

        /**
         * add an indention-string to the stream
         */
        void i();

    };

}