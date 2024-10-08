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

#pragma once

#include <vector>
#include <functional>
#include <string>
#include <sstream>
#include <unordered_map> // for std::pair

#include "./formatter.hpp"
#include "./util.hpp"
#include "./exceptions.hpp"

namespace cxxspec {

    // -- forward declaration --
    template<typename T_got>
    class Expectation;
    // -------------------------

    class DescribeAble {
    public:
        virtual std::string fulldesc() const = 0;
        virtual std::string desc() const = 0;
    };

    class Example {
    public:
        typedef std::function<void (Example&)> Block;
        typedef std::function<void()> ExBlock;
        typedef std::function<void()> CleanupBlock;

        Example(std::string name, Block block, DescribeAble* parent)
            : _name(name), block(block), parent(parent)
        {}

        Example(std::string name, std::string sourcefile, Block block, DescribeAble* parent)
            : _name(name), _sourcefile(sourcefile), block(block), parent(parent)
        {}

        void run(Formatter& formatter, bool hasNextExample);

        std::string fullname() const {
            return this->parent->fulldesc() + " " + this->_name;
        }

        std::string name() const {
            return this->_name;
        }

        std::string sourcefile() const {
            return this->_sourcefile;
        }

        template<typename T>
        typename std::enable_if<std::is_class<T>::value, T*>::type
        cleanup(T* ptr) {
            this->cleanup([=] () {
                delete ptr;
            });
            return ptr;
        }

        template<typename T>
        typename std::enable_if<!std::is_class<T>::value, T*>::type
        cleanup(T* ptr) {
            this->cleanup([=] () {
                free(ptr);
            });
            return ptr;
        }

        void cleanup(CleanupBlock cleanupblock) {
            cleanupBlocks.push_back(cleanupblock);
        }

        template<typename T>
        Expectation<T> expect(const T& value) {
            return Expectation<T>(value);
        }

        template<typename T>
        void expect_throw(ExBlock block) {
            try {
                block();
            }
            catch (T ex) {
                // we've expected that!
                return;
            }
            catch (...) {
                std::stringstream ss;
                ss << "Expected to throw a " << util::demangle(typeid(T).name()) << ", but did throw a " << util::current_exception_typename() << " instead";
                throw ExpectFailError(ss.str());
            }

            std::stringstream ss;
            ss << "Expected to throw a " << util::demangle(typeid(T).name()) << ", but didn't";
            throw ExpectFailError(ss.str());
        }

        void expect_no_throw(ExBlock block);

    private:
        std::string _name;
        std::string _sourcefile = "unknown";
        Block block;
        std::vector<CleanupBlock> cleanupBlocks;
        DescribeAble* parent;
    };

    class Spec : public DescribeAble {
    public:
        typedef std::function<void (Spec&)> Block;
        typedef std::function<void()> SpecHookBlock;
        typedef std::function<void (Example&)> ExampleHookBlock;
        enum HookType {
            HOOK_BEFORE,
            HOOK_AFTER,
        };

        Spec(std::string desc, Block block, DescribeAble* parent = nullptr)
            : _desc(desc), block(block), parent(parent)
        {}

        inline void _context(std::string name, Block block) {
            this->subspecs.push_back(Spec(name, block, this));
        }

        inline void _it(std::string name, Example::Block block) {
            this->examples.push_back(Example(name, block, this));
        }

        inline void _it(std::string name, std::string sourcefile, Example::Block block) {
            this->examples.push_back(Example(name, sourcefile, block, this));
        }

        inline void _it(const char* name, Example::Block block) {
            this->_it(std::string(name), block);
        }

        inline void _it(const char* name, const char* sourcefile, Example::Block block) {
            this->_it(std::string(name), std::string(sourcefile), block);
        }

        inline void _add_spec_hook(HookType type, SpecHookBlock block) {
            this->spec_hooks.push_back(std::make_pair(type, block));
        }

        inline void _add_example_hook(HookType type, ExampleHookBlock block) {
            this->example_hooks.push_back(std::make_pair(type, block));
        }

        void run_spec_hooks(HookType type);

        void run_example_hooks(HookType type, Example& ex);

        void defineChilds();

        void run(Formatter& formatter, bool hasNextSpec);

        void runMarkedOnly(Formatter& formatter, bool hasNextSpec);

        std::vector<Spec>& getSubSpecs() {
            return this->subspecs;
        }

        std::string fulldesc() const {
            if (this->parent == nullptr) {
                return this->_desc;
            }
            return this->parent->fulldesc() + " " + this->_desc;
        }

        std::string desc() const {
            return this->_desc;
        }

        int getRuns() const {
            return this->runs;
        }

        bool isMarked() const {
            return this->marked;
        }

        void mark() {
            this->marked = true;
        }

        bool hasMarkedSubSpecs() const {
            return this->markedSubSpecs;
        }

        void markAsHasMarkedSubSpecs() {
            this->markedSubSpecs = true;
        }

    private:
        std::string _desc;
        Block block;
        int runs = 0;
        bool marked = false; bool markedSubSpecs = false;
        bool defined = false;

        std::vector<Spec> subspecs;
        std::vector<Example> examples;
        std::vector<std::pair<HookType, SpecHookBlock>> spec_hooks;
        std::vector<std::pair<HookType, ExampleHookBlock>> example_hooks;

        DescribeAble* parent;
    };

}
