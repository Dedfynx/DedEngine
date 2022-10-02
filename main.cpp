#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "App.hpp"

int main() {
    DedOs::App app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}