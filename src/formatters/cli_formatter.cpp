#include "./cli_formatter.hpp"

#include <ctime>

namespace cxxspec {

    void CliFormatter::put_time() {
        char* timestr = (char*) malloc(sizeof(char) * 20);
        time_t rawtime;
        time(&rawtime);
        struct tm* timeinfo = localtime(&rawtime);
        strftime(timestr, 20, "%Y-%m-%d %H:%M:%S", timeinfo);
        timestr[19] = '\0';
        this->stream << timestr;
        free(timestr);
    }

    void CliFormatter::onBeginTesting() {
        stream << "Start testing "; put_time(); stream << "\n";
        stream << "========================================\n";
    }
    void CliFormatter::onEndTesting() {
        stream << "========================================\n";
        stream << "End testing "; put_time(); stream << "\n";
    }

    void CliFormatter::onEnterSpec(Spec& spec) {
        last_line_empty = false;

        i(); stream << spec.desc() << "\n";
        chi(1);
    }
    void CliFormatter::onLeaveSpec(Spec& spec) {
        chi(-1);
        if (!last_line_empty) {
            stream << "\n";
            last_line_empty = true;
        }
    }

    void CliFormatter::onEnterExample(Example& example) {}
    void CliFormatter::onExampleResult(Example& example, bool result, std::string reason) {
        last_line_empty = false;

        if (result) {
            i();
            if (this->useColors())
                stream << "\e[32m";
            stream << example.name() << "\n";
        }
        else {
            i();
            if (this->useColors())
                stream << "\e[31m";
            stream << example.name() << "\n";
            chi(1);
                i(); stream << reason << "\n";
            chi(-1);
        }
        if (this->useColors())
            stream << "\e[0m";
    }
    void CliFormatter::onLeaveExample(Example& example) {}

}