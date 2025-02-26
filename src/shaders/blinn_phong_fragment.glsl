#version 460 core
#define MAX_NUMBER_OF_LIGHTS 20
out vec4 FragColor;

in vec2 texture_coordinate;
in vec3 fragment_position;
in vec3 normal;

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
	vec3 ambient_light;
	vec3 ambient_color;
	float ambient_strength;
};

struct Light
{
	float strength;
	float ambient_strength;

	vec3 color;
	vec3 position;
	vec3 direction;

	float range;
	float intensity;
	float falloff;

	float inner_cutoff;
	float outer_cutoff;
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

#define DEBUG_DIFFUSE 1
#define DEBUG_SPECULAR 2
#define DEBUG_AMBIENT 3
#define DEBUG_ALL 4
#define DEBUG_NORMALS 5

uniform int shader_debug_value;

vec3 material_diffuse;
vec3 material_specular;
vec3 view_direction;

vec3 calculateSpotLight(Light light);
vec3 calculatePointLight(Light light);
vec3 calculateDirectionalLight(Light light);
mat3x3 calculateLight(Light light, vec3 light_direction);

void main()
{
	if(shader_debug_value == DEBUG_NORMALS)
	{
		FragColor = vec4((normalize(normal) + vec3(1.0f)) / vec3(2.0f), 1.0f);
		return;
	}

	if(mat_fullbright || is_light)
	{
		FragColor = vec4(material.color, 1.0f);
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
};

vec3 calculateSpotLight(Light light)
{
	vec3 light_direction = normalize(light.position - fragment_position);
	float light_distance = length(light.position - fragment_position);
	mat3x3 light_components = calculateLight(light, light_direction);

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
	vec3 this_ambient = light_components[2];

	this_diffuse *= light_attenuation * spotlight_radius;
	this_specular *= light_attenuation * spotlight_radius;
	this_ambient *= light_attenuation * spotlight_radius;

	if(shader_debug_value == DEBUG_DIFFUSE)
		return this_diffuse;
	if(shader_debug_value == DEBUG_SPECULAR)
		return this_specular;
	if(shader_debug_value == DEBUG_AMBIENT)
		return this_ambient;
	return (this_diffuse + this_specular + this_ambient);
};

vec3 calculatePointLight(Light light)
{
	float light_distance = length(light.position - fragment_position);
	vec3 light_direction = normalize(light.position - fragment_position);
	mat3x3 light_components = calculateLight(light, light_direction);

	// How I got the numbers in this attenuation calculation:
	// 		https://www.desmos.com/calculator/vtbqukgvgp
	// Where I got the starting data:
	//		https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
	float linear = (5.58574 * pow(light.range, -1.0704)) + (light.falloff * 0.01f);
	float quadratic = 131.35334 * pow(light.range, -2.20532);
	float light_attenuation = 1.0f / (light.intensity + linear * light_distance + quadratic * pow(light_distance, 2));

	vec3 this_diffuse = light_components[0];
	vec3 this_specular = light_components[1];
	vec3 this_ambient = light_components[2];

	this_diffuse *= light_attenuation;
	this_specular *= light_attenuation;
	this_ambient *= light_attenuation;

	if(shader_debug_value == DEBUG_DIFFUSE)
		return this_diffuse;
	if(shader_debug_value == DEBUG_SPECULAR)
		return this_specular;
	if(shader_debug_value == DEBUG_AMBIENT)
		return this_ambient;
	return (this_diffuse + this_specular + this_ambient);
};

vec3 calculateDirectionalLight(Light light)
{
	vec3 light_direction = normalize(-light.direction);
	mat3x3 light_components = calculateLight(light, light_direction);

	vec3 this_diffuse = light_components[0];
	vec3 this_specular = light_components[1];
	vec3 this_ambient = light_components[2];

	if(shader_debug_value == DEBUG_DIFFUSE)
		return this_diffuse;
	if(shader_debug_value == DEBUG_SPECULAR)
		return this_specular;
	if(shader_debug_value == DEBUG_AMBIENT)
		return this_ambient;
	return (this_diffuse + this_specular + this_ambient);
};

mat3x3 calculateLight(Light light, vec3 light_direction)
{
	float light_distance = length(light.position - fragment_position);
	vec3 reflect_direction = reflect(-light_direction, normalize(normal));
	vec3 blinn_halfway_vector = normalize(light_direction + view_direction);

	float diffuse = max(dot(normalize(normal), light_direction), 0.0f);
	float specular = pow(max(dot(normalize(normal), blinn_halfway_vector), 0.0f), material.specular_sharpness);

	vec3 this_diffuse  = light.strength * light.color * material_diffuse * material.color * diffuse;
	vec3 this_specular = light.color * material_specular * material.color * material.specular_strength * specular;
	vec3 this_ambient  = light.ambient_strength * light.color * material_diffuse * material.color * environment.ambient_light;

	mat3x3 light_components;
	light_components[0] = this_diffuse;
	light_components[1] = this_specular;
	light_components[2] = this_ambient;

	return light_components;
};