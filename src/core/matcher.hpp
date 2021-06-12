#pragma once

#include "./exceptions.hpp"
#include "./util.hpp"

#include <functional>
#include <string>
#include <sstream>
#include <type_traits>

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

    private:
        template< typename X = T>
        std::enable_if_t< !std::is_pointer<X>::value, bool >
        match_impl(const X& got) {
            std::stringstream ss;
            ss << "Derived classes of Matcher<T> (T = " << util::demangle(typeid(T).name()) << ") needs to implement match(const T& got)";
            throw std::runtime_error(ss.str());
        }

        template< typename X = T>
        std::enable_if_t< std::is_pointer<X>::value, bool >
        match_impl(const X& got) {
            //throw std::runtime_error("called Matcher<T>::match(const T& got) with a pointer type T");
        }

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

    template<typename T_got, typename T_expected = T_got>
    class ValueMatcher : public Matcher<T_got> {
    public:
        ValueMatcher(const ValueMatcher& matcher)
            : expected_value(matcher.expected_value), Matcher<T_got>(matcher)
        {}

        ValueMatcher(T_expected& expected_value)
            : expected_value(expected_value), Matcher<T_got>()
        {}

    protected:
        T_expected& expected_value;
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

    template<typename T_got, typename T_expected = T_got>
    class LamdaValueMatcher : public ValueMatcher<T_got, T_expected> {
    public:
        typedef std::function<bool(const T_got&, T_expected&)> MatchBlock;
        typedef std::function<std::string(bool, const T_got&, T_expected&)> ReasonBlock;

        LamdaValueMatcher(const LamdaValueMatcher& m)
            : mblock(m.mblock), rblock(m.rblock), ValueMatcher<T_got, T_expected>(m)
        {}

        LamdaValueMatcher(T_expected& expected_value, MatchBlock mblock, ReasonBlock rblock)
            : mblock(mblock), rblock(rblock), ValueMatcher<T_got, T_expected>(expected_value)
        {}

        bool match(const T_got& got) {
            return this->mblock(got, this->expected_value);
        };

        std::string reason(const T_got& got) {
            return this->rblock(this->is_negative, got, this->expected_value);
        }

    private:
        MatchBlock mblock;
        ReasonBlock rblock;
    };

    #define lvmu_do       [] (const T& got, T& expected) -> bool
    #define lvmu_reson    [] (bool is_negative, const T& got, T& expected) -> std::string

    #define lvm_do       [] (const T_got& got, T_expected& expected) -> bool
    #define lvm_reson    [] (bool is_negative, const T_got& got, T_expected& expected) -> std::string

}