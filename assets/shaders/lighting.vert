#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 normal;

out Varyings {
    vec3 fragPosition;
    vec4 color;
    vec2 tex_coord;
    vec3 normal;

} vs_out;

uniform mat4 transform;
uniform mat4 model;

void main(){
    //TODO: (Req 7) Change the next line to apply the transformation matrix
    gl_Position = transform *vec4(position, 1.0);
    vs_out.color = color;
    vs_out.tex_coord = tex_coord;
    vs_out.normal = mat3(transpose(inverse(model))) *  normal;
    vs_out.fragPosition = vec3(model * vec4(position, 1.0));
}