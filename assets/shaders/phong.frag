#version 460

const int LIGHT_MAX = 16;
const int LIGHT_AMBIENT = 0;
const int LIGHT_POINT = 1;
const int LIGHT_DIRECTIONAL = 2;

struct Ligth {
    vec3 color;
    float strength;
    vec3 pos_or_dir;
    int type;
};

in vec3 v_position;
in vec3 v_normal;
in vec2 v_uv;

out vec4 f_frag_color;

uniform float u_time;
uniform vec4 u_color;
uniform sampler2D u_texture;
uniform bool u_use_texture;

uniform mat4 u_projection;
uniform mat4 u_modelview;
uniform mat3 u_normal;

uniform Ligth u_lights[LIGHT_MAX];
uniform int u_lights_count;

vec3 blinnPhong(vec3 w_i, vec3 l_i, vec3 n, vec3 k_d, vec3 k_s, float shininess) {
    vec3 w_o = vec3(0, 0, 1);
    vec3 half_vector = (w_o + w_i) / 2;
    
    vec3 result_diffuse = k_d * max(dot(w_i, n), 0);
    vec3 result_specular = k_s * pow(max(dot(half_vector, n), 0), shininess);

    return l_i * ( result_diffuse + result_specular );
}

vec3 calculateLight(Ligth light, vec3 color, vec3 normal) {
    if (light.type == LIGHT_AMBIENT) {
        return color  * light.color * light.strength;
    } else if (light.type == LIGHT_POINT) {
        vec3 direction = normalize(light.pos_or_dir - v_position);
        float strength = 1.0 / pow(length(light.pos_or_dir - v_position), 2);
        return blinnPhong(direction, light.color * light.strength * strength, normal, color, vec3(1), 32.);
    } else if (light.type == LIGHT_DIRECTIONAL) {
        vec3 direction = normalize(-light.pos_or_dir);
        return blinnPhong(direction, light.color * light.strength, normal, color, vec3(1), 32.);
    } 
    return vec3(0);
}

void main() {
    vec4 diffuse = u_use_texture ? texture(u_texture, v_uv) : u_color;
    vec3 color = vec3(0);
    for (int i = 0; i < u_lights_count; i++) {
        color += calculateLight(u_lights[i], diffuse.rgb, normalize(v_normal));
    }
    f_frag_color = vec4(color, diffuse.a);
}
