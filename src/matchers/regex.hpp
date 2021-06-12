#pragma once

#include "../core/matcher.hpp"
#include "../core/util.hpp"
#include "../core/pretty_print.hpp"

#include <regex>

namespace cxxspec {
    namespace matchers {

        template<typename T_got>
        class RegexMatcher : public Matcher<T_got> {
        private:
            std::string& regex_str;
            std::regex regex;
            std::regex_constants::match_flag_type match_flags;
            std::regex_constants::syntax_option_type regex_flags;

            bool _match(const T_got& got, std::true_type) {
                return std::regex_search( got, this->regex, this->match_flags );
            }
            bool _match(const T_got& got, std::false_type) {
                return std::regex_search( got.begin(), got.end(), this->regex, this->match_flags );
            }

        public:
            RegexMatcher(std::string& regex_str)
                : regex_str(regex_str), match_flags(std::regex_constants::match_default), regex_flags(std::regex_constants::ECMAScript)
            {
                regex = std::regex(regex_str);
            }

            RegexMatcher(std::string& regex_str,
                        std::regex_constants::match_flag_type match_flags,
                        std::regex_constants::syntax_option_type regex_flags = std::regex_constants::ECMAScript
            )
                : regex_str(regex_str), match_flags(match_flags), regex_flags(regex_flags)
            {
                regex = std::regex(regex_str, regex_flags);
            }

            RegexMatcher(std::string& regex_str,
                        std::regex_constants::syntax_option_type regex_flags,
                        std::regex_constants::match_flag_type match_flags = std::regex_constants::match_default
            )
                : regex_str(regex_str), match_flags(match_flags), regex_flags(regex_flags)
            {
                regex = std::regex(regex_str, regex_flags);
            }

            bool match(const T_got& got) {
                return this->_match(got, util::is_c_str<T_got>{});
            }

            std::string reason(const T_got& got) {
                std::stringstream ss;
                ss << "Expected " << prettyprint::inspect(got);
                if (this->is_negative)
                    ss << " not";
                ss << " to match (regex) /" << this->regex_str << "/";
                if (( this->regex_flags & std::regex_constants::icase ) > 0)
                    ss << 'i';
                ss << ", but has";
                if (!this->is_negative)
                    ss << " not";
                return ss.str();
            }
        };

    }
}