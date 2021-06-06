#pragma once

#include "../core/core.hpp"
#include <ostream>

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