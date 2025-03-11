#version 430 core

out vec4 FragColor;
uniform float spacing;
uniform int thickness;


void main() {
    vec2 modResult = mod(gl_FragCoord.xy, spacing);
    
    float epsilon = 0.5 + thickness;
    
    if (modResult.x < epsilon && modResult.y < epsilon) {
        FragColor = vec4(1.0); // White
    } else {
        FragColor = vec4(0.0); // Black
    }
}
