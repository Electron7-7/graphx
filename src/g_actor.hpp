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

#ifdef COMPILER_FORWARD_DECLARATIONS // This is to keep forward declarations from causing issues when including header files
struct ActorPointerWrapper;
struct DevicePointerWrapper;
struct Theatre;
#endif

class Actor
{
public: // Externally accessible members
	Collider* collider = nullptr; // I SHOULD USE THE UID IDEA THAT I'M USING FOR MESH FOR COLLIDER

	Model* mesh = nullptr; // THIS IS GOING
	long mesh_uid = -1;   // THIS IS REPLACING IT

	bool visible = true;

	bool debug_highlight_enabled = false;

	std::string name = "Untitled Actor";

	// The constructor that should be used 99% of the time
	Actor(const std::string& Name);

	// The constructor that the Interpreter uses when creating Actors
	Actor(Theatre* ParentTheatre, const int UID, const graphx::gSettings& Settings = graphx::gSettings());

	virtual ~Actor();

	void updateStates(std::mutex&);
	int getUID() const;
	void setUID(const int NewUID);
	graphx::gSettings getSettings() const;
	void setSettings(const graphx::gSettings& NewSettings);

	void addChildActor(Actor*);
	void addChildDevice(Device*);

	Actor* getChildActor(const int);
	Device* getChildDevice(const int);

	void setGlobalPosition(const glm::vec3& Position);
	void setGlobalQuaternion(const glm::quat& Quaternion);
	void setGlobalEulerAngles(const glm::vec3& EulerAngles, const bool UseDegrees = false);
	void setGlobalPitch(const float Pitch, const bool UseDegrees = false);
	void setGlobalYaw(const float Yaw, const bool UseDegrees = false);
	void setGlobalRoll(const float Roll, const bool UseDegrees = false);
	void setGlobalScale(const glm::vec3& Scale);

	void setLocalPosition(const glm::vec3& Position);
	void setLocalQuaternion(const glm::quat& Quaternion);
	void setLocalEulerAngles(const glm::vec3& EulerAngles, const bool UseDegrees = false);
	void setLocalPitch(const float Pitch, const bool UseDegrees = false);
	void setLocalYaw(const float Yaw, const bool UseDegrees = false);
	void setLocalRoll(const float Roll, const bool UseDegrees = false);
	void setLocalScale(const glm::vec3& Scale);

	glm::vec3 getOrientationUp(const bool Global = true) const;
	glm::vec3 getOrientationFront(const bool Global = true) const;
	glm::vec3 getOrientationRight(const bool Global = true) const;

	glm::vec3 getGlobalPosition() const;
	glm::quat getGlobalQuaternion() const;
	glm::vec3 getGlobalEulerAngles(const bool AsDegrees = false) const;
	glm::vec3 getGlobalScale() const;

	glm::vec3 getLocalPosition() const;
	glm::quat getLocalQuaternion() const;
	glm::vec3 getLocalEulerAngles(const bool AsDegrees = false) const;
	glm::vec3 getLocalScale() const;


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
	Theatre* parent_theatre = nullptr;
	graphx::gSettings settings = graphx::gSettings();

	std::vector<ActorPointerWrapper> child_actors;
	std::vector<DevicePointerWrapper> child_devices;

	glm::vec3 orientation_up = graphx::orientation::up;
	glm::vec3 orientation_front = graphx::orientation::front;
	glm::vec3 orientation_right = graphx::orientation::right;

	glm::vec3 position_global = glm::vec3(0.0f);
	glm::vec3 position_local = glm::vec3(0.0f);
	glm::quat quaternion_global = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::quat quaternion_local = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scale_global  = glm::vec3(1.0f);
	glm::vec3 scale_local = glm::vec3(1.0f);

	// Consider changing vector to array
	std::vector<RenderState> current_state_buffer  = { RenderState(), RenderState() };
	std::vector<RenderState> previous_state_buffer = { RenderState(), RenderState() };
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
};

namespace graphx
{
	namespace safety
	{
		extern Actor actor;
	}
}
#endif
