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

	ambientTerm = Light_ambient * vec4(Ka,1.0);

	vec3 N = normalize(normal);
	vec3 L = normalize(lightDir);
	vec3 E = normalize(eyeDir);
	vec3 R = reflect(-L, N);
	
	float D = clamp(dot(L, N),0.0, 1.0);
	
	
	if(D<0.25)
		D = 0;
	else if(D<0.5)
		D = 0.25;
	else if(D<0.75)
		D = 0.5;
	else if(D<1.0)
		D = 0.75;
	else
		D = 1.0;

	diffuseTerm = Light_diffuse * vec4(Kd,1.0) * D;

	float specular = pow(max(dot(R, E), 0.0), Ns);
	specularTerm = Light_specular * vec4(Ks,1.0) * specular;

	outColor = texture2D(mainTexture, Texcoord) * (attenuation * diffuseTerm + ambientTerm) + attenuation * specularTerm;
	
}