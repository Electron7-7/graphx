#include <string>
std::string blinn_phong_frag = R"~(
// Blinn Phong Fragment Shader
#version 460 core
#define MAX_NUMBER_OF_LIGHTS 100

out vec4 FragColor;

in vec3 fragment_position;
in vec3 vertex_normal;
in vec2 vertex_uv;
in vec3 vertex_color;

struct Environment
{
	vec3 ambient_light_color;
	float ambient_light_contribution;
};

struct Material
{
	vec3 diffuse_color;
	sampler2D texture_diffuse;

	// Until I implement metallic, roughness, etc I'll just directly affect the specular
	float specular_strength;
	int specular_sharpness;
	sampler2D texture_specular;

	// These will eventually replace the specular variables, as they'll each affect the specular highlights in different ways
	// sampler2D texture_metallic;
	// float metallic_strength;
	// sampler2D texture_roughness;
	// float roughness_strength;
};

struct Light
{
	vec3 color;
	float energy;
	float specular_strength;
	float ambient_strength;

	float attenuation;
	float range;

	vec3 position;
	vec3 direction;

	float spot_cutoff;
	float spot_cutoff_fade;

	// Texture *projection_texture;
};

uniform Material current_material;
uniform Environment current_environment;

uniform Light directional_lights[MAX_NUMBER_OF_LIGHTS];
uniform Light point_lights[MAX_NUMBER_OF_LIGHTS];
uniform Light spot_lights[MAX_NUMBER_OF_LIGHTS];

uniform int directional_lights_count; // Honestly, should only be 1, but you do you, I guess
uniform int point_lights_count;
uniform int spot_lights_count;

uniform int enable_ambient;
uniform int enable_diffuse;
uniform int enable_specular;

uniform vec3 view_position;

vec3 calculatePointLight(Light light);
vec3 calculateSpotLight(Light light);
vec3 calculateDirectionalLight(Light light);
float calculateAttenuation(float range, float constant, float distance);
mat3x3 calculateLuminosity(Light light, vec3 light_direction);

void main()
{
	vec3 output_color = vec3(0.0f);

	for(int i = 0 ; i < directional_lights_count ; i++)
		output_color += calculateDirectionalLight(directional_lights[i]);
	for(int i = 0 ; i < point_lights_count ; i++)
		output_color += calculatePointLight(point_lights[i]);
	for(int i = 0 ; i < spot_lights_count ; i++)
		output_color += calculateSpotLight(spot_lights[i]);

	FragColor = vec4(output_color, 1.0f);
}

vec3 calculatePointLight(Light light)
{
	mat3x3 light_components = calculateLuminosity(light, normalize(light.position - fragment_position));
	float attenuation = calculateAttenuation(light.range, light.attenuation, length(light.position - fragment_position));

	light_components[0] *= attenuation;
	light_components[1] *= attenuation;
	light_components[2] *= attenuation;

	return light_components[0] + light_components[1] + light_components[2];
}

vec3 calculateSpotLight(Light light)
{
	vec3 light_direction = normalize(light.position - fragment_position);
	mat3x3 light_components = calculateLuminosity(light, light_direction);
	float attenuation = calculateAttenuation(light.range, light.attenuation, length(light.position - fragment_position));

	float theta = dot(light_direction, normalize(-light.direction));
	float epsilon = light.spot_cutoff_fade - light.spot_cutoff;
	float spotlight_radius = clamp((theta - light.spot_cutoff) / epsilon, 0.0f, 1.0f);

	light_components[1] *= attenuation * spotlight_radius;
	light_components[2] *= attenuation * spotlight_radius;

	return (light_components[0] + light_components[1] + light_components[2]);
}

vec3 calculateDirectionalLight(Light light)
{
	mat3x3 light_components = calculateLuminosity(light, normalize(-light.direction));
	return (light_components[0] + light_components[1] + light_components[2]);
}

mat3x3 calculateLuminosity(Light light, vec3 light_direction)
{
	float light_distance = length(light.position - fragment_position);
	vec3 reflect_direction = reflect(-light_direction, normalize(vertex_normal));
	vec3 blinn_halfway_vector = normalize(light_direction + normalize(view_position - fragment_position));

	float diffuse = max(dot(normalize(vertex_normal), light_direction), 0.0f);
	float specular = pow(max(dot(normalize(vertex_normal), blinn_halfway_vector), 0.0f), current_material.specular_sharpness);

	specular = diffuse != 0 ? specular : 0.0f;

	vec3 this_ambient = light.ambient_strength * light.color * texture(current_material.texture_diffuse, vertex_uv).rgb * current_material.diffuse_color * vertex_color;
	vec3 this_diffuse = light.energy * light.color * texture(current_material.texture_diffuse, vertex_uv).rgb * current_material.diffuse_color * vertex_color * diffuse;
	vec3 this_specular = light.specular_strength * light.color * texture(current_material.texture_specular, vertex_uv).rgb * current_material.specular_strength * specular;

	return mat3x3(this_ambient * enable_ambient, this_diffuse * enable_diffuse, this_specular * enable_specular);
}

float calculateAttenuation(float range, float constant, float distance)
{
	// How I got the numbers in this attenuation calculation:
	// 		https://www.desmos.com/calculator/vtbqukgvgp
	// Where I got the starting data:
	//		https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
	float linear = (4.69051 * pow(range, -1.00971));
	float quadratic = 82.44478 * pow(range, -2.01921);
	return (1.0f / (constant + linear * distance + quadratic * pow(distance, 2)));
}

)~";
std::string blinn_phong_vert = R"~(
// Blinn Phong Vertex Shader
#version 460 core
layout (location = 0) in vec3 _vertex_position;
layout (location = 1) in vec3 _vertex_normal;
layout (location = 2) in vec2 _vertex_uv;
layout (location = 3) in vec3 _vertex_color;

out vec3 fragment_position;
out vec3 vertex_normal;
out vec2 vertex_uv;
out vec3 vertex_color;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat3 normal_matrix;

void main()
{
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(_vertex_position, 1.0f);
	fragment_position = vec3(model_matrix * vec4(_vertex_position, 1.0f));
	vertex_normal = normal_matrix * _vertex_normal;
	vertex_uv = _vertex_uv;
	vertex_color = _vertex_color;
}
)~";
std::string debug_normals_frag = R"~(
#version 460 core
out vec4 FragColor;

in vec3 fragment_position;
in vec3 vertex_normal;
in vec2 vertex_uv;
in vec3 vertex_color;

void main()
{
	FragColor = vec4((normalize(vertex_normal) + vec3(1.0f)) / vec3(2.0f), 1.0f);
}
)~";
std::string debug_vertex_colors_frag = R"~(
#version 460 core
out vec4 FragColor;

in vec3 fragment_position;
in vec3 vertex_normal;
in vec2 vertex_uv;
in vec3 vertex_color;

void main()
{
	FragColor = vec4(vertex_color, 1.0f);
}
)~";
std::string font2d_frag = R"~(
// 2D Font Fragment Shader
#version 460 core
in vec2 vertex_uv;

out vec4 FragColor;

uniform sampler2D glyph_texture;
uniform vec3 text_color;

void main()
{
	float glyph_shape = texture(glyph_texture, vertex_uv).r;
	if(glyph_shape < 0.5)
		discard;
	FragColor = vec4(text_color, 1.0f);
}
)~";
std::string font2d_vert = R"~(
// 2D Font Vertex Shader
#version 460 core
layout (location = 0) in vec2 _vertex_position;
layout (location = 1) in vec2 _vertex_uv;

out vec2 vertex_uv;

uniform mat4 projection_matrix;

void main()
{
	gl_Position = projection_matrix * vec4(_vertex_position, 0.0f, 1.0f);
	vertex_uv = _vertex_uv;
}
)~";
std::string font3d_frag = R"~(
// 3D Font Fragment Shader
#version 460 core
in vec2 vertex_uv;

out vec4 FragColor;

uniform sampler2D glyph_texture;
uniform vec3 text_color;

void main()
{
	float glyph_shape = texture(glyph_texture, vertex_uv).r;
	if(glyph_shape < 0.5)
		discard;
	FragColor = vec4(text_color, 1.0f);
}
)~";
std::string font3d_vert = R"~(
// 3D Font Vertex Shader
#version 460 core
layout (location = 0) in vec2 _vertex_position;
layout (location = 1) in vec2 _vertex_uv;

out vec2 vertex_uv;

uniform vec2 glyph_scale;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main()
{
	// https://stackoverflow.com/a/62629272
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vec2(_vertex_position / glyph_scale / 2) - vec2(1.0f), 0.0f, 1.0f);
	vertex_uv = _vertex_uv.xy;
}
)~";
std::string light_debug_frag = R"~(
#version 460 core
out vec4 FragColor;

in vec3 fragment_position;
in vec3 vertex_normal;
in vec2 vertex_uv;
in vec3 vertex_color;

struct Material
{
	vec3 diffuse_color;
	sampler2D texture_diffuse;
};

uniform Material current_material;

void main()
{
	FragColor = vec4(texture(current_material.texture_diffuse, vertex_uv).rgb * current_material.diffuse_color, 1.0f);
}
)~";
std::string old_blinn_phong_frag = R"~(
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
uniform bool is_light; // Make this a separate shader
uniform int point_lights_count;
uniform int spot_lights_count;
uniform bool mat_fullbright;
uniform bool is_primitive; // Make this a separate shader

#define DEBUG_NORMALS       1
#define DEBUG_VERTEX_COLORS 2

uniform float enable_diffuse;
uniform float enable_specular;
uniform int shader_debug_value; // Make this a separate shader

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

	float diffuse = max(dot(normalize(normal), light_direction), 0.0f);
	float specular = pow(max(dot(normalize(normal), blinn_halfway_vector), 0.0f), material.specular_sharpness);

	vec3 this_diffuse  = light.strength * light.diffuse * vertex_color * material_diffuse * material.color * diffuse;
	vec3 this_specular = light.strength * light.specular * material_specular * material.specular_strength * specular;

	return mat2x3(this_diffuse, this_specular);
}
)~";
std::string old_blinn_phong_vert = R"~(
#version 460 core
layout (location = 0) in vec3 _vertex_position;
layout (location = 1) in vec3 _vertex_normal;
layout (location = 2) in vec2 _vertex_texture_coordinate;
layout (location = 3) in vec3 _vertex_color;

out vec3 fragment_position;
out vec2 texture_coordinate;
out vec3 vertex_color;
out vec3 normal;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat3 normal_matrix;

void main()
{
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(_vertex_position, 1.0f);
	texture_coordinate = _vertex_texture_coordinate;
	fragment_position = vec3(model_matrix * vec4(_vertex_position, 1.0f)); // Transforming vertex position from local to global coordinates
	normal = normal_matrix * _vertex_normal;
	vertex_color = _vertex_color;
}
)~";
std::string phong_frag = R"~(
#version 460 core
#define MAX_NUMBER_OF_LIGHTS 20

in vec2 texture_coordinate;
in vec3 fragment_position;
in vec3 normal;
in vec3 vertex_colors;

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
uniform bool is_primitive;

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
	if(is_primitive)
	{
		// No support for primitive textures/lighting, yet
		FragColor = vec4(vertex_colors, 1.0f);
		return;
	}

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

	float diffuse = max(dot(normalize(normal), light_direction), 0.0f);
	float specular = pow(max(dot(view_direction, reflect_direction), 0.0f), material.specular_sharpness);

	vec3 this_diffuse  = light.strength * light.color * material_diffuse * material.color * diffuse;
	vec3 this_specular = light.color * material_specular * material.color * material.specular_strength * specular;
	vec3 this_ambient  = light.ambient_strength * light.color * material_diffuse * material.color * environment.ambient_light;

	mat3x3 light_components;
	light_components[0] = this_diffuse;
	light_components[1] = this_specular;
	light_components[2] = this_ambient;

	return light_components;
};
)~";
std::string phong_vert = R"~(
#version 460 core
layout (location = 0) in vec3 _vertex_position;
layout (location = 1) in vec3 _vertex_normal;
layout (location = 2) in vec2 _vertex_texture_coordinate;
layout (location = 3) in vec3 _vertex_colors;

out vec3 fragment_position;
out vec2 texture_coordinate;
out vec3 vertex_colors;
out vec3 normal;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat3 normal_matrix;

void main()
{
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(_vertex_position, 1.0);
	texture_coordinate = _vertex_texture_coordinate;
	fragment_position = vec3(model_matrix * vec4(_vertex_position, 1.0f)); // Transforming vertex position from local to global coordinates
	normal = normal_matrix * _vertex_normal;
	vertex_colors = _vertex_colors;
};
)~";
std::string skybox_frag = R"~(
#version 460 core
out vec4 FragColor;

in vec3 skybox_uv;

uniform samplerCube skybox;

void main()
{
	FragColor = texture(skybox, -skybox_uv);
}
)~";
std::string skybox_vert = R"~(
#version 460 core
layout (location = 0) in vec3 _skybox_vertex_position;

out vec3 skybox_uv;

uniform mat4 skybox_projection_matrix;
uniform mat4 skybox_view_matrix;

void main()
{
	skybox_uv = _skybox_vertex_position;
	vec4 position = skybox_projection_matrix * skybox_view_matrix * vec4(_skybox_vertex_position, 1.0f);
	gl_Position = position.xyww;
}
)~";
