#version 460 core
#define MAX_NUMBER_OF_LIGHTS 50
out vec4 FragColor;

in vec2 texture_coordinate;
in vec3 fragment_position;
in vec3 normal;
in vec3 vertex_color;

struct Material
{
	sampler2D texture_diffuse;
	sampler2D texture_specular;

	vec3 color;
	float specular_strength;
	int specular_sharpness;
};

struct Environment
{
	float ambient_strength;
};

struct Light
{
	float strength;

	vec3 diffuse;
	vec3 specular;

	vec3 position;

	float range;
	float intensity;
	float falloff;

	float inner_cutoff;
	float outer_cutoff;
	vec3 direction;
};

uniform Material material;
uniform Environment environment;

uniform Light directional_light;
uniform Light point_lights[MAX_NUMBER_OF_LIGHTS];
uniform Light spot_lights[MAX_NUMBER_OF_LIGHTS];

uniform vec3 view_position;
uniform bool is_light;
uniform int point_lights_count;
uniform int spot_lights_count;
uniform bool mat_fullbright;
uniform bool is_primitive;

#define DEBUG_NORMALS       1
#define DEBUG_VERTEX_COLORS 2

uniform float enable_diffuse;
uniform float enable_specular;
uniform int shader_debug_value; // Show Normals or Vertex Colors?

vec3 material_diffuse;
vec3 material_specular;
vec3 view_direction;

vec3 calculateSpotLight(Light light);
vec3 calculatePointLight(Light light);
vec3 calculateDirectionalLight(Light light);
mat2x3 calculateLight(Light light, vec3 light_direction);

void main()
{
	if(is_light)
	{
		FragColor = vec4(texture(material.texture_diffuse, texture_coordinate).rgb * material.color, 1.0f);
		return;
	}

	if(is_primitive)
	{
		// No support for primitive textures/lighting, yet
		FragColor = vec4(vertex_color, 1.0f);
		return;
	}

	if(shader_debug_value == DEBUG_NORMALS)
	{
		FragColor = vec4((normalize(normal) + vec3(1.0f)) / vec3(2.0f), 1.0f);
		return;
	}

	if(shader_debug_value == DEBUG_VERTEX_COLORS)
	{
		FragColor = vec4(vertex_color, 1.0f);
		return;
	}

	if(mat_fullbright)
	{
		FragColor = vec4(texture(material.texture_diffuse, texture_coordinate).rgb * material.color, 1.0f);
		return;
	}

	material_diffuse = texture(material.texture_diffuse, texture_coordinate).rgb;
	material_specular = texture(material.texture_specular, texture_coordinate).rgb;
	view_direction = normalize(view_position - fragment_position);

	vec3 output_color = calculateDirectionalLight(directional_light);

	for(int i = 0 ; i < point_lights_count ; i++)
		output_color += calculatePointLight(point_lights[i]);

	for(int i = 0 ; i < spot_lights_count ; i++)
		output_color += calculateSpotLight(spot_lights[i]);

	FragColor = vec4(output_color, 1.0f);
}

vec3 calculateSpotLight(Light light)
{
	vec3 light_direction = normalize(light.position - fragment_position);
	float light_distance = length(light.position - fragment_position);
	mat2x3 light_components = calculateLight(light, light_direction);

	// How I got the numbers in this attenuation calculation:
	// 		https://www.desmos.com/calculator/vtbqukgvgp
	// Where I got the starting data:
	//		https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
	float linear = (5.58574 * pow(light.range, -1.0704)) + (light.falloff * 0.01f);
	float quadratic = 131.35334 * pow(light.range, -2.20532);
	float light_attenuation = 1.0f / (light.intensity + linear * light_distance + quadratic * pow(light_distance, 2));

	float theta = dot(light_direction, normalize(-light.direction));
	float epsilon = light.inner_cutoff - light.outer_cutoff;
	float spotlight_radius = clamp((theta - light.outer_cutoff) / epsilon, 0.0f, 1.0f);

	vec3 this_diffuse = light_components[0];
	vec3 this_specular = light_components[1];

	this_diffuse *= light_attenuation * spotlight_radius  * enable_diffuse;
	this_specular *= light_attenuation * spotlight_radius * enable_specular;

	return (this_diffuse + this_specular);
}

vec3 calculatePointLight(Light light)
{
	float light_distance = length(light.position - fragment_position);
	vec3 light_direction = normalize(light.position - fragment_position);
	mat2x3 light_components = calculateLight(light, light_direction);

	// How I got the numbers in this attenuation calculation:
	// 		https://www.desmos.com/calculator/vtbqukgvgp
	// Where I got the starting data:
	//		https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
	float linear = (5.58574 * pow(light.range, -1.0704)) + (light.falloff * 0.01f);
	float quadratic = 131.35334 * pow(light.range, -2.20532);
	float light_attenuation = 1.0f / (light.intensity + linear * light_distance + quadratic * pow(light_distance, 2));

	vec3 this_diffuse = light_components[0];
	vec3 this_specular = light_components[1];

	this_diffuse *= light_attenuation  * enable_diffuse;
	this_specular *= light_attenuation * enable_specular;

	return (this_diffuse + this_specular);
}

vec3 calculateDirectionalLight(Light light)
{
	vec3 light_direction = normalize(-light.direction);
	mat2x3 light_components = calculateLight(light, light_direction);

	vec3 this_diffuse = light_components[0]  * enable_diffuse;
	vec3 this_specular = light_components[1] * enable_specular;

	return (this_diffuse + this_specular);
}

mat2x3 calculateLight(Light light, vec3 light_direction)
{
	float light_distance = length(light.position - fragment_position);
	vec3 reflect_direction = reflect(-light_direction, normalize(normal));
	vec3 blinn_halfway_vector = normalize(light_direction + view_direction);

	float diffuse = max(dot(normalize(normal), light_direction), 0.0f/* environment.ambient_strength */);
	float specular = pow(max(dot(normalize(normal), blinn_halfway_vector), 0.0f/* environment.ambient_strength */), material.specular_sharpness);

	vec3 this_diffuse  = light.strength * light.diffuse * vertex_color * material_diffuse * material.color * diffuse;
	vec3 this_specular = light.strength * light.specular * material_specular * material.specular_strength * specular;

	return mat2x3(this_diffuse, this_specular);
}