#pragma once

#include "../core/core.hpp"
#include "./text_formatter.hpp"

#include <ostream>

namespace cxxspec {
    class JsonFormatter : public TextFormatter {
    private:
        bool pretty = true;
        const char* endl;

    protected:
        void i();

    public:

        JsonFormatter(std::ostream& stream, bool pretty = true)
            : TextFormatter(stream), pretty(pretty), endl(pretty ? "\n" : "")
        {}

        void onBeginTesting();
        void onEndTesting();

        void onEnterSpec(Spec& spec);
        void onLeaveSpec(Spec& spec, bool hasNextElement);

        void onEnterExample(Example& example);
        void onExampleResult(Example& example, bool result, std::string reason);
        void onLeaveExample(Example& example, bool hasNextElement);
    };
}