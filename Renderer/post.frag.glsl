#version 410 core

in float varDelta;
in vec2 varTexCoord;

layout(location = 0) out vec4 fragColor;

uniform sampler2D framebuffer;
uniform sampler2D depthbuffer;
uniform sampler2D normalbuffer;

void main() {
  vec4 color = texture(framebuffer, varTexCoord);
  
  float depthValue = texture(depthbuffer, varTexCoord.st).r;
  vec4 depth = vec4(depthValue, depthValue, depthValue, 1.0);
  fragColor = depth;
  
  vec4 normal = texture(normalbuffer, varTexCoord.st);
//  fragColor = normal;
//  fragColor = color * depth;
//  fragColor = color * vec4(abs(cos(varDelta)), 0.0, abs(sin(varDelta)), 1.0);
}