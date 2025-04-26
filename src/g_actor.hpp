#ifndef GRAPHX_ACTOR
#define GRAPHX_ACTOR
#include "graphx_namespace.hpp"
#include "graphx_classes.hpp"
#include "r_common_fwd.hpp"
#include <glm/vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <mutex>

namespace graphx
{
	namespace orientation
	{
		inline const unsigned int UP    = 0;
		inline const unsigned int FRONT = 1;
		inline const unsigned int RIGHT = 2;
	}
}

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
	const graphx::gClass& type = graphx::classes::ACTOR;

	Actor(const graphx::gClass&, const graphx::gID& = graphx::gID("Untitled Actor"), const graphx::gSettings& = graphx::gSettings());
	virtual ~Actor();

	// Virtual functions
	virtual const bool givesAFuckAboutPhysics() const;
	virtual void loadSettings();
	virtual RenderCommands getRenderCommands();
	virtual void checkForInput(GLFWwindow* window);
	virtual void processMouse(GLFWwindow* window, double x_position_in, double y_position_in);
	virtual void processKey(GLFWwindow* window, int key, int scancode, int action, int mods);
	virtual void tick(const int current_tick);

	graphx::gID getID() const;
	void setName(const std::string&);
	void setUID(const int);
	void updateStates(std::mutex&);
	graphx::gSettings getSettings() const;
	void setSettings(const graphx::gSettings&);

	void setGlobalPosition(const glm::vec3&);
	void setGlobalRotationAngles(const glm::vec3&, const bool degrees_instead_of_radians = false);
	void setGlobalQuaternion(const glm::quat&);
	void setLocalPosition(const glm::vec3&);
	void setLocalRotationAngles(const glm::vec3&, const bool degrees_instead_of_radians = false);
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

private:
	graphx::gID name_and_uid = graphx::gID(-1, "Untitled Actor");

	glm::vec3 orientation_up = graphx::global::orientation_up;
	glm::vec3 orientation_front = graphx::global::orientation_front;
	glm::vec3 orientation_right = graphx::global::orientation_right;

	glm::vec3 position_global = glm::vec3(0.0f);
	glm::vec3 position_local = glm::vec3(0.0f);
	glm::quat quaternion_global = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::quat quaternion_local = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scale_global  = glm::vec3(1.0f);
	glm::vec3 scale_local = glm::vec3(1.0f);

protected:
	graphx::gSettings settings = graphx::gSettings();

	Collider* collider = nullptr; // I SHOULD USE THE UID IDEA THAT I'M USING FOR MESH FOR COLLIDER

	bool visible = true;

	Model* mesh = nullptr; // THIS IS GOING
	long mesh_uid = -1;   // THIS IS REPLACING IT

	std::vector<RenderState> current_state_buffer;
	std::vector<RenderState> previous_state_buffer;
	int state_index = 0;

	glm::vec4 debug_highlight_color = glm::vec4(0.0f);

	void updateOrientationVectors();
	glm::vec3 getOrientation(const unsigned int);
	void selfOverrideColliderTransform(const bool = true);
	void colliderOverrideSelfTransform(const bool = true);
};
#endif
