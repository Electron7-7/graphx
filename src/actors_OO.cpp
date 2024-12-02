#include "sanity.hpp"
#include "g_actors.hpp"
#include "r_common.hpp"

//
// Actor
//
Actor::Actor(const char* new_name, glm::vec3 init_position, glm::vec3 init_up, float init_yaw, float init_pitch) : orientation_front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(INIT_SPEED)
{
	name = new_name;
	position_global = init_position;
	world_orientation_up = init_up;
	rotation_euler.y = init_yaw;
	rotation_euler.x = init_pitch;
	rotation_euler.z = 0.0f; // roll
	updateActorVectors();
}

void Actor::updateActorVectors()
{
	glm::vec3 new_front;
	new_front.x = cos(glm::radians(rotation_euler.y)) * cos(glm::radians(rotation_euler.x));
	new_front.y = sin(glm::radians(rotation_euler.x));
	new_front.z = sin(glm::radians(rotation_euler.y)) * cos(glm::radians(rotation_euler.x));
	
	orientation_front = glm::normalize(new_front);
	orientation_right = glm::normalize(glm::cross(orientation_front, world_orientation_up));
	orientation_up = glm::normalize(glm::cross(orientation_right, orientation_front));
}

void Actor::doMovement(int direction[2], float delta_time)
{
	position_global += orientation_front * static_cast<float>(direction[0] * movement_speed * delta_time);
	position_global += orientation_right * static_cast<float>(direction[1] * movement_speed * delta_time);

	// std::cout << "(" << name << ") Position: " << glm::to_string(position_global) << std::endl;
}

//
// GraphXPlayer
//
GraphXPlayer::GraphXPlayer(const char* new_name, glm::vec3 init_position) : Actor(new_name, init_position), mouse_sensitivity(INIT_SENSITIVITY)
{}

void GraphXPlayer::doMouseMovement(float offset[2], GLboolean constrain_pitch)
{
	offset[0] *= mouse_sensitivity;
	offset[1] *= mouse_sensitivity;

	rotation_euler.y += offset[0];
	rotation_euler.x += offset[1];

	if(constrain_pitch)
	{
		if(std::abs(rotation_euler.x) > 89.0f)
			rotation_euler.x = 89.0f * ((rotation_euler.x > 0) - (rotation_euler.x < 0));
	}
	updateActorVectors();
}

glm::mat4 GraphXPlayer::getViewMatrix()
{
	return glm::lookAt(position_global, position_global + orientation_front, orientation_up);
}

//
// Tester
//
Tester::Tester() : Actor("Tester", glm::vec3(-3.0f, 0.0f, 0.0f))
{
	texture = "src/images/COMP04_5.png";
	render_init_cmd = new RenderInitializeCmd(texture, new float[DEFAULT_CUBE_VERTS_FLOATS], new unsigned int[DEFAULT_CUBE_INDICES_INTS], new int[DEFAULT_CUBE_ATTRIBUTES_X][DEFAULT_CUBE_ATTRIBUTES_Y]);
}

void Tester::flipPosition()
{
	position_flip = 1 - position_flip;
	position_global = testing_position[position_flip];
}

void Tester::makeCube()
{
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Idea: set GL_FALSE to GL_TRUE and use ints instead of floats for more efficient storage?
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int c_texture = T_GenerateTexture(texture);
	glBindTexture(GL_TEXTURE_2D, c_texture);
}

void Tester::drawCube()
{
	glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
}