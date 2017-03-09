#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec4 vPosition[];
in vec4 vColor[];
in vec4 vNormal[];
in int vMaterialIndex[];
in vec2 vTextureCoord[];
in vec3 vGlobalLightPosition[];
in vec3 vGlobalPosition[];

out vec4 gPosition;
out vec4 gColor;
out vec4 gNormal;
flat out int gMaterialIndex;
out vec2 gTextureCoord;
out vec3 vectorReturn;
out vec3 globalLightPosition;
out vec3 globalPosition;