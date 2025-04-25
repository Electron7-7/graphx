#ifndef GRAPHX_ACTOR
#define GRAPHX_ACTOR
#include "t_settings.hpp"
#include "graphx_namespace.hpp"
#include "r_common_fwd.hpp"
#include "g_common_fwd.hpp"
#include <glm/fwd.hpp>
#include <mutex>

extern bool loading_new_main_theatre;

struct RenderState
{   // Used by Actors to store/send position, rotation, and scale data
	glm::vec3 render_position = glm::vec3(0.0f);
	glm::quat render_quaternion = glm::quat();
	glm::vec3 render_scale = glm::vec3(0.0f);
};

class Actor
{
public:
	const graphx::gClass* const type = &graphx::gClass::INVALID_TYPE;

	Actor(const graphx::gClass*, const graphx::gUID&, const graphx::gSettings& = graphx::empty_settings);
	Actor(const graphx::gClass*, const int, const std::string& = "Untitled Actor", const graphx::gSettings& = graphx::empty_settings);
	Actor(const int, const std::string& = "Untitled Actor", const graphx::gSettings& = graphx::empty_settings);
	Actor(const graphx::gUID&, const graphx::gSettings& = graphx::empty_settings);

	virtual ~Actor();


	void updateStates(std::mutex&);

	graphx::gUID getUID() const;
	void setName(const std::string&);

	virtual const bool givesAFuckAboutPhysics() const;
	virtual void loadSettings(const graphx::gSettings& = empty_settings); // If no argument is provided, re-loads Actor::settings
	virtual RenderCommands getRenderCommands();
	virtual void processMouse(GLFWwindow*, double, double);
	virtual void processInput(GLFWwindow*);
	virtual void processKey(GLFWwindow*, int, int, int, int);
	virtual void tick(const int);

	void setGlobalPosition(const glm::vec3&);
	void setGlobalRotationAngles(const glm::vec3&, const bool = false);
	void setGlobalQuaternion(const glm::quat&);
	void setLocalPosition(const glm::vec3&);
	void setLocalRotationAngles(const glm::vec3&, const bool = false);
	void setLocalQuaternion(const glm::quat&);
	void setGlobalScale(const glm::vec3&);
	void setLocalScale(const glm::vec3&);

	glm::vec3 getGlobalPosition() const;
	glm::vec3 getGlobalRotationAngles(const bool = false) const;
	glm::quat getGlobalQuaternion() const;
	glm::vec3 getLocalPosition() const;
	glm::vec3 getLocalRotationAngles(const bool = false) const;
	glm::quat getLocalQuaternion() const;
	glm::vec3 getGlobalScale() const;
	glm::vec3 getLocalScale() const;

	void debug_highlight(const bool = true);

protected:
	graphx::gUID uid = graphx::gUID(-1, "Untitled Actor");
	graphx::gSettings settings = empty_settings;

	Collider* collider = nullptr;

	bool visible = true;

	Mesh* mesh = nullptr; // THIS IS GOING
	long mesh_uid = -1;   // THIS IS REPLACING IT

	glm::vec3 orientation_front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 orientation_grounded_front;
	glm::vec3 orientation_up;
	glm::vec3 orientation_right;
	glm::vec3 world_orientation_up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 position_global = glm::vec3(0.0f);
	glm::vec3 position_local = glm::vec3(0.0f);
	glm::quat quaternion_global = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::quat quaternion_local = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scale_global  = glm::vec3(1.0f);
	glm::vec3 scale_local = glm::vec3(1.0f);

	std::vector<RenderState> current_state_buffer;
	std::vector<RenderState> previous_state_buffer;
	int state_index = 0;

	glm::vec4 debug_highlight_color = glm::vec4(0.0f);

	virtual void updateVectors();
	virtual void selfOverrideColliderTransform(const bool = true);
	virtual void colliderOverrideSelfTransform(const bool = true);
};
#endif
