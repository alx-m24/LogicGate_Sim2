#version 430 core

out vec4 FragColor;
uniform float spacing;
uniform int thickness;
uniform vec2 resolution;

struct Node {
    vec2 position;
    bool state;
};

uniform Node node;

void main() {
    vec2 position = gl_FragCoord.xy;
    vec2 uv = position / resolution;
    vec2 modResult = mod(position + spacing / 2.0, spacing);
    
    float epsilon = 0.5 + thickness;
    
    if (modResult.x < epsilon && modResult.y < epsilon) {
        FragColor = vec4(1.0); // White
        return;
    }

    float distFromCenter = length(2.0 * uv - 1.0);

    FragColor = vec4(vec3(34.0, 31.0, 27.0) / 255.0, 1.0);
    FragColor = vec4(mix(FragColor, FragColor / 1.5, distFromCenter).xyz, 1.0);

    vec2 flippedNodePos = vec2(node.position.x, resolution.y - node.position.y);
    float distFromNode = distance(position, flippedNodePos);
    float intensity = (300.0 * int(node.state)) / max(distFromNode * distFromNode, 0.001);
    FragColor = FragColor + vec4((vec3(219.0, 22.0, 47.0) / 255.0) * intensity, 1.0);
}
