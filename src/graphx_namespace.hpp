#ifndef GRAPHX_NAMESPACE
#define GRAPHX_NAMESPACE
#include <glm/glm.hpp>
#include <map>
#include <any>
#include <array>
#include <string>
#include <vector>
#include <unordered_map>

#define GRAPHXTHEATRE_EXTENSION std::string(".gt")

namespace graphx
{
	namespace error
	{
		namespace rendercmd
		{
			static constexpr int MISSING_VBO_NAME           = 1 << 0; // 1
			static constexpr int MISSING_MESH_DATA_SIZE     = 1 << 1; // 2
			static constexpr int MISSING_MESH_DATA_OFFSET   = 1 << 2; // 4
			static constexpr int MISSING_BOTH_RENDER_STATES = 1 << 3; // 8

		};
	};

	namespace types
	{
		typedef int gPrimitive;

		namespace primitive
		{
			static constexpr gPrimitive FOO      = -1;
			static constexpr gPrimitive LINE     = 0;
			static constexpr gPrimitive TRIANGLE = 1;
			static constexpr gPrimitive TEXT     = -1; // Text not supported yet!
		};
	};

	namespace classes
	{
		static constexpr int THEATRE			= 0;
		static constexpr int ACTOR				= 1;
		static constexpr int PHYSICSACTOR		= 2;
		static constexpr int RIGIDBODYACTOR		= 3;
		static constexpr int STATICBODYACTOR	= 11;
		static constexpr int CAMERA				= 4;
		static constexpr int GRAPHXPLAYER		= 5;
		static constexpr int LIGHT				= 6;
		static constexpr int LIGHTDIRECTIONAL	= 7;
		static constexpr int LIGHTSPOT			= 8;
		static constexpr int LIGHTFLASHLIGHT	= 9;
		static constexpr int LIGHTTESTERMOVER	= 10;

		static constexpr int DEVICE				= 500;
		static constexpr int ENVIRONMENT		= 501;
		static constexpr int MATERIAL			= 502;
		static constexpr int MESH				= 503;
		static constexpr int SPRITE				= 504;
		static constexpr int COLLIDER			= 505;

		static constexpr int ACTORS[2] = {0, 499};
		static constexpr int DEVICES[2] = {500, 999};
		static constexpr std::array<int, 5> LIGHTS =
		{
			LIGHT,
			LIGHTSPOT,
			LIGHTFLASHLIGHT,
			LIGHTDIRECTIONAL,
			LIGHTTESTERMOVER,
		};
	}

	typedef std::map<int, std::pair<std::string, std::string>> gObjectStore;
	typedef std::multimap<int, std::pair<std::string, std::string>> gSourceRefStore;
	typedef std::multimap<int, std::pair<std::string, int>> gTheatreRefStore;
	typedef std::multimap<int, std::pair<std::string, std::string>> gRawDataStore;
	typedef std::multimap<int, std::pair<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>>> gSandwichStore;
	typedef std::tuple<std::string, gObjectStore, gSourceRefStore, gTheatreRefStore, gRawDataStore, gSandwichStore> gTheatreStorage;
	typedef std::vector<std::string> gRawData;

	struct gMeshData
	{
	public:
		int vertex_data_uid = -1;
		int vao_index = -1;

		std::vector<float> vertex_positions;
		std::vector<float> vertex_normals;
		std::vector<float> vertex_uvs;
		std::vector<float> vertex_colors;

		gMeshData(int init_vao_index, std::vector<float> init_positions = {0.0f, 0.0f, 0.0f}, std::vector<float> init_normals = {0.0f, 0.0f, 0.0f}, std::vector<float> init_uvs = {0.0f, 0.0f}, std::vector<float> init_colors = {1.0f, 1.0f, 1.0f})
		: vao_index(init_vao_index), vertex_positions(init_positions), vertex_normals(init_normals), vertex_uvs(init_uvs), vertex_colors(init_colors)
		{}

		gMeshData(std::vector<float> init_positions = {0.0f, 0.0f, 0.0f}, std::vector<float> init_normals = {0.0f, 0.0f, 0.0f}, std::vector<float> init_uvs = {0.0f, 0.0f}, std::vector<float> init_colors = {1.0f, 1.0f, 1.0f})
		: vertex_positions(init_positions), vertex_normals(init_normals), vertex_uvs(init_uvs), vertex_colors(init_colors)
		{}

		inline std::vector<float> getVertexData()
		{
			std::vector<float> vertex_data = vertex_positions;
			vertex_data.insert(vertex_data.end(), vertex_normals.begin(), vertex_normals.end());
			vertex_data.insert(vertex_data.end(), vertex_uvs.begin(), vertex_uvs.end());
			vertex_data.insert(vertex_data.end(), vertex_colors.begin(), vertex_colors.end());
			return vertex_data;
		}

		inline bool operator==(const gMeshData &compared_with) const
		{
			return
			(
				// vao_index        == compared_with.vao_index        &&
				vertex_positions == compared_with.vertex_positions &&
				vertex_normals   == compared_with.vertex_normals   &&
				vertex_uvs       == compared_with.vertex_uvs       &&
				vertex_colors    == compared_with.vertex_colors
			);
		}

	private:
		// For the time being, I'm removing indices from the rendering process
		// std::vector<unsigned int> vertex_indices;
	};

	struct gMeshBufferData
	{
	public:
		int vao_index         = 0; // There's only one VAO right now (VAO_DEFAULT == 0)
		int vbo_name          = -1;
		long mesh_data_size   = -1;
		long mesh_data_offset = -1;
	};

	// gKey, gValue, gStringSetting, and gStringSettings are for the interpreter/parser only
	// and shouldn't be used by anything else (except for Theatre::graphx_theatre_settings)
	typedef std::string gKey;
	typedef std::pair<int, std::string> gValue;
	typedef std::pair<gKey, gValue> gStringSetting;
	typedef std::vector<std::vector<gStringSetting>> gStringSettings;

	// The int in gSetting identifies the type (found in t_common.hpp); the four types are:
	// 1: RAW_DATA (a vector of strings, using the typedef "gRawData")
	// 2: CPP_REFERENCE (see "cpp_definitions" in "t_interpreter.cpp")
	// 3: THEATRE_REFERENCE (a pointer to a pre-existing Actor/Device in the current Theatre)
	// 4: EXTERNAL_REFERENCE (an external file's pathname, passed via string)
	// 5: SANDWICH_BUN (a unique copy of a pre-existing Actor/Device in the current Theatre)
	typedef std::pair<int, std::any> gSetting;
	typedef std::unordered_map<std::string, gSetting> gSettings;

	static std::map<int, std::string> classnames =
	{
		{classes::THEATRE, "Theatre"},
		{classes::ACTOR, "Actor"},
		{classes::PHYSICSACTOR, "PhysicsActor"},
		{classes::RIGIDBODYACTOR, "RigidBodyActor"},
		{classes::STATICBODYACTOR, "StaticBodyActor"},
		{classes::CAMERA, "Camera"},
		{classes::GRAPHXPLAYER, "GraphXPlayer"},
		{classes::LIGHT, "Light"},
		{classes::LIGHTDIRECTIONAL, "LightDirectional"},
		{classes::LIGHTSPOT, "LightSpot"},
		{classes::LIGHTFLASHLIGHT, "LightFlashlight"},
		{classes::LIGHTTESTERMOVER, "LightTesterMover"},
		{classes::DEVICE, "Device"},
		{classes::ENVIRONMENT, "Environment"},
		{classes::MATERIAL, "Material"},
		{classes::MESH, "Mesh"},
		{classes::SPRITE, "Sprite"},
		{classes::COLLIDER, "Collider"}
	};
}
#endif