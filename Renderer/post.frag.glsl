#version 410 core

#define PI 3.14159265359

in float varDelta;
in vec2 varTexCoord;

layout(location = 0) out vec4 fragColor;

uniform vec2 varResolution;

uniform sampler2D framebuffer;
uniform sampler2D depthbuffer;
uniform sampler2D normalbuffer;

vec3 srgb(vec3 rgb) {
  vec3 mask = vec3(greaterThan(rgb, vec3(0.0031308)));
  return mix(rgb * 12.92,
             pow(rgb, vec3(1.0 / 2.4)) * 1.055 - 0.055,
             mask);
}

vec4 creationShaderToy() {
  float t = varDelta;
  vec2 r = vec2(480.0, 360.0);
  
  vec3 c;
  float l, z = t;
  for(int i = 0; i < 3; i++) {
    vec2 uv, p = varTexCoord;//fragCoord.xy / r;
    uv = p;
    p -= 0.5;
    p.x *= r.x / r.y;
    z += 0.07;
    l = length(p);
    uv += p / l * (sin(z) + 1.0) * abs(sin(l * 9.0 - z * 2.0));
    c[i] = 0.01 / length(abs(mod(uv, 1.0) - 0.5));
  }
  
  return vec4(c / l, t);
}

float doubleCircleSigmoid(float x, float a) {
  float minParamA = 0.0;
  float maxParamA = 1.0;
  a = max(minParamA, min(maxParamA, a));
  
  float y = 0;
  if (x <= a) {
    y = a - sqrt(a * a - x * x);
  } else {
    float na = 1.0 - a;
    float nx = x - 1.0;
    y = a + sqrt(na * na - nx * nx);
  }
  
  return y;
}

float plot(vec2 st, float pct) {
  float size = 0.02;
  return smoothstep(pct - size, pct, st.y) -
         smoothstep(pct, pct + size, st.y);
}

vec3 plotColor(vec2 st, float y) {
  float pct = plot(st, y);
  return /*(1.0 - pct) * */vec3(y);// + pct * vec3(0.0, 1.0, 0.0);
}

vec3 invert(vec3 color) {
  return vec3(abs(color.r - 1.0),
              abs(color.g - 1.0),
              abs(color.b - 1.0));
}

void main() {
  vec4 color = texture(framebuffer, varTexCoord);
//  fragColor = color;
//  fragColor = creationShaderToy() * color;
  
//  float depthValue = texture(depthbuffer, varTexCoord.st).r;
//  vec4 depth = vec4(depthValue, depthValue, depthValue, 1.0);
//  fragColor = depth;
  
//  vec4 normal = texture(normalbuffer, varTexCoord.st);
//  fragColor = normal;
//  fragColor = color * depth;
//  fragColor = color * vec4(abs(cos(varDelta)), 0.0, abs(sin(varDelta)), 1.0);
  
  float v = 13.0 * PI;
  float y = (1.0 + sin(varTexCoord.t * PI * v * v)) / 2.0;
//  float y = doubleCircleSigmoid(sin(varTexCoord.s * PI * v * v), 0.293);
  fragColor = vec4(srgb(color.rgb * plotColor(varTexCoord.st, y)), 1.0);
}
