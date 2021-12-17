#include "./prettyable_formatter.hpp"

#include <ctime>

namespace cxxspec {

    void PrettyableFormatter::i() {
        if (this->pretty) {
            TextFormatter::i();
        }
    }

}