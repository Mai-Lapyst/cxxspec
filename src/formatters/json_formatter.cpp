#include "./json_formatter.hpp"

#include <ctime>

namespace cxxspec {

    void JsonFormatter::onBeginTesting() {
        stream << "[" << endl;
        chi(1);
    }
    void JsonFormatter::onEndTesting() {
        chi(-1);
        stream << "]" << endl;
    }

    void JsonFormatter::onEnterSpec(Spec& spec) {
        i(); stream << "{" << endl;
        chi(1);
            i(); stream << "\"type\": \"spec\"," << endl;
            i(); stream << "\"desc\": \"" << spec.desc() << "\"," << endl;
            i(); stream << "\"body\": [" << endl;
            chi(1);
    }
    void JsonFormatter::onLeaveSpec(Spec& spec, bool hasNextElement) {
            chi(-1);
            i(); stream << "]" << endl;
        chi(-1);
        i(); stream << "}" << (hasNextElement ? "," : "") << endl;
    }

    void JsonFormatter::onEnterExample(Example& example) {
        i(); stream << "{" << endl;
        chi(1);
            i(); stream << "\"type\": \"example\"," << endl;
            i(); stream << "\"name\": \"" << example.name() << "\"," << endl;
    }

    void JsonFormatter::onExampleResult(Example& example, bool result, std::string reason, ExampleDuration timeTaken) {
            i(); stream << "\"result\": " << (result ? "\"success\"" : "\"failed\"") << "," << endl;
            i(); stream << "\"reason\": \"" << reason << "\"," << endl;
            i(); stream << "\"time_ns\": " << timeTaken.count() << endl;
    }

    void JsonFormatter::onLeaveExample(Example& example, bool hasNextElement) {
        chi(-1);
        i(); stream << "}" << (hasNextElement ? "," : "") << endl;
    }

}