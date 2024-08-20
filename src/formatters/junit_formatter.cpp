/*
 * cxxspec - a TDD/BDD framework for c++ projects
 * Copyright (C) 2021-2024 Mai-Lapyst
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "./junit_formatter.hpp"

#include <ctime>
#include <algorithm>

namespace cxxspec {

    static std::string escapeString(std::string str) {
        std::string buf;
        buf.reserve(str.size());
        for (std::size_t pos = 0; pos != str.size(); pos++) {
            switch (str[pos]) {
                case '&':
                    buf.append("&amp;");
                    break;
                case '\"':
                    buf.append("&quot;");
                    break;
                case '\'':
                    buf.append("&apos;");
                    break;
                case '<':
                    buf.append("&lt;");
                    break;
                case '>':
                    buf.append("&gt;");
                    break;
                default:
                    buf.append(&str[pos], 1);
                    break;
            }
        }
        return buf;
    }

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
                std::string classname = testcase.sourcefile.substr(0, testcase.sourcefile.find_last_of("."));
                stream << " classname=\"" << classname << "\"";
                stream << " name=\"" << escapeString(testcase.name) << "\"";
                stream << " time=\"" << testcase.timeTaken.count() << "\"";
            if (testcase.result) {
                stream << "/>" << endl;
            }
            else {
                stream << ">" << endl;
                chi(1);
                    i(); stream << "<failure";
                        stream << " message=\"" << escapeString(testcase.reason) << "\"";
                        stream << " type=\"expect\"";
                        stream << ">" << endl;
                    chi(1);
                        // content of an failure element is the stacktrace
                        i(); stream << "<![CDATA[" << "]]>" << endl;
                    chi(-1);
                    i(); stream << "</failure>" << endl;
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