#pragma once
#include "graphics/Camera.hpp"
#include <GLFW/glfw3.h>
#include <array>

class Scene;
class UiPanel;
class Application final {
public:
    Application();
    ~Application();
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    void run();
private:
    GLFWwindow* window_ = nullptr;
    bool glfwReady_ = false;
    Camera camera_;
    bool dragging_ = false;
    double lastX_ = 0.0;
    double lastY_ = 0.0;
    std::array<bool, GLFW_KEY_LAST + 1> heldKeys_{};
    void runScene();
    void processInput(float deltaTime, Scene& scene, UiPanel& ui);
    bool pressedOnce(int key);
    static void cursorCallback(GLFWwindow* window, double x, double y);
    static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
};
