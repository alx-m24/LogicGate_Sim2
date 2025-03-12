#version 430 core

#define MAXNODE_NUM 64

out vec4 FragColor;
uniform float spacing;
uniform float time;
uniform int thickness;
uniform vec2 resolution;

struct Node {
    vec2 position;
    bool state;
};

uniform Node nodes[MAXNODE_NUM];
uniform int nodeNum;

void main() {
    vec2 position = gl_FragCoord.xy;
    vec2 uv = position / resolution;
    vec2 modResult = mod(position + spacing / 2.0, spacing);
    
    float epsilon = 0.5 + thickness;
    
    if (modResult.x < epsilon && modResult.y < epsilon) {
        FragColor = vec4(1.0); // White grid
        return;
    }

    float distFromCenter = length(2.0 * uv - 1.0);

    // Base background color
    FragColor = vec4(vec3(64.0, 61.0, 57.0) / 255.0, 1.0);
    FragColor = vec4(mix(FragColor, FragColor / 2.0, distFromCenter).xyz, 1.0);

    for (int i = 0; i < nodeNum; ++i) {
        Node node = nodes[i];
        // Glow Effect
        if (node.state) {
            float nodeRadius = spacing / 2.0;
            vec2 flippedNodePos = vec2(node.position.x, resolution.y - node.position.y);
            float distFromNode = max(distance(position, flippedNodePos), 0.001); // Avoid division by zero
            float normalizedDist = distFromNode / nodeRadius;

            float pulse = sin(time * 2.0) / 7.0 + (1.0 - 1.0 / 7.0);
            float intensity = smoothstep(0.0, 1.0, pulse / (normalizedDist * normalizedDist)); // Smoothed glow

            vec3 glowColor = vec3(219.0, 22.0, 47.0) / 255.0 * intensity;
            FragColor.rgb = min(FragColor.rgb + glowColor, vec3(1.0)); // Prevent overexposure
        }
    }
}
