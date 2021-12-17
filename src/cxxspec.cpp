#include "./cxxspec.hpp"
#include "./formatters/cli_formatter.hpp"
#include "./formatters/json_formatter.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

namespace cxxspec {

    std::vector<Spec> all_specs = std::vector<Spec>();

    void runAllSpecs(Formatter& formatter) {
        formatter.onBeginTesting();
        for (Spec& spec : all_specs) {
            spec.run(formatter);
        }
        formatter.onEndTesting();
    }

    void runSpecs(int argc, char** argv) {
        if (argc <= 0) {
            CliFormatter formatter(std::cout);
            runAllSpecs(formatter);
            return;
        }

        std::vector<std::string> args;
        for (int i = 0; i < argc; i++) {
            args.push_back(std::string(argv[i]));
        }

        runSpecs(args);
    }

    void runSpec(Formatter& formatter, std::string specpath, std::vector<Spec>& list, std::size_t off = 0UL) {
        //std::cout << "runSpec(..., " << specpath << ", [..])\n";

        std::size_t pos = specpath.find_first_of('/', off);
        std::string name = specpath.substr(off, pos);

        auto iter = std::find_if(list.begin(), list.end(), [&name] (Spec& spec) -> bool {
            return spec.desc().compare(name) == 0;
        });

        if (iter == list.end()) {
            throw std::runtime_error("Could not find spec '" + specpath + "'");
        }

        if (pos != std::string::npos) {
            // define all subspecs so we can search them!
            (*iter).defineChilds();

            std::vector<Spec>& sublist = (*iter).getSubSpecs();
            runSpec(formatter, specpath, sublist, pos + 1);
        }
        else {
            if ((*iter).getRuns() <= 0) {
                (*iter).run(formatter);
            }
        }
    }

    enum FormatterType {
        FT_CLI, FT_JSON
    };

    void runSpecs(std::vector<std::string>& arguments) {
        #define CONSUME_ARG \
            i++; arg = arguments[i];

        std::string output_file = "-";
        FormatterType formatter_type = FT_CLI;
        bool force_colors = false;

        std::vector<std::string> to_run;
        try {

            for (int i = 0; i < arguments.size(); i++) {
                std::string arg = arguments[i];

                if (arg[0] == '-') {
                    if (arg == "--format") {
                        CONSUME_ARG;

                        if (arg == "cli") {
                            formatter_type = FT_CLI;
                        }
                        else if (arg == "json") {
                            formatter_type = FT_JSON;
                        }
                        else {
                            throw std::runtime_error("Unknown formatter: " + arg);
                        }

                        continue;
                    }
                    else if (arg == "-f") {
                        CONSUME_ARG;
                        output_file = arg;
                        continue;
                    }
                    else if (arg == "--force-colors") {
                        force_colors = true;
                        continue;
                    }
                    else if (arg == "-h" || arg == "--help") {
                        puts("Usage: specs [<options>] <specs to run>");
                        puts("Available options:");
                        puts("  -h, --help          Displays this help");
                        puts("  -f <output>         Writes output instead of stdout to the specified file.");
                        puts("  --force-colors      Forces colorized output, even when writing to file");
                        puts("  --format <format>   Outputs in the given format. Available: cli, json");
                        exit(1);
                    }
                    else {
                        throw std::runtime_error("Unknown option: " + arg);
                    }
                }
                else {
                    // seems to be a spec-path
                    to_run.push_back(arg);
                }
            }

        } catch (std::runtime_error e) {
            std::cout << e.what() << '\n';
            std::exit(1);
        }

        bool is_outfile_cout = (output_file == "-");
        std::ostream* stream = is_outfile_cout ? (&std::cout) : new std::ofstream(output_file);

        Formatter* formatter = nullptr;
        switch (formatter_type) {
            case FT_CLI:
                formatter = new CliFormatter(*stream);
                ((TextFormatter*)formatter)->force_colors = force_colors;
                break;

            case FT_JSON:
                formatter = new JsonFormatter(*stream);
                ((TextFormatter*)formatter)->force_colors = force_colors;
                break;
        }

        if (to_run.size() <= 0) {
            runAllSpecs(*formatter);
        }
        else {
            // try to find specs to run...
            try {
                std::sort(to_run.begin(), to_run.end());
                for (std::string& specpath : to_run) {
                    runSpec(*formatter, specpath, all_specs);
                }
            }
            catch (std::runtime_error e) {
                std::cout << e.what() << '\n';
                stream->flush();
                if (!is_outfile_cout)
                    delete stream;
                delete formatter;
                std::exit(1);
            }
        }

        stream->flush();
        if (!is_outfile_cout)
            delete stream;
        delete formatter;
    }

}

