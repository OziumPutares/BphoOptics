#version 330 core

layout (location = 0) in vec2 aPos; // Vertex position (-1 to 1)
out vec2 vTexCoord; // Pass to fragment shader

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);

    // Map from [-1,1] to [0,1] for better gradient control
    vTexCoord = aPos * 0.5 + 0.5;
}

