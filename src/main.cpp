#include "app/Application.hpp"
#include <exception>
#include <iostream>

int main() {
    try {
        Application application;
        application.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "DreamDeveloperDesk error: " << e.what() << '\n';
        return 1;
    }
}
