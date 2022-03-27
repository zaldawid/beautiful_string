
#include <iostream>

#include "bstring.h"

#include <cstring>

int main() {
        bt::string str{"Alice ❤ bob! "};

        auto moved = std::move(str);

        std::cout << moved.data() << '\n';

        auto copy = moved;

        std::cout << copy.data() << '\n';

        const char* s = "महसुस";
        std::cout << s << ": " << std::strlen(s) << '\n';

    if constexpr (std::endian::native == std::endian::big)
        std::cout << "big-endian\n";
    else if constexpr (std::endian::native == std::endian::little)
        std::cout << "little-endian\n";
    else std::cout << "mixed-endian\n";
}
