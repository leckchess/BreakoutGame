#version 330

layout (location = 0) in vec4 vertex_position;

uniform mat4 model_matrex;
uniform mat4 proj_matrex;

out vec2 TexCoords;
void main()
{
	gl_Position = proj_matrex * model_matrex * vec4(vertex_position.xy,0.0,1.0);

	TexCoords = vertex_position.zw;
}