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
    void CliFormatter::onLeaveSpec(Spec& spec, bool hasNextElement) {
        chi(-1);
        if (!last_line_empty) {
            stream << "\n";
            last_line_empty = true;
        }
    }

    typedef std::chrono::duration<double> SecondsDuration;

    void CliFormatter::onEnterExample(Example& example) {}
    void CliFormatter::onExampleResult(Example& example, bool result, std::string reason, ExampleDuration timeTaken) {
        last_line_empty = false;

        auto old_precision = stream.precision();
        stream.precision(9);
        stream << std::fixed;

        SecondsDuration secs = std::chrono::duration_cast<SecondsDuration>(timeTaken);

        if (result) {
            i();
            if (this->useColors())
                stream << "\e[32m";
            stream << example.name();
            if (this->display_time) {
                stream << " (took: " << secs.count() << "s)";
            }
            stream << "\n";
        }
        else {
            i();
            if (this->useColors())
                stream << "\e[31m";
            stream << example.name();
            if (this->display_time) {
                stream << " (took: " << secs.count() << "s)";
            }
            stream << "\n";
            chi(1);
                i(); stream << reason << "\n";
            chi(-1);
        }
        if (this->useColors())
            stream << "\e[0m";

        stream.precision(old_precision);
        stream << std::defaultfloat;
    }
    void CliFormatter::onLeaveExample(Example& example, bool hasNextElement) {}

}