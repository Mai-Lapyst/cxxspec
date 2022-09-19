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