#version 430

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec4 vCameraPosition[];
in vec4 vGlobalPosition[];
in vec4 vLocalPosition[];
in vec4 vNormal[];
in vec4 vColor[];

flat out vec4 fragCenter;
out vec4 gCameraPosition;
out vec4 gGlobalPosition;
out vec4 gLocalPosition;
out vec4 gNormal;
out vec4 gColor;

void main()
{

	float dxProj = 0.2;
	vec4 posDxProj = vec4(-dxProj, -dxProj, 0.0, 0.0);

	gl_Position = gl_in[0].gl_Position + posDxProj;
	gCameraPosition = vCameraPosition[0] + posDxProj;
	gGlobalPosition = vGlobalPosition[0];
	gLocalPosition = vLocalPosition[0];
	gNormal = vNormal[0];
	gColor = vColor[0];
	
	fragCenter = vCameraPosition[0];
	EmitVertex();


	posDxProj = vec4(dxProj, -dxProj, 0.0, 0.0);
	gl_Position = gl_in[0].gl_Position + posDxProj;
	gCameraPosition = vCameraPosition[0] + posDxProj;
	gGlobalPosition = vGlobalPosition[0];
	gLocalPosition = vLocalPosition[0];
	gNormal = vNormal[0];
	gColor = vColor[0];

	fragCenter = vCameraPosition[0];
	EmitVertex();


	posDxProj = vec4(-dxProj, dxProj, 0.0, 0.0);
	gl_Position = gl_in[0].gl_Position + posDxProj;
	gCameraPosition = vCameraPosition[0] + posDxProj;
	gGlobalPosition = vGlobalPosition[0];
	gLocalPosition = vLocalPosition[0];
	gNormal = vNormal[0];
	gColor = vColor[0];

	fragCenter = vCameraPosition[0];
	EmitVertex();


	posDxProj = vec4(dxProj, dxProj, 0.0, 0.0);
	gl_Position = gl_in[0].gl_Position + posDxProj;
	gCameraPosition = vCameraPosition[0] + posDxProj;
	gGlobalPosition = vGlobalPosition[0];
	gLocalPosition = vLocalPosition[0];
	gNormal = vNormal[0];
	gColor = vColor[0];

	fragCenter = vCameraPosition[0];
	EmitVertex();


	EndPrimitive();
}