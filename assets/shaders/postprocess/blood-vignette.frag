#version 330

uniform sampler2D tex;
in vec2 tex_coord;
out vec4 frag_color;

void main(){
    // Calculate normalized device coordinates (NDC) from texture coordinates
    vec2 ndc = 2.0 * tex_coord - 1.0;
    
    // Calculate the distance from the center of the screen
    float distance = length(ndc);
    
    // Define the radius within which we want the corners to be blood red
    float radius = 1; // Adjust as needed
    
    // Calculate vignette strength based on the distance from the center
    float vignette = smoothstep(radius, radius + 0.05, distance);
    
    // Create a blood red color
    vec4 bloodColor = vec4(0.5, 0.0, 0.0, 1.0); // Adjust color values as needed
    
    // Apply vignette effect with blood red tint
    frag_color = texture(tex, tex_coord) * mix(vec4(1.0), bloodColor, vignette);
}
