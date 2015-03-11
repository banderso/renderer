#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out float varDelta;
out vec2 varTexCoord;

uniform float delta;
uniform mat4 modelView;
uniform mat4 projection;

void main() {
  varDelta = delta;
  varTexCoord = texCoord;
  
  gl_Position = projection * vec4(position, 1.0);
}
