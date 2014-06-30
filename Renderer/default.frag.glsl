#version 410 core

in vec3 varVertex;
in vec3 varNormal;
in mat4 varModelView;
in mat4 varModelViewInverseTranspose;

out vec4 fragColor;

vec4 compute_light(vec3 direction, vec4 light_color, vec3 normal, vec3 half_vec, vec4 diffuse, vec4 specular, float shininess) {
    
    float n_dot_l = dot(normal, direction);
    float n_dot_h = dot(normal, half_vec);
    
    vec4 lambert = diffuse * light_color * max(n_dot_l, 0.0);
    vec4 phong = specular * light_color * pow(max(n_dot_h, 0.0), shininess);
    
    return lambert + phong;
}
/*
void altMain() {
    vec3 eye_pos = vec3(0.0, 0.0, 0.0);
    vec4 _pos = varModelView * varVertex;
    vec3 pos = _pos.xyz / _pos.w;
    vec3 eye_dir = normalize(eye_pos - pos);
    
    vec3 _normal = (varModelViewInverseTranspose * vec4(varNormal, 0.0)).xyz;
    vec3 normal = normalize(_normal);
    
    vec3 light_dir = normalize(light_0_dir);
    vec3
}
*/
void main() {
    fragColor = vec4(vec3(1.0, 0.85, 0.35), 1.0);
}
