#include <core/game.hpp>

#include <fmt/core.h>

#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <iostream>

int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    fmt::print("Hello from \"{}\"!\n", TEMPLATE_NAME);
    try {
        core::game game{};
        return game.run();
    } catch (const std::exception &e) {
        std::cin.get();
        return EXIT_FAILURE;
    } catch (...) {
        std::cin.get();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
