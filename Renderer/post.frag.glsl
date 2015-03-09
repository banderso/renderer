#version 410 core

in float varDelta;
in vec2 varTexCoord;

layout(location = 0) out vec4 fragColor;

uniform sampler2D framebuffer;

void main() {
  fragColor = texture(framebuffer, varTexCoord.st) * vec4(1.0, 1.0, sin(varDelta), 1.0);
}