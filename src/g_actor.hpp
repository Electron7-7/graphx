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
	bool visible = true;

	Mesh* mesh = nullptr; // THIS IS GOING
	long mesh_uid = -1;   // THIS IS REPLACING IT

	glm::vec3 scale = glm::vec3(1.0f);

	glm::vec3 orientation_front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 orientation_grounded_front;
	glm::vec3 orientation_up;
	glm::vec3 orientation_right;
	glm::vec3 world_orientation_up = glm::vec3(0.0f, 1.0f, 0.0f);

	std::vector<RenderState> current_state_buffer;
	std::vector<RenderState> previous_state_buffer;

	int state_index = 0;

	graphx::gSettings settings = empty_settings;

	Actor(const graphx::gUID&, const graphx::gSettings&);
	Actor(const long, const std::string = "Untitled Actor", Mesh* = nullptr, glm::vec3 = glm::vec3(0.0f), glm::vec3 = glm::vec3(0.0f), glm::vec3 = glm::vec3(1.0f));
	virtual ~Actor() = default;

	template<typename T> T getPosition();
	template<typename T> T getRotation();
	template<typename T> T getRotationDegrees();

	// Todo: convert these to function templates
	virtual void setGlobalPosition(glm::vec3);
	virtual void setGlobalPosition(JPH::Vec3);
	virtual void setGlobalRotation(glm::vec3);
	virtual void setGlobalRotation(glm::quat);
	virtual void setGlobalRotation(JPH::Vec3);
	virtual void setGlobalRotation(JPH::Quat);
	virtual void setLocalPosition(glm::vec3);
	virtual void setLocalPosition(JPH::Vec3);
	virtual void setLocalRotation(glm::vec3);
	virtual void setLocalRotation(glm::quat);
	virtual void setLocalRotation(JPH::Vec3);
	virtual void setLocalRotation(JPH::Quat);

	void setUID(const graphx::gUID&);
	void setName(const std::string&);
	const graphx::gUID& getUID() const;
	const graphx::gClass* getType() const;
	// std::string getName() const;
	// void setName(const std::string);
	// void setName(const char*);

	void highlightMe();
	void unHighlightMe();

	virtual RenderCommands getRenderCommands();
	virtual bool isPhysicsActor();
	virtual void youGotACallBack(graphx::gSettings = empty_settings); // Loads settings
	virtual void callToStage(Theatre*);
	virtual void takeABow();
	virtual void processMouse(GLFWwindow*, double, double);
	virtual void processInput(GLFWwindow*);
	virtual void processKey(GLFWwindow*, int, int, int, int);
	virtual void tick(int);
	virtual void updateStates(std::mutex&);

protected:
	glm::vec3 position_global = glm::vec3(0.0f);
	glm::vec3 position_local = glm::vec3(0.0f);
	glm::quat quaternion = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::quat local_quaternion = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	graphx::gUID uid = graphx::gUID(-1, "Untitled Actor");
	const graphx::gClass* my_type = nullptr;
	glm::vec4 debug_highlight_color = glm::vec4(0.0f);

	virtual void updateVectors();
};
#endif
