#pragma once

#include "../core/core.hpp"
#include "./text_formatter.hpp"

#include <ostream>

namespace cxxspec {

    class CliFormatter : public TextFormatter {
    private:
        void put_time();
        bool last_line_empty = false;

    public:
        CliFormatter(std::ostream& stream) : TextFormatter(stream) {}

        void onBeginTesting();
        void onEndTesting();

        void onEnterSpec(Spec& spec);
        void onLeaveSpec(Spec& spec);

        void onEnterExample(Example& example);
        void onExampleResult(Example& example, bool result, std::string reason);
        void onLeaveExample(Example& example);
    };

}