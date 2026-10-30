#pragma once

struct GLFWwindow;
class Scene;

// Dear ImGui lifecycle is tied to an existing, current OpenGL context.
class UiPanel final {
public:
    explicit UiPanel(GLFWwindow* window);
    ~UiPanel();
    UiPanel(const UiPanel&) = delete;
    UiPanel& operator=(const UiPanel&) = delete;
    void beginFrame();
    void draw(Scene& scene, float fps);
    void endFrame();
    void toggleVisible() { visible_ = !visible_; }
    bool visible() const { return visible_; }
    static bool wantsMouse();
    static bool wantsKeyboard();
private:
    bool visible_ = true;
};
