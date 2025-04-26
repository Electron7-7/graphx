#ifndef GRAPHX_ACTOR
#define GRAPHX_ACTOR
#include "graphx_namespace.hpp"
#include "g_devices.hpp" // TODO: CHANGE HOW CHILD DEVICES WORK SO I DON'T NEED THIS
#include "r_common.hpp"
#include <glm/vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glfw_fwd.hpp>
#include <mutex>
#include <vector>

struct ActorPointerWrapper
{
	Actor* pointer;
	bool owned_by_me;

	ActorPointerWrapper(Actor*, const bool);
};

class Actor
{
public:
	bool debug_highlight_enabled = false;

	inline static const unsigned int ORIENTATION_UP    = 0;
	inline static const unsigned int ORIENTATION_FRONT = 1;
	inline static const unsigned int ORIENTATION_RIGHT = 2;

	std::string name = "Untitled Actor";

	// The constructor that should be used 99% of the time
	Actor(const std::string& Name = "Untitled Actor");

	// The constructor that a Theatre uses when creating Actors
	Actor(Theatre* ParentTheatre, const int UID, const graphx::gSettings& Settings = graphx::gSettings());

	virtual ~Actor();

	void setUID(const int);
	int getUID() const;
	void updateStates(std::mutex&);
	graphx::gSettings getSettings() const;
	void setSettings(const graphx::gSettings&);

	void addChildActor(Actor*);
	void addChildDevice(Device*);

	Actor* getChildActor(const int);
	Device* getChildDevice(const int);

	void setGlobalPosition(const glm::vec3&);
	void setGlobalRotationAngles(const glm::vec3&, const bool degrees_instead_of_radians = false);
	void setGlobalQuaternion(const glm::quat&);
	void setLocalPosition(const glm::vec3&);
	void setLocalRotationAngles(const glm::vec3&, const bool degrees_instead_of_radians = false);
	void setLocalQuaternion(const glm::quat&);
	void setGlobalScale(const glm::vec3&);
	void setLocalScale(const glm::vec3&);

	glm::vec3 getOrientation(const unsigned int) const;
	glm::vec3 getGlobalPosition() const;
	glm::vec3 getGlobalRotationAngles(const bool = false) const;
	glm::quat getGlobalQuaternion() const;
	glm::vec3 getLocalPosition() const;
	glm::vec3 getLocalRotationAngles(const bool = false) const;
	glm::quat getLocalQuaternion() const;
	glm::vec3 getGlobalScale() const;
	glm::vec3 getLocalScale() const;

	// Virtual functions
	virtual void tick(const int current_tick);
	virtual void loadSettings();
	virtual RenderCommands getRenderCommands();
	virtual const bool givesAFuckAboutPhysics() const;
	virtual void checkForInput(GLFWwindow* window);
	virtual void processMouse(GLFWwindow* window, double x_position_in, double y_position_in);
	virtual void processKey(GLFWwindow* window, int key, int scancode, int action, int mods);

	// Only used for debugging purposes; will return "Actor" if not implemented
	virtual std::string getTypeName() const;

protected: // Members that aren't externally accessible
	graphx::gSettings settings = graphx::gSettings();

	std::vector<ActorPointerWrapper> child_actors;
	std::vector<DevicePointerWrapper> child_devices;

	glm::vec3 orientation_up = graphx::global::variables::orientation_up;
	glm::vec3 orientation_front = graphx::global::variables::orientation_front;
	glm::vec3 orientation_right = graphx::global::variables::orientation_right;

	glm::vec3 position_global = glm::vec3(0.0f);
	glm::vec3 position_local = glm::vec3(0.0f);
	glm::quat quaternion_global = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::quat quaternion_local = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scale_global  = glm::vec3(1.0f);
	glm::vec3 scale_local = glm::vec3(1.0f);

	Collider* collider = nullptr; // I SHOULD USE THE UID IDEA THAT I'M USING FOR MESH FOR COLLIDER

	Model* mesh = nullptr; // THIS IS GOING
	long mesh_uid = -1;   // THIS IS REPLACING IT

	bool visible = true;

	std::vector<RenderState> current_state_buffer;
	std::vector<RenderState> previous_state_buffer;
	int state_index = 0;

	glm::vec4 debug_highlight_color = glm::vec4(0.3f, 0.4f, 0.7f, 0.3f);
	glm::vec2 mouse_last = glm::vec2(0.0f);

	void updateOrientationVectors();
	void selfOverrideColliderTransform(const bool = true);
	void colliderOverrideSelfTransform(const bool = true);
	void addOwnedChildActor(Actor*);
	void addOwnedChildDevice(Device*);

	virtual const bool canBeRendered() const; // Todo: remove the need to use this (its only use is in Actor::getRenderCommands())

private:
	int UID = -1;
	Theatre* parent_theatre = nullptr;
};
#endif
