#version 430

in vec4 gPosition;
in vec4 gColor;
in vec4 gNormal;
flat in int gMaterialIndex;
in vec2 gTextureCoord;
in vec3 globalLightPosition;
in vec3 globalPosition;