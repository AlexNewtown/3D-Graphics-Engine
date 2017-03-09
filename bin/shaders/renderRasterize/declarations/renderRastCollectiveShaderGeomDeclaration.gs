#version 430

layout (lines) in;
layout (line_strip, max_vertices = 2) out;

in vec4 vPosition[];
in vec4 vColor[];

out vec4 gPosition;
out vec4 gColor;