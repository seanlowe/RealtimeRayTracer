#version 450
layout(location = 0) in vec3 vertexPosition;
out vec2 ScreenCoord;
void main()
{
	gl_Position.xyz = vertexPosition;
	ScreenCoord = gl_Position.xy;
	gl_Position.w = 1.0;
}
