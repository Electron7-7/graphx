#version 460 core
#define MAX_NUMBER_OF_LIGHTS 20
out vec4 FragColor;

in vec2 texture_coordinate;
in vec3 fragment_position;
in vec3 normal;

struct Material
{
	vec3 diffuse;
	vec3 specular;
	vec3 color;
	float specular_strength;
	int specular_sharpness;
} material;

// #define ENVIRONMENT_BACKGROUND_COLOR	0
// #define ENVIRONMENT_BACKGROUND_IMAGE	1
struct Environment
{
	// int background_type;
	vec3 ambient_light;
} environment;

struct Light
{
	vec3 color;
	vec3 specular;
	vec3 position;
	vec3 direction;

	float range;
	float intensity;
	float falloff;

	float inner_cutoff;
	float outer_cutoff;
} directional_light;

uniform Light point_lights[MAX_NUMBER_OF_LIGHTS];
uniform Light spot_lights[MAX_NUMBER_OF_LIGHTS];

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

uniform vec3 view_position;
uniform bool is_light;
uniform int point_lights_count;
uniform int spot_lights_count;

vec3 view_direction = normalize(view_position - fragment_position);

vec3 calculateDirectionalLight();
vec3 calculateLight(Light light, bool is_spot_light);

void main()
{
	material.diffuse = texture(texture_diffuse, texture_coordinate).rgb;
	material.specular = texture(texture_specular, texture_coordinate).rgb;

	if(is_light)
	{
		FragColor = vec4(material.diffuse * material.color, 1.0f);
		return;
	}

	vec3 output_color = calculateDirectionalLight();

	for(int i = 0 ; i < point_lights_count ; i++)
		output_color += calculateLight(point_lights[i], false);

	for(int i = 0 ; i < spot_lights_count ; i++)
		output_color += calculateLight(spot_lights[i], true);

	FragColor = vec4(output_color, 1.0f);
};

vec3 calculateDirectionalLight()
{
	vec3 light_direction = normalize(-directional_light.direction);
	float diffuse = max(dot(normal, light_direction), 0.0f);
	vec3 reflect_direction = reflect(-light_direction, normal);
	float specular = pow(max(dot(view_direction, reflect_direction), 0.0f), material.specular_sharpness);

	vec3 this_ambient = directional_light.color * environment.ambient_light * material.diffuse * material.color;
	vec3 this_diffuse = directional_light.color * diffuse * material.diffuse * material.color;
	vec3 this_specular = directional_light.specular * material.specular * material.specular_strength;

	return (this_ambient + this_diffuse + this_specular);
}

vec3 calculateLight(Light light, bool is_spot_light)
{
	vec3 light_direction = normalize(light.position - fragment_position);
	float light_distance = length(light.position - fragment_position);
	vec3 reflect_direction = reflect(-light_direction, normal);
	
	float diffuse = max(dot(normal, light_direction), 0.0f);
	float specular = pow(max(dot(view_direction, reflect_direction), 0.0f), material.specular_sharpness);
	
	// How I got the numbers in this attenuation calculation:
	// 		https://www.desmos.com/calculator/vtbqukgvgp
	// Where I got the starting data:
	//		https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
	float linear = (5.58574 * pow(light.range, -1.0704)) + (light.falloff * 0.01f);
	float quadratic = 131.35334 * pow(light.range, -2.20532);
	float light_attenuation = 1.0f / (light.intensity + linear * light_distance + quadratic * pow(light_distance, 2));
	
	float spotlight_radius = 1.0f;
	if(is_spot_light)
	{
		float theta = dot(light_direction, normalize(-light.direction));
		float epsilon = light.inner_cutoff - light.outer_cutoff;
		spotlight_radius = clamp((theta - light.outer_cutoff) / epsilon, 0.0f, 1.0f);
	}

	vec3 this_ambient = light.color * environment.ambient_light * material.diffuse * material.color;
	vec3 this_diffuse = light.color * diffuse * material.diffuse * material.color;
	vec3 this_specular = light.specular * material.specular * material.specular_strength;

	this_ambient *= light_attenuation * spotlight_radius;
	this_diffuse *= light_attenuation * spotlight_radius;
	this_specular *= light_attenuation * spotlight_radius;

	return (this_ambient + this_diffuse + this_specular);
}