#version 330 core

in Varyings {
    vec3 fragPosition;
    vec4 color;
    vec2 tex_coord;
    vec3 normal;

} fs_in;

out vec4 frag_color;

uniform vec4 tint;
uniform sampler2D tex;
uniform vec3 camPos;

void main(){
    //TODO: (Req 7) Modify the following line to compute the fragment color
    // by multiplying the tint with the vertex color and with the texture color 
    // ambient lighting
	float ambient = 0.1f;

    vec3 lightPos = vec3(1.0, 2.0, 2.0);
	// diffuse lighting
	vec3 normalized = normalize(fs_in.normal);
	vec3 lightDirection =normalize(lightPos - fs_in.fragPosition);
	float diffuse = max(dot(normalized, lightDirection), 0.0f);

    //
    float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - fs_in.fragPosition);
	vec3 reflectionDirection = reflect(-lightDirection, normalized);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
	float specular = specAmount * specularLight;

    vec4 texture_color = texture(tex, fs_in.tex_coord);
    frag_color = tint * (texture_color*(2*diffuse+ambient+specular));
}