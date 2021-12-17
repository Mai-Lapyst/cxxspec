#pragma once

#include <string>

namespace cxxspec {

    // forward decl's
    class Spec;
    class Example;
    class ExpectationFailException;

    class Formatter {
    public:
        virtual ~Formatter() {}

        // called when the testing starts (before first spec)
        virtual void onBeginTesting() = 0;

        // called when the testing ends (after last spec)
        virtual void onEndTesting() = 0;

        // called when we enter a spec to be executed
        virtual void onEnterSpec(Spec& spec) = 0;
        // called before we leave a spec we have been executed
        virtual void onLeaveSpec(Spec& spec, bool hasNextElement) = 0;

        virtual void onEnterExample(Example& example) = 0;
        virtual void onExampleResult(Example& example, bool result, std::string reason) = 0;
        virtual void onLeaveExample(Example& example, bool hasNextElement) = 0;

        //virtual void onExpectationFail(ExpectationFailException& ex) = 0;
    };

}