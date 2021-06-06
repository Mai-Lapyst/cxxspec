#pragma once

#include "./exceptions.hpp"

#include <functional>
#include <string>

namespace cxxspec {

    template<typename T>
    class Expectation;

    template<typename T>
    class Matcher {
    public:
        Matcher(const Matcher& matcher)
            : is_negative(matcher.is_negative)
        {}

        Matcher() {}

        virtual ~Matcher() {}

        virtual bool match(const T& got) = 0;
        void run(const T& got);

        virtual std::string reason(const T& got) = 0;

    protected:
        bool is_negative = false;

        friend class Expectation<T>;
    };

    template<typename T>
    void Matcher<T>::run(const T& got) {
        if (!this->is_negative) {
            if (this->match(got)) {
                return;
            }
            throw ExpectFailError( this->reason(got) );
        }
        else {
            if (!this->match(got)) {
                return;
            }
            throw ExpectFailError( this->reason(got) );
        }
    }

    template<typename T>
    class ValueMatcher : public Matcher<T> {
    public:
        ValueMatcher(const ValueMatcher& matcher)
            : expected_value(matcher.expected_value), Matcher<T>(matcher)
        {}

        ValueMatcher(T& expected_value)
            : expected_value(expected_value), Matcher<T>()
        {}

    protected:
        T& expected_value;
    };

    template<typename T>
    class LamdaMatcher : public Matcher<T> {
    public:
        typedef std::function<bool(const T&)> MatchBlock;
        typedef std::function<std::string(bool, const T&)> ReasonBlock;

        LamdaMatcher(const LamdaMatcher& m)
            : mblock(m.mblock), rblock(m.rblock), Matcher<T>(m)
        {}

        LamdaMatcher(MatchBlock mblock, ReasonBlock rblock)
            : mblock(mblock), rblock(rblock), Matcher<T>()
        {}

        bool match(const T& got) {
            return this->mblock(got);
        };

        std::string reason(const T& got) {
            return this->rblock(this->is_negative, got);
        }

    private:
        MatchBlock mblock;
        ReasonBlock rblock;
    };

    #define lm_do       [=] (T& got) -> bool
    #define lm_reson    [=] (bool is_negative, T& got) -> std::string

    template<typename T>
    class LamdaValueMatcher : public ValueMatcher<T> {
    public:
        typedef std::function<bool(const T&, T&)> MatchBlock;
        typedef std::function<std::string(bool, const T&, T&)> ReasonBlock;

        LamdaValueMatcher(const LamdaValueMatcher& m)
            : mblock(m.mblock), rblock(m.rblock), ValueMatcher<T>(m)
        {}

        LamdaValueMatcher(T& expected_value, MatchBlock mblock, ReasonBlock rblock)
            : mblock(mblock), rblock(rblock), ValueMatcher<T>(expected_value)
        {}

        bool match(const T& got) {
            return this->mblock(got, this->expected_value);
        };

        std::string reason(const T& got) {
            return this->rblock(this->is_negative, got, this->expected_value);
        }

    private:
        MatchBlock mblock;
        ReasonBlock rblock;
    };

    #define lvm_do       [] (const T& got, T& expected) -> bool
    #define lvm_reson    [] (bool is_negative, const T& got, T& expected) -> std::string

}