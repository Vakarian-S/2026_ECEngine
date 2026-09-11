#pragma once
#include <memory>
#include <Vector.h>

class LightActor;
/**
 * Firework struct used to hold all the extra variables needed for the lifetime of a Firework Light
 * Tracks movement, lifetime, intensity flicker, and attenuation over time.
 * Note: field initializers are defaults and are overwritten on spawn.
 */
struct Firework
{
    /** Max intensity at explosion peak (default, randomized on spawn). */
    float explosion_intensity_peak = 5.0f;
    /** Owning light actor for render/light parameters. */
    std::shared_ptr<LightActor> light_actor;
    /** Current velocity in world units per second (default, randomized on spawn). */
    MATH::Vec3 velocity = MATH::Vec3(0.0f, 0.0f, 0.0f);
    /** Age since last spawn in seconds (default, reset on spawn). */
    float age_seconds = 0.0f;
    /** Total lifetime before respawn in seconds (default, calculated as ascent + fixed explosion duration on spawn). */
    float lifetime_seconds = 50.0f;
    /** Baseline light intensity before explosion (default, randomized on spawn). */
    float base_intensity = 1.0f;
    /** Flicker oscillation speed in radians per second (default, randomized on spawn). */
    float flicker_speed = 5.0f;
    /** Flicker amplitude multiplier (0..1 typical, default randomized on spawn). */
    float flicker_amplitude = 0.9f;
    /** Linear attenuation start value (default, randomized on spawn). */
    float attenuation_linear_start = 0.02f;
    /** Linear attenuation end value (default, randomized on spawn). */
    float attenuation_linear_end = 0.30f;
    /** Quadratic attenuation start value (default, randomized on spawn). */
    float attenuation_quadratic_start = 0.003f;
    /** Quadratic attenuation end value (default, randomized on spawn). */
    float attenuation_quadratic_end = 0.08f;

    /** Fireworks phases */
    /** Time to fly upward before explosion (default, randomized on spawn). */
    float ascent_duration = 2.0f;
    /** Age when explosion triggers (default, set to ascent duration on spawn). */
    float explosion_start = 2.0f;
    /** Whether explosion phase has occurred. */
    bool has_exploded = false;
    
};
