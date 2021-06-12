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