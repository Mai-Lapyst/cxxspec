#pragma once

#include "../core/core.hpp"
#include "./prettyable_formatter.hpp"

#include <ostream>

namespace cxxspec {
    class JsonFormatter : public PrettyableFormatter {
    public:

        JsonFormatter(std::ostream& stream, bool pretty = true) : PrettyableFormatter(stream, pretty) {}

        void onBeginTesting();
        void onEndTesting();

        void onEnterSpec(Spec& spec);
        void onLeaveSpec(Spec& spec, bool hasNextElement);

        void onEnterExample(Example& example);
        void onExampleResult(Example& example, bool result, std::string reason);
        void onLeaveExample(Example& example, bool hasNextElement);
    };
}