#version 300 es

#define SOURCE %d
#define MASK %d

#define INPUT_TEXTURE_RGBA 1
#define INPUT_TEXTURE_RGBX 2
#define INPUT_TEXTURE_EXTERNAL 3

#if !defined(SOURCE)
#error "Missing shader preamble"
#endif

#if SOURCE == INPUT_TEXTURE_EXTERNAL || MASK == INPUT_TEXTURE_EXTERNAL
#extension GL_OES_EGL_image_external : require
#endif

precision highp float;

in vec2 v_texcoord;
in vec2 v_tex2coord;

#if SOURCE == INPUT_TEXTURE_EXTERNAL
uniform samplerExternalOES tex;
#elif SOURCE == INPUT_TEXTURE_RGBA || SOURCE == INPUT_TEXTURE_RGBX
uniform sampler2D tex;
#endif
#if MASK == INPUT_TEXTURE_EXTERNAL
uniform samplerExternalOES tex2;
#elif MASK == INPUT_TEXTURE_RGBA || MASK == INPUT_TEXTURE_RGBX
uniform sampler2D tex2;
#endif

uniform float alpha;

uniform vec2 size;
uniform vec2 position;
uniform float radius_top_left;
uniform float radius_top_right;
uniform float radius_bottom_left;
uniform float radius_bottom_right;

uniform vec2 clip_size;
uniform vec2 clip_position;
uniform float clip_radius_top_left;
uniform float clip_radius_top_right;
uniform float clip_radius_bottom_left;
uniform float clip_radius_bottom_right;

uniform bool discard_transparent;

out vec4 fragColor;

vec4 sample_texture() {
#if SOURCE == INPUT_TEXTURE_RGBA || SOURCE == INPUT_TEXTURE_EXTERNAL
	return texture2D(tex, v_texcoord);
#elif SOURCE == INPUT_TEXTURE_RGBX
	return vec4(texture2D(tex, v_texcoord).rgb, 1.0);
#endif
}

vec4 sample_mask() {
#if MASK == INPUT_TEXTURE_RGBA || MASK == INPUT_TEXTURE_EXTERNAL
	return texture2D(tex2, v_tex2coord);
#elif MASK == INPUT_TEXTURE_RGBX
    return vec4(texture2D(tex, v_tex2coord).rgb, 1.0);
#endif
}

float corner_alpha(vec2 size, vec2 position, float round_tl, float round_tr, float round_bl, float round_br);

void main() {
    if (sample_mask().a == 0.0) {
        discard;
        return;
    }

    float quad_corner_alpha = corner_alpha(
        size - 0.5,
        position + 0.25,
        radius_top_left,
        radius_top_right,
        radius_bottom_left,
        radius_bottom_right
    );

    // Clipping
    float clip_corner_alpha = corner_alpha(
        clip_size - 1.0,
        clip_position + 0.5,
        clip_radius_top_left,
        clip_radius_top_right,
        clip_radius_bottom_left,
        clip_radius_bottom_right
    );

	fragColor = mix(sample_texture() * alpha, vec4(0.0), quad_corner_alpha) * clip_corner_alpha;

	if (fragColor.a == 0.0 && discard_transparent) {
		discard;
		return;
	}
}
