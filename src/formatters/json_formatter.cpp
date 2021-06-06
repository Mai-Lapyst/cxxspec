#include "./json_formatter.hpp"

#include <ctime>

namespace cxxspec {

    void JsonFormatter::onBeginTesting() {
        stream << "[\n";
        chi(1);
    }
    void JsonFormatter::onEndTesting() {
        chi(-1);
        stream << "]\n";
    }

    void JsonFormatter::onEnterSpec(Spec& spec) {
        i(); stream << "{\n";
        chi(1);
            i(); stream << "\"type\": \"spec\",\n";
            i(); stream << "\"desc\": \"" << spec.desc() << "\",\n";
            i(); stream << "\"body\": [\n";
            chi(1);
    }
    void JsonFormatter::onLeaveSpec(Spec& spec) {
            chi(-1);
            i(); stream << "]\n";
        chi(-1);
        i(); stream << "}\n";
    }

    void JsonFormatter::onEnterExample(Example& example) {
        i(); stream << "{\n";
        chi(1);
            i(); stream << "\"type\": \"example\",\n";
            i(); stream << "\"name\": \"" << example.name() << "\",\n";
    }

    void JsonFormatter::onExampleResult(Example& example, bool result, std::string reason) {
            i(); stream << "\"result\": " << (result ? "\"success\"" : "\"failed\"") << ",\n";
            i(); stream << "\"reason\": \"" << reason << "\"\n";
    }

    void JsonFormatter::onLeaveExample(Example& example) {
        chi(-1);
        i(); stream << "}\n";
    }

}