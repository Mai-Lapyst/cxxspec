#pragma once

#include <vector>
#include <functional>
#include <string>
#include <sstream>

#include "./formatter.hpp"
#include "./util.hpp"
#include "./exceptions.hpp"

namespace cxxspec {

    // -- forward declaration --
    template<typename T_got>
    class Expectation;
    // -------------------------

    class Example {
    public:
        typedef std::function<void (Example&)> Block;
        typedef std::function<void()> ExBlock;
        typedef std::function<void()> CleanupBlock;

        Example(std::string name, Block block)
            : _name(name), block(block)
        {}

        void run(Formatter& formatter, bool hasNextExample);

        std::string name() const {
            return this->_name;
        }

        template<typename T>
        typename std::enable_if<std::is_class<T>::value, T*>::type
        cleanup(T* ptr) {
            this->cleanup([=] () { delete ptr; });
            return ptr;
        }

        template<typename T>
        typename std::enable_if<!std::is_class<T>::value, T*>::type
        cleanup(T* ptr) {
            this->cleanup([=] () { free(ptr); });
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
                ss << "Expected to throw a " << util::demangle(typeid(T).name()) << ", but did throw something other";
                throw ExpectFailError(ss.str());
            }

            std::stringstream ss;
            ss << "Expected to throw a " << util::demangle(typeid(T).name()) << ", but didn't";
            throw ExpectFailError(ss.str());
        }

        void expect_no_throw(ExBlock block);

    private:
        std::string _name;
        Block block;
        std::vector<CleanupBlock> cleanupBlocks;
    };

    class Spec {
    public:
        typedef std::function<void (Spec&)> Block;

        Spec(std::string desc, Block block)
            : _desc(desc), block(block)
        {}

        inline void _context(std::string name, Block block) {
            this->subspecs.push_back(Spec(name, block));
        }

        inline void _it(std::string name, Example::Block block) {
            this->examples.push_back(Example(name, block));
        }

        inline void _it(const char* name, Example::Block block) {
            this->_it(std::string(name), block);
        }

        void defineChilds();

        void run(Formatter& formatter, bool hasNextSpec);

        void runMarkedOnly(Formatter& formatter, bool hasNextSpec);

        std::vector<Spec>& getSubSpecs() {
            return this->subspecs;
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
    };

}
