#version 410 core

in float varDelta;
in vec2 varTexCoord;

layout(location = 0) out vec4 fragColor;

uniform sampler2D framebuffer;

void main() {
  vec4 dcolor = vec4(1.0, 1.0, cos(varDelta), 1.0);
  vec4 color = texture(framebuffer, gl_FragCoord.xy);
  if (color.x == 0.0 &&
      color.y == 0.0 &&
      color.z == 0.0 &&
      color.w == 1.0) {
    color = dcolor;
  }
  
//  vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
//  vec2 tsize = textureSize(framebuffer, 0);
//  if (tsize.y > 359) {
//    color.x = 1.0;
//  } else {
//    color.y = 1.0;
//  }
  fragColor = color;
//  fragColor = vec4(gl_FragCoord.xy, 0.0, 1.0);
}