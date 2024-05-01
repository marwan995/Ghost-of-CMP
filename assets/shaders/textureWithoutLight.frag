#version 330 core

in Varyings {
    vec4 color;
    vec2 tex_coord;
} fs_in;
struct Material {
    sampler2D tex;
    float shininess;
}; 

uniform Material material;

out vec4 frag_color;

uniform vec4 tint;

void main(){
    vec4 texture_color = texture(material.tex, fs_in.tex_coord);
    frag_color = tint  * texture_color;
}