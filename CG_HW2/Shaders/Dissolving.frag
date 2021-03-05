#version 400

uniform vec4 Light_specular;
uniform vec4 Light_diffuse;
uniform vec4 Light_ambient;
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;
uniform sampler2D mainTexture;
uniform sampler2D noiseTexture;
uniform float     threshold;

in vec2 Texcoord;
in vec3 normal, lightDir, eyeDir;

out vec4 outColor;

void main (void)
{
	vec4 yellow =vec4(255.0, 255.0, 0.0, 1.0);
	vec4 D = texture2D(noiseTexture, Texcoord);
	if( D.s < threshold )
		discard;
	else if( D.s < threshold + 0.05 )
		outColor = yellow;
	else
		outColor = texture2D(mainTexture, Texcoord);

}