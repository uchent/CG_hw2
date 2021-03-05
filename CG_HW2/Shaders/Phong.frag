#version 400

uniform vec4 Light_specular;
uniform vec4 Light_diffuse;
uniform vec4 Light_ambient;
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;
uniform sampler2D mainTexture;

in vec2 Texcoord;
in vec3 normal, lightDir, eyeDir;
in float attenuation;

out vec4 outColor;

void main (void)
{
	vec4 ambientTerm, diffuseTerm, specularTerm;

	ambientTerm = Light_ambient * vec4(Ka, 1.0);

	vec3 N = normalize(normal);
	vec3 L = normalize(lightDir);
	vec3 E = normalize(eyeDir);
	vec3 R = reflect(-L, N);

	float D = dot(N,L);
	
	if(D > 0.0)
	{
		diffuseTerm = Light_diffuse * vec4(Kd, 1.0) * D;

		float specular = pow( max(dot(R, E), 0.0), Ns);
		specularTerm = Light_specular * vec4(Ks, 1.0) * specular;
	}

	outColor = texture2D(mainTexture, Texcoord) * (attenuation * diffuseTerm + ambientTerm) + attenuation * specularTerm;
}