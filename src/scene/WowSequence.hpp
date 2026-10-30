#pragma once

// CPU-only, time-step-based lighting choreography. No OpenGL or GLFW dependency.
// The scene reads these four values each frame and uploads them as uniforms.
class WowSequence final {
public:
    enum class Phase { Normal, Dimming, RgbReveal, Showcase, Restoring };
    struct Values {
        float daylight = 1.0f;
        float lampLevel = 1.0f;
        float roomMultiplier = 1.0f;
        float rgbBoost = 1.0f;
    };

    void start(float daylight, float lampLevel);
    void restore();
    void update(float deltaTime);

    bool active() const { return phase_ != Phase::Normal; }
    Phase phase() const { return phase_; }
    float progress() const;
    const char* phaseName() const;
    const Values& values() const { return values_; }

private:
    static float ease(float t);
    static float blend(float from, float to, float t);
    static float duration(Phase phase);
    void apply(float t);

    Phase phase_ = Phase::Normal;
    float elapsed_ = 0.0f;
    Values values_{};
    Values original_{};
    Values restoreFrom_{};
};
