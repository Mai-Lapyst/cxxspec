#pragma once

#include "../core/core.hpp"
#include "./text_formatter.hpp"

#include <ostream>

namespace cxxspec {
    class PrettyableFormatter : public TextFormatter {
    protected:
        bool pretty = true;
        const char* endl;

        void i();

    public:
        PrettyableFormatter(std::ostream& stream, bool pretty = true)
            : TextFormatter(stream), pretty(pretty), endl(pretty ? "\n" : "")
        {}
    };
}