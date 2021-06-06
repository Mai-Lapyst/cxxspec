#pragma once

#include <vector>
#include <functional>
#include <string>

#include "./formatter.hpp"
#include "./matcher.hpp"

namespace cxxspec {

    template<typename T>
    class Expectation {
    public:
        Expectation(const T& got, Formatter& formatter)
            : got(got), formatter(formatter)
        {}

        void to(Matcher<T>&& matcher) {
            matcher.is_negative = false;
            matcher.run(this->got);
        }

        void to_not(Matcher<T>&& matcher) {
            matcher.is_negative = true;
            matcher.run(this->got);
        }

    private:
        const T& got;
        Formatter& formatter;
    };

    class Example {
    public:
        typedef std::function<void (Example&)> Block;

        Example(std::string name, Block block)
            : _name(name), block(block), formatter( *( ( (Formatter*)nullptr ) ) )
        {}

        void run(Formatter& formatter);

        std::string name() const {
            return this->_name;
        }

        template<typename T>
        Expectation<T> expect(const T& value) {
            return Expectation<T>(value, this->formatter);
        }

    protected:
        Formatter& getFormatter() {
            return this->formatter;
        }

    private:
        std::string _name;
        Block block;
        Formatter& formatter;
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

        inline void it(std::string name, Example::Block block) {
            this->examples.push_back(Example(name, block));
        }

        inline void it(const char* name, Example::Block block) {
            this->it(std::string(name), block);
        }

        void run(Formatter& formatter);

        std::string desc() const {
            return this->_desc;
        }

    private:
        std::string _desc;
        Block block;

        std::vector<Spec> subspecs;
        std::vector<Example> examples;
    };

}