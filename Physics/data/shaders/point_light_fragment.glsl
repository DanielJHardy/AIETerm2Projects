#version 410

out vec3 light_output;

uniform vec3 light_view_position;
uniform vec3 light_diffuse;
uniform float light_radius;

uniform sampler2D position_texture;
uniform sampler2D normal_texture;

float specPower;

void main()
{
	specPower = 10;

	//compute texture coord
	vec2 texcoord = gl_FragCoord.xy / textureSize(position_texture,0).xy;


	//sample from textures
	vec3 position_sample = texture(position_texture, texcoord).xyz;
	vec3 normal_sample = texture(normal_texture, texcoord).xyz;

	normal_sample *= 2;
	normal_sample -= 1;

	//compute light direction
	vec3 to_light = light_view_position - position_sample;
	vec3 L = normalize(to_light);

	//compute lambertian term
	float d = max(0.0f, dot(normal_sample, L));

	//compute falloff
	float falloff = 1.0f - min(1.0f,(length(to_light) / light_radius));

	//specular
	vec3 E = normalize(position_sample);
	vec3 R = reflect(L,normal_sample.xyz);
	float s = max(0.0f, dot(E,R));
	s = pow(s, specPower);

	//output lambert * fallof * color
	light_output = (d * falloff * light_diffuse) + (falloff * light_diffuse) * s;
}