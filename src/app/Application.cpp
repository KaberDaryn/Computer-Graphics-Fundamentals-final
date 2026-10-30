#include "app/Application.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Framebuffer.hpp"
#include "scene/Lighting.hpp"
#include "scene/Scene.hpp"
#include "ui/UiPanel.hpp"
#include <glad/glad.h>
#include <algorithm>
#include <cstddef>
#include <glm/gtc/matrix_transform.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace {
void errorCallback(int error, const char* message) {
    std::cerr << "GLFW error " << error << ": " << message << '\n';
}

// Draw all world geometry for either camera. The live-screen surface itself
// is deliberately excluded here: only the final onscreen pass samples the FBO.
void renderWorld(const Scene& scene, const Shader& lit, const Shader& rgb,
                 const Shader& sky, const glm::mat4& view,
                 const glm::mat4& projection, const glm::vec3& eye, float time) {
    lit.use();
    lit.setMat4("uView", view);
    lit.setMat4("uProjection", projection);
    lit.setVec3("uViewPosition", eye);
    Lighting::upload(lit, scene, time);
    scene.draw(lit, RenderPass::Lit);

    rgb.use();
    rgb.setMat4("uView", view);
    rgb.setMat4("uProjection", projection);
    rgb.setFloat("uTime", time);
    rgb.setFloat("uRgbSpeed", scene.rgbSpeed());
    rgb.setInt("uRgbMode", scene.rgbMode());
    rgb.setFloat("uRgbIntensity", scene.rgbBoost());
    rgb.setVec3("uRgbStaticColor", scene.rgbColor());
    scene.draw(rgb, RenderPass::RGB);

    sky.use();
    sky.setMat4("uView", view);
    sky.setMat4("uProjection", projection);
    sky.setFloat("uDayFactor", scene.dayFactor());
    sky.setFloat("uTime", time);
    scene.draw(sky, RenderPass::Sky);
}

void clearScene(float daylight) {
    const glm::vec3 night(0.009f, 0.017f, 0.044f);
    const glm::vec3 day(0.22f, 0.35f, 0.54f);
    const glm::vec3 c = glm::mix(night, day, daylight);
    glClearColor(c.r, c.g, c.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
}
Application::Application() {
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit()) throw std::runtime_error("GLFW initialization failed");
    glfwReady_ = true;
    try {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
        glfwWindowHint(GLFW_SAMPLES, 4);
        window_ = glfwCreateWindow(1280, 720,
                      "Dream Developer Desk", nullptr, nullptr);
        if (!window_) throw std::runtime_error("Cannot create OpenGL 3.3 context");
        glfwMakeContextCurrent(window_);
        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
            throw std::runtime_error("GLAD initialization failed");
        if (!GLAD_GL_VERSION_3_3)
            throw std::runtime_error("OpenGL 3.3 Core required");
        glfwSwapInterval(1);
        glfwSetWindowUserPointer(window_, this);
        glfwSetCursorPosCallback(window_, cursorCallback);
        glfwSetScrollCallback(window_, scrollCallback);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_MULTISAMPLE);
        std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';
    } catch (...) {
        if (window_) glfwDestroyWindow(window_);
        window_ = nullptr;
        glfwTerminate();
        glfwReady_ = false;
        throw;
    }
}
Application::~Application() {
    if (window_) glfwDestroyWindow(window_);
    if (glfwReady_) glfwTerminate();
}
void Application::cursorCallback(GLFWwindow* window, double x, double y) {
    auto* self = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (!self) return;
    bool down = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    if (UiPanel::wantsMouse()) down = false;
    if (down && self->dragging_)
        self->camera_.orbit(float((x - self->lastX_) * 0.22),
                            float((self->lastY_ - y) * 0.22));
    self->dragging_ = down;
    self->lastX_ = x;
    self->lastY_ = y;
}
void Application::scrollCallback(GLFWwindow* window, double, double dy) {
    auto* self = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (self && !UiPanel::wantsMouse()) self->camera_.zoom(float(dy));
}
bool Application::pressedOnce(int key) {
    const bool down = glfwGetKey(window_, key) == GLFW_PRESS;
    const bool firstPress = down && !heldKeys_[static_cast<size_t>(key)];
    heldKeys_[static_cast<size_t>(key)] = down;
    return firstPress;
}
void Application::processInput(float dt, Scene& scene, UiPanel& ui) {
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window_, true);
    // Always sample key edges, even when a UI text field captures input.
    const bool toggleLamp = pressedOnce(GLFW_KEY_L);
    const bool changeMonitors = pressedOnce(GLFW_KEY_M);
    const bool changeRgb = pressedOnce(GLFW_KEY_R);
    const bool toggleHelp = pressedOnce(GLFW_KEY_H);
    const bool toggleDay = pressedOnce(GLFW_KEY_N);
    const bool toggleFeed = pressedOnce(GLFW_KEY_P);
    const bool toggleFinale = pressedOnce(GLFW_KEY_F);
    if (UiPanel::wantsKeyboard()) return;

    float angle = 80.0f * dt;
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) camera_.orbit(-angle, 0);
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) camera_.orbit(angle, 0);
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) camera_.orbit(0, angle);
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) camera_.orbit(0, -angle);
    if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS) camera_.zoom(-3.0f * dt);
    if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS) camera_.zoom(3.0f * dt);

    if (toggleFinale) scene.toggleFinale();
    if (toggleLamp) scene.toggleLamp();
    if (changeMonitors) scene.cycleMonitorLayout();
    if (changeRgb) scene.cycleRgbMode();
    if (toggleHelp) ui.toggleVisible();
    if (toggleDay) scene.toggleDayNight();
    if (toggleFeed) scene.setFeedEnabled(!scene.feedEnabled());
    if (glfwGetKey(window_, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
        scene.setRgbSpeed(scene.rgbSpeed() - 0.8f * dt);
    if (glfwGetKey(window_, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
        scene.setRgbSpeed(scene.rgbSpeed() + 0.8f * dt);
}
void Application::run() { runScene(); }
void Application::runScene() {
    // All GPU objects must be destroyed while this GL context is current.
    Shader lit("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    Shader rgb("assets/shaders/rgb.vert", "assets/shaders/rgb.frag");
    Shader sky("assets/shaders/sky.vert", "assets/shaders/sky.frag");
    Shader screen("assets/shaders/screen.vert", "assets/shaders/screen.frag");
    Scene scene;
    Framebuffer secondaryFeed(640, 368); // approximately the monitor's ratio
    UiPanel ui(window_);

    lit.use();
    lit.setInt("uTexture", 0);
    screen.use();
    screen.setInt("uCameraFeed", 0);

    // Second camera is intentionally different from the orbit camera.
    // Its fixed viewpoint captures the desk while the user moves freely.
    const glm::vec3 securityEye(-2.70f, 2.72f, 2.40f);
    const glm::mat4 securityView = glm::lookAt(
        securityEye, glm::vec3(0.0f, 1.38f, -0.38f), glm::vec3(0, 1, 0));
    const glm::mat4 securityProjection = glm::perspective(
        glm::radians(65.0f),
        float(secondaryFeed.width()) / float(secondaryFeed.height()),
        0.10f, 100.0f);

    double previous = glfwGetTime(), elapsed = 0.0;
    float fps = 0.0f;
    int frameCount = 0;
    while (!glfwWindowShouldClose(window_)) {
        const double now = glfwGetTime();
        const double rawDt = now - previous;
        previous = now;
        glfwPollEvents();
        const float dt = float(std::clamp(rawDt, 0.0, 0.05));
        processInput(dt, scene, ui);
        scene.update(dt);

        int width = 0, height = 0;
        glfwGetFramebufferSize(window_, &width, &height);
        if (width > 0 && height > 0) {
            // PASS A: render a second live camera view into a color texture.
            // Screen geometry is omitted to prevent sampling our current FBO.
            if (scene.feedEnabled()) {
                secondaryFeed.bind();
                glViewport(0, 0, secondaryFeed.width(), secondaryFeed.height());
                clearScene(scene.dayFactor());
                renderWorld(scene, lit, rgb, sky, securityView,
                            securityProjection, securityEye, float(now));
                Framebuffer::unbind();
            }

            // PASS B: main orbit camera to the default framebuffer.
            // Restore viewport; the offscreen framebuffer has a different size.
            glViewport(0, 0, width, height);
            clearScene(scene.dayFactor());
            const glm::mat4 view = camera_.viewMatrix();
            const glm::mat4 projection = camera_.projectionMatrix(
                float(width) / float(height));
            renderWorld(scene, lit, rgb, sky, view, projection,
                        camera_.position(), float(now));

            // PASS C: map the pre-rendered FBO texture to the right monitor.
            // Texture binding is not a CPU upload and does not rebuild meshes.
            if (scene.feedEnabled()) {
                screen.use();
                screen.setMat4("uView", view);
                screen.setMat4("uProjection", projection);
                secondaryFeed.bindColorTexture(0);
                scene.draw(screen, RenderPass::Screen);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            ui.beginFrame();
            ui.draw(scene, fps);
            ui.endFrame();
            glfwSwapBuffers(window_);
        }

        elapsed += rawDt;
        ++frameCount;
        if (elapsed >= 0.5) {
            fps = float(double(frameCount) / elapsed);
            std::ostringstream title;
            title << "Dream Developer Desk | " << std::fixed
                  << std::setprecision(1) << fps << " FPS"
                  << " | F: finale | N: day/night | P: camera feed";
            glfwSetWindowTitle(window_, title.str().c_str());
            elapsed = 0.0;
            frameCount = 0;
        }
    }
}
