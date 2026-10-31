#include "app/Application.hpp"
#include <exception>
#include <iostream>
#include <filesystem>
#include <vector>

int main(int argc, char** argv) {
    try {
        namespace fs = std::filesystem;
        std::vector<fs::path> roots;
        if (argc > 0) roots.push_back(fs::absolute(argv[0]).parent_path());
        roots.push_back(fs::current_path());
#ifdef DREAM_DESK_ASSET_ROOT
        roots.emplace_back(DREAM_DESK_ASSET_ROOT);
#endif
        bool found = false;
        for (const auto& root : roots) {
            if (fs::exists(root / "assets/shaders/basic.vert")) {
                fs::current_path(root);
                found = true;
                break;
            }
        }
        if (!found) throw std::runtime_error("Assets not found beside executable or in source directory");
        Application application;
        application.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "DreamDeveloperDesk error: " << e.what() << '\n';
        return 1;
    }
}
