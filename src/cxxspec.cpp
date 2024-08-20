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

#include "./cxxspec.hpp"
#include "./formatters/cli_formatter.hpp"
#include "./formatters/json_formatter.hpp"
#include "./formatters/junit_formatter.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

namespace cxxspec {

    const char* getVersion(int* major, int* minor, int* patch) {
        if (major != nullptr) { *major = CXXSPEC_VERSION_MAJOR; }
        if (minor != nullptr) { *minor = CXXSPEC_VERSION_MINOR; }
        if (patch != nullptr) { *patch = CXXSPEC_VERSION_PATCH; }
        return CXXSPEC_VERSION;
    }

    std::vector<Spec> all_specs = std::vector<Spec>();

    void runAllSpecs(Formatter& formatter, bool onlyMarked) {
        formatter.onBeginTesting();

        if (onlyMarked) {
            all_specs.erase(
                std::remove_if(all_specs.begin(), all_specs.end(), [] (Spec& spec) -> bool {
                    return !spec.isMarked() && !spec.hasMarkedSubSpecs();
                }),
                all_specs.end()
            );
        }

        int specLimit = all_specs.size() - 1;
        for (int i = 0; i <= specLimit; i++) {
            Spec& spec = all_specs.at(i);
            if (!onlyMarked) {
                spec.run(formatter, i < specLimit);
            }
            else {
                spec.runMarkedOnly(formatter, i < specLimit);
            }
        }

        formatter.onEndTesting();
    }

    void runSpecs(int argc, char** argv) {
        if (argc <= 0) {
            CliFormatter formatter(std::cout, false);
            runAllSpecs(formatter);
            return;
        }

        std::vector<std::string> args;
        for (int i = 0; i < argc; i++) {
            args.push_back(std::string(argv[i]));
        }

        runSpecs(args);
    }

    bool markSpec(Formatter& formatter, std::string specpath, std::vector<Spec>& list, std::size_t off = 0UL) {

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
            if (markSpec(formatter, specpath, sublist, pos + 1)) {
                (*iter).markAsHasMarkedSubSpecs();
            }
        }
        else {
            (*iter).mark();
            return true;
            // if ((*iter).getRuns() <= 0) {
            //     (*iter).run(formatter);
            // }
        }
        return false;
    }

    enum FormatterType {
        FT_CLI, FT_JSON, FT_JUNIT
    };

    void runSpecs(std::vector<std::string>& arguments) {
        #define CONSUME_ARG \
            i++; arg = arguments[i];

        std::string output_file = "-";
        FormatterType formatter_type = FT_CLI;
        bool force_colors = false;
        bool pretty_print = true;
        bool display_time = false;

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
                        else if (arg == "junit") {
                            formatter_type = FT_JUNIT;
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
                        puts("  -f <output>         Writes output to the specified file instead of the standard output.");
                        puts("  --force-colors      Forces colorized output, even when writing to file");
                        puts("  --format <format>   Outputs in the given format. Available: cli, json, junit");
                        puts("  -j, --json          Equivalent to --format json");
                        puts("  -c, --compact       Disables pretty printing of output for some formats.");
                        puts("                      Supported by: json");
                        puts("  -t, --time          Displays time taken when using the cli format");
                        exit(1);
                    }
                    else if (arg == "-j" || arg == "--json") {
                        formatter_type = FT_JSON;
                        continue;
                    }
                    else if (arg == "-c" || arg == "--compact") {
                        pretty_print = false;
                        continue;
                    }
                    else if (arg == "-t" || arg == "--time") {
                        display_time = true;
                        continue;
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
                formatter = new CliFormatter(*stream, display_time);
                ((TextFormatter*)formatter)->force_colors = force_colors;
                break;

            case FT_JSON:
                formatter = new JsonFormatter(*stream, pretty_print);
                ((TextFormatter*)formatter)->force_colors = force_colors;
                break;

            case FT_JUNIT:
                formatter = new JunitFormatter(*stream, pretty_print);
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
                int specPathLimit = to_run.size() - 1;
                for (int i = 0; i <= specPathLimit; i++) {
                    std::string& specpath = to_run.at(i);
                    markSpec(*formatter, specpath, all_specs);
                }
                runAllSpecs(*formatter, true);
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

