#version 330

uniform sampler2D tex;
uniform float iTime;
uniform vec2 iResolution;

in vec2 tex_coord;
out vec4 frag_color;

void main()
{
    vec2 uv = tex_coord;

    float mdf = 0.1; // effect strength to be changed
    float noise = (fract(sin(dot(uv, vec2(12.9898,78.233)*2.0)) * 43758.5453));
    vec4 texColor = texture(tex, uv);
    
    mdf *= sin(iTime) + 1.0; // animate the effect's strength

    frag_color = texColor - noise * mdf;
}