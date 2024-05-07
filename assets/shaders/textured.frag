#version 330 core
#define NR_POINT_LIGHTS 22  

in Varyings {
    vec3 fragPosition;
    vec4 color;
    vec2 tex_coord;
    vec3 normal;

} fs_in;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight;

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

uniform PointLight pointLights[NR_POINT_LIGHTS];

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};
uniform SpotLight spotLight;

struct Material {
    sampler2D tex;
    float shininess;
}; 


uniform Material material;

out vec4 frag_color;

uniform vec4 tint;
uniform vec3 camPos;
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir,vec3 texColor);  
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDirection,vec3 texColor);  
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir,vec3 texColor);


void main(){
    vec3 normalized = normalize(fs_in.normal);
    vec3 viewDirection = normalize(camPos - fs_in.fragPosition);
    vec3 texColor = texture(material.tex, fs_in.tex_coord).rgb;
    vec3 combination = CalcDirLight(dirLight, normalized, viewDirection,texColor);

        // do the same for all point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        combination += CalcPointLight(pointLights[i],normalized,viewDirection,texColor);
    combination += CalcSpotLight(spotLight, normalized, viewDirection,texColor);
    frag_color = tint * vec4(combination,1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 texColor)
{
    vec3 lightDirection = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDirection), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDirection, normal);
    float specAmount  = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient * texColor;
    vec3 diffuse = light.diffuse * (diff *  texColor);
    vec3 specular = light.specular * (specAmount *  texColor); 
    return (ambient + diffuse + specular);
}  
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDirection,vec3 texColor)
{
    vec3 lightDirection =normalize(light.position - fs_in.fragPosition);
    float diff = max(dot(normal, lightDirection), 0.0f);


    vec3 ambient = light.ambient * texColor;
    vec3 diffuse = light.diffuse * (diff * texColor);
    
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), material.shininess);
    vec3 specular = light.specular * (specAmount *  texColor);  
   // attenuation
    float distance   = length(light.position - fs_in.fragPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    ambient  *= attenuation;  
    diffuse   *= attenuation;
    specular *= attenuation;  

    return (ambient + diffuse + specular); 
}
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDirection,vec3 texColor){
    vec3 lightDirection =normalize(light.position - fs_in.fragPosition);
    float diff = max(dot(normal, lightDirection), 0.0f);


    vec3 ambient = light.ambient *  texColor;
    vec3 diffuse = light.diffuse * (diff * texColor);
    
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), material.shininess);
    vec3 specular = light.specular * (specAmount * texColor);  
   // attenuation
    float distance   = length(light.position - fs_in.fragPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

     // spotlight intensity
    float theta = dot(lightDirection, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    ambient  *= attenuation*intensity;  
    diffuse   *= attenuation*intensity;
    specular *= attenuation*intensity;  
    return (ambient + diffuse + specular);
}