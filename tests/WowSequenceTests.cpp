#ifdef NDEBUG
#undef NDEBUG
#endif
#include "scene/WowSequence.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

namespace {
bool near(float actual, float expected, float eps = 0.0003f) {
    return std::fabs(actual - expected) <= eps;
}
void advance(WowSequence& w, float seconds) {
    // Simulates a realistic update at 60 FPS (frame-rate independence).
    int count = static_cast<int>(seconds * 60.0f + 0.5f);
    for (int i = 0; i < count; ++i) w.update(1.0f / 60.0f);
}
void testFullFinale() {
    WowSequence w;
    assert(!w.active());
    w.start(1.0f, 1.0f);
    assert(w.phase() == WowSequence::Phase::Dimming);
    assert(near(w.values().daylight, 1.0f));
    advance(w, 1.2f);
    assert(w.values().daylight > 0.0f && w.values().daylight < 1.0f);
    assert(w.values().lampLevel > 0.0f && w.values().lampLevel < 1.0f);
    advance(w, 3.6f);
    assert(w.phase() == WowSequence::Phase::Showcase);
    assert(w.active());
    assert(near(w.values().daylight, 0.0f));
    assert(near(w.values().lampLevel, 0.0f));
    assert(near(w.values().roomMultiplier, 0.06f));
    assert(near(w.values().rgbBoost, 2.75f));
    assert(near(w.progress(), 1.0f));
    advance(w, 10.0f); // The final shot holds until F is pressed again.
    assert(w.phase() == WowSequence::Phase::Showcase);
    w.restore();
    assert(w.phase() == WowSequence::Phase::Restoring);
    advance(w, 2.0f);
    assert(!w.active());
    assert(near(w.values().daylight, 1.0f));
    assert(near(w.values().lampLevel, 1.0f));
    assert(near(w.values().rgbBoost, 1.0f));
    assert(near(w.values().roomMultiplier, 1.0f));
}
void testCancelMidwayAndNightStart() {
    WowSequence w;
    w.start(0.35f, 0.0f); // User had lamp off and partial daylight.
    advance(w, 0.65f);
    float lightAtCancel = w.values().daylight;
    w.restore();
    assert(near(w.values().daylight, lightAtCancel));
    advance(w, 2.1f);
    assert(!w.active());
    assert(near(w.values().daylight, 0.35f));
    assert(near(w.values().lampLevel, 0.0f));
    w.start(0.0f, 0.0f);
    advance(w, 5.0f);
    assert(w.phase() == WowSequence::Phase::Showcase);
    w.restore();
    advance(w, 2.0f);
    assert(near(w.values().daylight, 0.0f));
}
void testInvalidDtAndRapidInput() {
    WowSequence w;
    w.start(1.0f, 1.0f);
    w.start(0.0f, 0.0f); // Cannot overwrite saved state mid-finale.
    w.update(-1.0f);
    assert(near(w.values().daylight, 1.0f));
    w.update(50.0f); // Paused-app spike is limited to one small update.
    assert(w.values().daylight > 0.95f);
    w.restore();
    w.restore();
    advance(w, 2.0f);
    assert(near(w.values().daylight, 1.0f));
}
}
int main() {
    testFullFinale();
    testCancelMidwayAndNightStart();
    testInvalidDtAndRapidInput();
    std::cout << "PASS: WowSequence CPU timing, hold, restore, cancel, and lag spike tests.\n";
}
