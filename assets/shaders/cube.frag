#version 460

in vec3 v_position;
in vec3 v_normal;
in vec2 v_uv;

out vec4 f_frag_color;

uniform mat4 u_projection;
uniform mat4 u_modelview;
uniform mat3 u_normal;

uniform sampler2D u_texture;
uniform float u_time;

const vec3 c_color_0 = vec3(0, 0, 1);
const vec3 c_color_1 = vec3(0, 1, 1);
const vec3 c_color_2 = vec3(1, 1, 1);

void main() {
    float noise_value = texture(u_texture, mod(v_uv + u_time, vec2(1))).r;

    vec3 color = vec3(0, 0, 0);

    // interpolate between c_color_0 and c_color_1 and c_color_2
    if(noise_value < 0.33) {
        color = mix(c_color_0, c_color_1, noise_value / 0.33);
    } else if(noise_value < 0.66) {
        color = mix(c_color_1, c_color_2, (noise_value - 0.33) / 0.33);
    } else {
        color = mix(c_color_2, c_color_0, (noise_value - 0.66) / 0.33);
    }

    f_frag_color = vec4(color, (noise_value > 0.45 && noise_value < 0.55) ? (noise_value * 1.5f) : (noise_value / 2.f));

    // vec3 normal = normalize(inverse(u_normal) * v_normal);
    // f_frag_color = vec4(normal, 1);
}
