#version 330 core

in Varyings {
    vec3 fragPosition;
    vec4 color;
    vec2 tex_coord;
    vec3 normal;

} fs_in;

struct Material {
    sampler2D tex;
    float shininess;
}; 
struct Light {
    vec3  position;
    vec3  direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
 

    // for attenuation and point light 
    float constant;
    float linear;
    float quadratic;
    
};

uniform Light light; 

uniform Material material;

out vec4 frag_color;

uniform vec4 tint;
uniform vec3 camPos;

void main(){
    //TODO: (Req 7) Modify the following line to compute the fragment color
    // by multiplying the tint with the vertex color and with the texture color 
    // ambient lighting
    vec3 ambient = light.ambient * texture(material.tex, fs_in.tex_coord).rgb;

	// diffuse lighting
	vec3 normalized = normalize(fs_in.normal);
	vec3 lightDirection = normalize(light.position - fs_in.fragPosition);
	float diff = max(dot(normalized, lightDirection), 0.0f);
    vec3 diffuse = light.diffuse * (diff *  texture(material.tex, fs_in.tex_coord).rgb);


    //
	vec3 viewDirection = normalize(camPos - fs_in.fragPosition);
	vec3 reflectionDirection = reflect(-lightDirection, normalized);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), material.shininess);
    vec3 specular = light.specular * (specAmount *  texture(material.tex,fs_in.tex_coord).rgb);  

    float theta = dot(lightDirection, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    // attenuation
    float distance   = length(light.position - fs_in.fragPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    ambient  *= attenuation;  
    diffuse   *= attenuation;
    specular *= attenuation;   

    frag_color = tint * (vec4(specular+diffuse+ambient,1.0));
}