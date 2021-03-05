#version 400

uniform mat4 MV;	// modelview matrix
uniform mat4 P;		// projection matrix
uniform vec3 Light_position;
uniform vec3 eye_position;

layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec3 in_Norm;
layout(location = 2) in vec2 in_Texcoord;

out vec2 Texcoord;
out vec3 normal;
out vec3 lightDir;
out vec3 eyeDir;
out float attenuation;

void main()
{
	
	mat4 normalMatrix = transpose(inverse(MV));
	vec3 vVertex = vec3(MV * vec4(in_Pos, 1.0));
	vec3 dist = Light_position - in_Pos;
	gl_Position = P * MV * vec4(in_Pos, 1.0);

	Texcoord = vec2(in_Texcoord);	
	normal = vec3(normalMatrix * vec4(in_Norm, 0.0));
	lightDir = Light_position - vVertex;
	eyeDir = -vVertex;
	attenuation = 3 / (dist[0]*dist[0] + dist[1]*dist[1] + dist[2]*dist[2]);
	
}