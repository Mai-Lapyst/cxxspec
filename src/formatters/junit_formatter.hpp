#pragma once

#include "../core/core.hpp"
#include "./prettyable_formatter.hpp"

#include <ostream>

namespace cxxspec {
    typedef std::chrono::duration<double> JunitDuration;

    class JunitTestcase {
    private:
        std::string name;
        std::string sourcefile;
        JunitDuration timeTaken;
        bool result;
        std::string reason;

        friend class JunitFormatter;
    public:
        JunitTestcase(std::string name, std::string sourcefile, bool result, std::string reason, JunitDuration timeTaken)
            : name(name), sourcefile(sourcefile), result(result), reason(reason), timeTaken(timeTaken)
        {}
    };

    class JunitFormatter : public PrettyableFormatter {
    protected:
        std::vector<JunitTestcase> testcases;
        ExampleDuration timeSum;
        std::size_t failures = 0;

    public:
        JunitFormatter(std::ostream& stream, bool pretty = true) : PrettyableFormatter(stream, pretty) {}

        void onBeginTesting();
        void onEndTesting();

        void onEnterSpec(Spec& spec);
        void onLeaveSpec(Spec& spec, bool hasNextElement);

        void onEnterExample(Example& example);
        void onExampleResult(Example& example, bool result, std::string reason, ExampleDuration timeTaken);
        void onLeaveExample(Example& example, bool hasNextElement);
    };
}