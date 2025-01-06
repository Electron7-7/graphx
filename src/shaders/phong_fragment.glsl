#version 460 core
out vec4 FragColor;

in vec3 _fragment_position;
in vec3 vertex_normal;
in vec2 vertex_texture_coordinate;

uniform vec3 light_position;
uniform vec3 view_position;
uniform mat3 normal_matrix;
uniform bool is_light;
uniform sampler2D texture_one;
uniform vec3 albedo;
uniform vec3 light_color;
uniform vec3 ambient_light;
// uniform vec3 ambient_light_color;
// uniform float ambient_light_strength; // 0.0 -> 1.0
uniform float specular_strength;
uniform int specular_sharpness; // shininess

void main()
{
	if(is_light)
	{
		FragColor = vec4(albedo, 1.0f);
		return;
	}

	vec3 normal = normal_matrix * normalize(vertex_normal);
	vec3 light_direction = normalize(light_position - _fragment_position);
	float diffuse = max(dot(normal, light_direction), 0.0f);

	vec3 view_direction = normalize(-_fragment_position);
	vec3 reflect_direction = reflect(-light_direction, normal);
	float specular = pow(max(dot(view_direction, reflect_direction), 0.0f), specular_sharpness);

	vec3 out_diffuse = diffuse * light_color;
	// vec3 out_ambient = ambient_light_color * ambient_light_strength;
	vec3 out_specular = specular * specular_strength * light_color;
	vec3 out_color = (albedo) * (out_diffuse + ambient_light + out_specular);

	FragColor = texture(texture_one, vertex_texture_coordinate) * vec4(out_color, 1.0f);
};