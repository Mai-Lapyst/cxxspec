#pragma once

#include "./matcher.hpp"
#include "../matchers/compare.hpp"
#include "../matchers/contain.hpp"
#include "../matchers/be.hpp"
#include "../matchers/regex.hpp"

namespace cxxspec {

    template<typename T_got>
    class Expectation {
    public:
        Expectation(const T_got& got)
            : got(got)
        {}

        void to(Matcher<T_got>&& matcher) {
            matcher.is_negative = false;
            matcher.run(this->got);
        }

        void to_not(Matcher<T_got>&& matcher) {
            matcher.is_negative = true;
            matcher.run(this->got);
        }

        #define COMPARE_MATCHER(name, clazz) \
            template<typename T_expected> void to_##name(T_expected& expected_value) { clazz<T_got, T_expected>(expected_value).run(this->got); } \
            template<typename T_expected> void to_##name(T_expected&& expected_value) { clazz<T_got, T_expected>(util::unmove(expected_value)).run(this->got); } \
            template<typename T_expected> void to_not_##name(T_expected& expected_value) { \
                auto m = clazz<T_got, T_expected>(expected_value); m.is_negative = true; m.run(this->got); \
            } \
            template<typename T_expected> void to_not_##name(T_expected&& expected_value) { \
                auto m = clazz<T_got, T_expected>(util::unmove(expected_value)); m.is_negative = true; m.run(this->got); \
            }

        // ---------- eq <value> ----------

        COMPARE_MATCHER(eq, matchers::EqualMatcher);

        // ---------- neq <value> ----------

        COMPARE_MATCHER(neq, matchers::NotEqualMatcher);

        // ---------- lt <value> ----------

        COMPARE_MATCHER(lt, matchers::LowerThanMatcher);

        // ---------- gt <value> ----------

        COMPARE_MATCHER(gt, matchers::GreaterThanMatcher);

        // ---------- le <value> ----------

        COMPARE_MATCHER(le, matchers::LowerEqualMatcher);

        // ---------- ge <value> ----------

        COMPARE_MATCHER(ge, matchers::GreaterEqualMatcher);

        // ---------- contain <value> ----------

        template<typename T_expected>
        void to_contain(T_expected expected_value) {
            matchers::IncludeMatcher<T_got, T_expected>(expected_value).run(this->got);
        }

        template<typename T_expected>
        void to_not_contain(T_expected expected_value) {
            auto m = matchers::IncludeMatcher<T_got, T_expected>(expected_value);
            m.is_negative = true;
            m.run(this->got);
        }

        // ---------- be <value> ----------

        COMPARE_MATCHER(be, matchers::BeMatcher);

        // ---------- be a <class> ----------

        template<typename T_expected>
        void to_be_a() {
            matchers::BeAMatcher<T_got, T_expected>().run(this->got);
        }

        template<typename T_expected>
        void to_not_be_a() {
            auto m = matchers::BeAMatcher<T_got, T_expected>();
            m.is_negative = true;
            m.run(this->got);
        }

        // ---------- match <regex> ----------

        void to_match(std::string& regex_str) {
            matchers::RegexMatcher<T_got>(regex_str).run(this->got);
        }

        void to_match(std::string& regex_str,
                        std::regex_constants::match_flag_type match_flags,
                        std::regex_constants::syntax_option_type regex_flags = std::regex_constants::ECMAScript
        ) {
            matchers::RegexMatcher<T_got>(regex_str, match_flags, regex_flags).run(this->got);
        }

        void to_match(std::string& regex_str,
                        std::regex_constants::syntax_option_type regex_flags,
                        std::regex_constants::match_flag_type match_flags = std::regex_constants::match_default
        ) {
            matchers::RegexMatcher<T_got>(regex_str, regex_flags, match_flags).run(this->got);
        }

        void to_match(std::string&& regex_str) {
            matchers::RegexMatcher<T_got>(util::unmove(regex_str)).run(this->got);
        }

        void to_match(std::string&& regex_str,
                        std::regex_constants::match_flag_type match_flags,
                        std::regex_constants::syntax_option_type regex_flags = std::regex_constants::ECMAScript
        ) {
            matchers::RegexMatcher<T_got>(util::unmove(regex_str), match_flags, regex_flags).run(this->got);
        }

        void to_match(std::string&& regex_str,
                        std::regex_constants::syntax_option_type regex_flags,
                        std::regex_constants::match_flag_type match_flags = std::regex_constants::match_default
        ) {
            matchers::RegexMatcher<T_got>(util::unmove(regex_str), regex_flags, match_flags).run(this->got);
        }

    private:
        const T_got& got;
    };

}