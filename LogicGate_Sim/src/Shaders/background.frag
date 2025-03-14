#version 430 core

#define MAXNODE_NUM 64
#define MAXWIRE_NUM 64

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

struct Wire {
    vec2 p1;
    vec2 p2;
    bool state;
};

uniform Wire wires[MAXWIRE_NUM];
uniform int wireNum;

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

    // Node glow
    for (int i = 0; i < nodeNum; ++i) {
        Node node = nodes[i];
        if (node.state) {
            float nodeRadius = spacing / 2.0;
            vec2 flippedNodePos = vec2(node.position.x, resolution.y - node.position.y);
            float distFromNode = max(distance(position, flippedNodePos), 0.001); // Avoid division by zero
            float normalizedDist = distFromNode / nodeRadius;

            float pulse = sin(time * 2.0) / 7.0 + (1.0 - 1.0 / 7.0);
            float intensity = smoothstep(0.0, 1.0, pulse * 0.9 / (normalizedDist * normalizedDist)); // Smoothed glow

            vec3 glowColor = vec3(219.0, 22.0, 47.0) / 255.0 * intensity;
            FragColor.rgb = min(FragColor.rgb + glowColor, vec3(1.0)); // Prevent overexposure
        }
    }
    // Wire glow
    for (int i = 0; i < wireNum; ++i) {
        Wire wire = wires[i];
        vec2 p1 = vec2(wire.p1.x, resolution.y - wire.p1.y);
        vec2 p2 = vec2(wire.p2.x, resolution.y - wire.p2.y);

        // Get direction and length of the line segment
        vec2 dir = p2 - p1;
        float lenSq = dot(dir, dir);
        
        // Projection factor (t)
        float t = dot(position - p1, dir) / lenSq;
        t = clamp(t, 0.0, 1.0);
        
        // Closest point on the line segment
        vec2 closest = p1 + t * dir;
        
        // Distance from fragment to closest point
        float distFromLine = max(length(position - closest), 0.001);
        if (wire.state) {
            float pulse = (1.25 * sin((length(closest - p1) - time * 100.0) * 0.05) + 1.0) / 2.0 + 1.5;

            float intensity = smoothstep(0.0, 1.0, pulse / (distFromLine * distFromLine)); // Smoothed glow

            vec3 glowColor = vec3(219.0, 22.0, 47.0) / 255.0 * intensity;
            FragColor.rgb = min(FragColor.rgb + glowColor, vec3(1.0)); // Prevent overexposure
        }
        else {
            if (distFromLine < 1.5) FragColor = vec4(vec3(147.0, 145.0, 150.0) / 255.0, 1.0);
        }
    }
}
