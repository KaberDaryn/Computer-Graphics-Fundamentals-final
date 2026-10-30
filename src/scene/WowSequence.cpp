#include "scene/WowSequence.hpp"
#include <algorithm>

namespace {
constexpr float DIM_SECONDS = 2.40f;
constexpr float REVEAL_SECONDS = 2.10f;
constexpr float RESTORE_SECONDS = 1.85f;
constexpr float NIGHT_ROOM_MULTIPLIER = 0.06f;
constexpr float MAX_RGB_BOOST = 2.75f;
}

float WowSequence::ease(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}
float WowSequence::blend(float from, float to, float t) {
    return from + (to - from) * ease(t);
}
float WowSequence::duration(Phase phase) {
    switch (phase) {
        case Phase::Dimming: return DIM_SECONDS;
        case Phase::RgbReveal: return REVEAL_SECONDS;
        case Phase::Restoring: return RESTORE_SECONDS;
        default: return 0.0f;
    }
}
void WowSequence::start(float daylight, float lampLevel) {
    if (active()) return;
    original_ = {std::clamp(daylight, 0.0f, 1.0f),
                 std::clamp(lampLevel, 0.0f, 1.0f), 1.0f, 1.0f};
    values_ = original_;
    elapsed_ = 0.0f;
    phase_ = Phase::Dimming;
}
void WowSequence::restore() {
    if (!active() || phase_ == Phase::Restoring) return;
    restoreFrom_ = values_; // Works even if F is pressed mid-transition.
    phase_ = Phase::Restoring;
    elapsed_ = 0.0f;
}
void WowSequence::apply(float t) {
    switch (phase_) {
        case Phase::Dimming:
            values_.daylight = blend(original_.daylight, 0.0f, t);
            values_.lampLevel = blend(original_.lampLevel, 0.0f, t);
            values_.roomMultiplier = blend(1.0f, NIGHT_ROOM_MULTIPLIER, t);
            values_.rgbBoost = blend(1.0f, 1.20f, t);
            break;
        case Phase::RgbReveal:
            values_.daylight = 0.0f;
            values_.lampLevel = 0.0f;
            values_.roomMultiplier = NIGHT_ROOM_MULTIPLIER;
            values_.rgbBoost = blend(1.20f, MAX_RGB_BOOST, t);
            break;
        case Phase::Showcase:
            values_ = {0.0f, 0.0f, NIGHT_ROOM_MULTIPLIER, MAX_RGB_BOOST};
            break;
        case Phase::Restoring:
            values_.daylight = blend(restoreFrom_.daylight, original_.daylight, t);
            values_.lampLevel = blend(restoreFrom_.lampLevel, original_.lampLevel, t);
            values_.roomMultiplier = blend(restoreFrom_.roomMultiplier, 1.0f, t);
            values_.rgbBoost = blend(restoreFrom_.rgbBoost, 1.0f, t);
            break;
        case Phase::Normal: break;
    }
}
void WowSequence::update(float deltaTime) {
    // Large lag spikes or a paused window cannot skip the entire demo.
    float remaining = std::clamp(deltaTime, 0.0f, 0.10f);
    if (!active() || phase_ == Phase::Showcase) return;

    // Handles exact transition boundaries without losing residual delta time.
    for (int iteration = 0; iteration < 3 && remaining > 0.0f; ++iteration) {
        const float d = duration(phase_);
        if (d <= 0.0f) break;
        const float step = std::min(remaining, d - elapsed_);
        elapsed_ += step;
        remaining -= step;
        apply(elapsed_ / d);
        if (elapsed_ < d - 0.000001f) break;
        if (phase_ == Phase::Dimming) phase_ = Phase::RgbReveal;
        else if (phase_ == Phase::RgbReveal) phase_ = Phase::Showcase;
        else if (phase_ == Phase::Restoring) phase_ = Phase::Normal;
        elapsed_ = 0.0f;
    }
}
float WowSequence::progress() const {
    if (phase_ == Phase::Normal) return 0.0f;
    if (phase_ == Phase::Showcase) return 1.0f;
    if (phase_ == Phase::Dimming)
        return 0.5f * elapsed_ / DIM_SECONDS;
    if (phase_ == Phase::RgbReveal)
        return 0.5f + 0.5f * elapsed_ / REVEAL_SECONDS;
    return 1.0f - elapsed_ / RESTORE_SECONDS;
}
const char* WowSequence::phaseName() const {
    switch (phase_) {
        case Phase::Normal: return "Ready";
        case Phase::Dimming: return "Lights fading to black";
        case Phase::RgbReveal: return "RGB lights taking over";
        case Phase::Showcase: return "Night RGB showcase - press F to restore";
        case Phase::Restoring: return "Restoring previous lighting";
    }
    return "Unknown";
}
