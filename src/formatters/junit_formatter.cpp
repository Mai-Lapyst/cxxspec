#include "./junit_formatter.hpp"

#include <ctime>

namespace cxxspec {

    void JunitFormatter::onBeginTesting() {}
    void JunitFormatter::onEndTesting() {
        auto old_precision = stream.precision();
        stream.precision(9);
        stream << std::fixed;

        stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl;
        stream << "<testsuite";
            stream << " errors=\"0\"";
            stream << " failures=\"" << this->failures << "\"";
            stream << " skipped=\"0\"";
            stream << " tests=\"" << this->testcases.size() << "\"";
            JunitDuration time = std::chrono::duration_cast<JunitDuration>(this->timeSum);
            stream << " time=\"" << time.count() << "\"";
        stream << ">" << endl;
        chi(1);

        for (JunitTestcase& testcase : this->testcases) {
            i(); stream << "<testcase";
                stream << " classname=\"" << testcase.sourcefile << "\"";
                stream << " name=\"" << testcase.name << "\"";
                stream << " time=\"" << testcase.timeTaken.count() << "\"";
            if (testcase.result) {
                stream << "/>" << endl;
            }
            else {
                stream << ">" << endl;
                chi(1);
                    i(); stream << "<failure";
                        stream << " message=\"" << testcase.reason << "\"";
                        stream << " type=\"expect\"";
                        stream << ">" << endl;
                chi(-1);
                i(); stream << "</testcase>" << endl;
            }
        }

        chi(-1);
        stream << "</testsuite>" << endl;

        stream.precision(old_precision);
        stream << std::defaultfloat;
    }

    void JunitFormatter::onEnterSpec(Spec& spec) {}
    void JunitFormatter::onLeaveSpec(Spec& spec, bool hasNextElement) {}

    void JunitFormatter::onEnterExample(Example& example) {}
    void JunitFormatter::onExampleResult(Example& example, bool result, std::string reason, ExampleDuration timeTaken) {
        JunitDuration time = std::chrono::duration_cast<JunitDuration>(timeTaken);
        testcases.push_back(JunitTestcase(example.fullname(), example.sourcefile(), result, reason, time));
        timeSum += timeTaken;
        if (!result) {
            failures++;
        }
    }
    void JunitFormatter::onLeaveExample(Example& example, bool hasNextElement) {}

}