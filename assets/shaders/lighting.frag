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
    float shininess;
    sampler2D albedo;
    sampler2D specular;
    sampler2D emissive;
    sampler2D roughness;
    sampler2D ambientOcclusion;
}; 


uniform Material material;

out vec4 frag_color;

uniform vec4 tint;
uniform vec3 camPos;
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 material_ambient, vec3 material_diffuse, vec3 material_specular);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDirection,vec3 material_ambient, vec3 material_diffuse, vec3 material_specular, vec3 material_shininess);  
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir,vec3 material_ambient, vec3 material_diffuse, vec3 material_specular, vec3 material_shininess);


void main(){
    vec3 normalized = normalize(fs_in.normal);
	
	vec3 material_diffuse = texture(material.albedo, fs_in.tex_coord).rgb;
    vec3 material_specular = texture(material.specular, fs_in.tex_coord).rgb;
    vec3 material_ambient = material_diffuse * texture(material.ambientOcclusion, fs_in.tex_coord).r;
	float material_roughness = texture(material.roughness, fs_in.tex_coord).r;
    float material_shininess = 2.0 / pow(clamp(material_roughness, 0.001, 0.999), 4.0) - 2.0;

    vec3 viewDirection = normalize(camPos - fs_in.fragPosition);
    vec3 combination = CalcDirLight(dirLight, normalized, viewDirection, material_ambient, material_diffuse, material_specular);

        // do the same for all point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        combination += CalcPointLight(pointLights[i],normalized,viewDirection, material_ambient, material_diffuse, material_specular, material_shininess);
    combination += CalcSpotLight(spotLight, normalized, viewDirection, material_ambient, material_diffuse, material_specular, material_shininess);
    frag_color = tint * vec4(1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 material_ambient, vec3 material_diffuse, vec3 material_specular)
{
    vec3 lightDirection = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDirection), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDirection, normal);
    float specAmount  = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient * material_ambient;
    vec3 diffuse = light.diffuse * (diff *  material_diffuse);
    vec3 specular = light.specular * (specAmount *  material_specular); 
    return (ambient + diffuse + specular);
}  
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDirection, vec3 material_ambient, vec3 material_diffuse, vec3 material_specular, vec3 material_shininess)
{
    vec3 lightDirection =normalize(light.position - fs_in.fragPosition);
    float diff = max(dot(normal, lightDirection), 0.0f);


    vec3 ambient = light.ambient * material_ambient;
    vec3 diffuse = light.diffuse * (diff * material_diffuse);
    
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), material_shininess);
    vec3 specular = light.specular * (specAmount *  material_specular);  
   // attenuation
    float distance   = length(light.position - fs_in.fragPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    ambient  *= attenuation;  
    diffuse   *= attenuation;
    specular *= attenuation;  

    return (ambient + diffuse + specular); 
}
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDirection, vec3 material_ambient, vec3 material_diffuse, vec3 material_specular, vec3 material_shininess){
    vec3 lightDirection =normalize(light.position - fs_in.fragPosition);
    float diff = max(dot(normal, lightDirection), 0.0f);


    vec3 ambient = light.ambient *  material_ambient;
    vec3 diffuse = light.diffuse * (diff * material_diffuse);
    
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), material_shininess);
    vec3 specular = light.specular * (specAmount * material_specular);  
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