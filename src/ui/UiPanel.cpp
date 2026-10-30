#include "ui/UiPanel.hpp"
#include "scene/Scene.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

UiPanel::UiPanel(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    if (!ImGui::CreateContext())
        throw std::runtime_error("Cannot create ImGui context");
    ImGui::StyleColorsDark();
    auto& style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;
    style.FrameRounding = 5.0f;
    style.WindowPadding = ImVec2(14, 12);
    // GLFW backends chain our original camera mouse callbacks.
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        ImGui::DestroyContext();
        throw std::runtime_error("ImGui GLFW backend init failed");
    }
    if (!ImGui_ImplOpenGL3_Init("#version 330")) {
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        throw std::runtime_error("ImGui OpenGL3 backend init failed");
    }
}
UiPanel::~UiPanel() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
void UiPanel::beginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
void UiPanel::draw(Scene& scene, float fps) {
    if (!visible_) return;
    ImGui::SetNextWindowPos(ImVec2(14, 14), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(356, 0), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Dream Developer Desk | Controls", &visible_,
                     ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("OpenGL 3.3 Core  |  Interactive 3D Scene");
        ImGui::Text("FPS: %.1f", fps);
        ImGui::Separator();
        if (scene.finaleActive()) {
            ImGui::TextWrapped("Finale: %s", scene.finalePhaseName());
            ImGui::ProgressBar(scene.finaleProgress(), ImVec2(-1, 0));
            if (scene.finaleRestoring()) {
                ImGui::TextDisabled("Returning to the saved settings...");
            } else if (ImGui::Button("Restore previous lighting (F)")) {
                scene.toggleFinale();
            }
            ImGui::TextDisabled("Lighting and RGB controls are locked during finale.");
        } else if (ImGui::Button("Start wow finale (F)")) {
            scene.toggleFinale();
        }
        ImGui::Separator();

        ImGui::BeginDisabled(scene.finaleActive());
        bool lamp = scene.lampOn();
        if (ImGui::Checkbox("Desk lamp (L)", &lamp)) scene.toggleLamp();

        float day = scene.dayFactor();
        if (ImGui::SliderFloat("Daylight (N)", &day, 0.0f, 1.0f, "%.2f"))
            scene.setDayFactor(day);
        ImGui::SameLine();
        ImGui::TextUnformatted(day >= 0.5f ? "DAY" : "NIGHT");

        ImGui::EndDisabled();

        bool feed = scene.feedEnabled();
        if (ImGui::Checkbox("Live camera monitor (P)", &feed))
            scene.setFeedEnabled(feed);

        const char* layouts[] = {"Parallel", "Angled", "Wide"};
        int layout = scene.monitorLayout();
        if (ImGui::Combo("Monitor layout (M)", &layout, layouts, 3))
            scene.setMonitorLayout(layout);

        ImGui::BeginDisabled(scene.finaleActive());
        const char* modes[] = {"Rainbow wave", "Static color", "Off"};
        int mode = scene.rgbMode();
        if (ImGui::Combo("RGB mode (R)", &mode, modes, 3))
            scene.setRgbMode(mode);

        float speed = scene.rgbSpeed();
        if (ImGui::SliderFloat("RGB speed ([ / ])", &speed, 0.1f, 5.0f, "%.2fx"))
            scene.setRgbSpeed(speed);

        glm::vec3 rgbColor = scene.rgbColor();
        if (ImGui::ColorEdit3("Static color", glm::value_ptr(rgbColor)))
            scene.setRgbColor(rgbColor);
        ImGui::EndDisabled();
        ImGui::Separator();
        ImGui::TextUnformatted("Left drag / WASD : orbit camera");
        ImGui::TextUnformatted("Wheel / Q / E : zoom");
        ImGui::TextUnformatted("L : lamp   M : monitors   R : RGB mode");
        ImGui::TextUnformatted("N : day/night   P : live camera monitor");
        ImGui::TextUnformatted("[ / ] : RGB speed   H : hide/show UI");
        ImGui::TextUnformatted("F : play/restore finale, press F again anytime");
        ImGui::TextUnformatted("Esc : close application");
        ImGui::TextDisabled("Finale holds the RGB night shot until restored.");
    }
    ImGui::End();
}
void UiPanel::endFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
bool UiPanel::wantsMouse() {
    return ImGui::GetCurrentContext() && ImGui::GetIO().WantCaptureMouse;
}
bool UiPanel::wantsKeyboard() {
    return ImGui::GetCurrentContext() && ImGui::GetIO().WantCaptureKeyboard;
}
