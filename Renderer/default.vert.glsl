#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out float varDelta;
out vec3 varVertex;
out vec3 varNormal;
out mat4 varModelView;
out mat4 varModelViewInverseTranspose;

uniform float delta;
uniform mat4 modelView;
uniform mat4 projection;

void main() {
  varDelta = delta;
  varVertex = position;
  varNormal = normal;
  varModelView = modelView;
  varModelViewInverseTranspose = transpose(inverse(modelView));
  
  gl_Position = projection * modelView * vec4(position, 1.0);
}
