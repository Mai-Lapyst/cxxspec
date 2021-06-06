#include "./text_formatter.hpp"

#include <ctime>

namespace cxxspec {

    void TextFormatter::chi(int i) {
        indention += i;
    }

    void TextFormatter::i() {
        for (int j = 0; j < indention; j++) {
            stream << "    ";
        }
    }

}