#define GLM_ENABLE_EXPERIMENTAL
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <vector>
#include <string>

bool loadOBJ(std::string path, std::vector<glm::vec3> &out_vertices, std::vector<glm::vec3> &out_indices);

int main()
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> indices;

	loadOBJ("include/ERROR.obj", vertices, indices);

	std::cout << "Vertices:\n";
	for(glm::vec3 vertex : vertices)
	{
		std::cout << glm::to_string(vertex) << std::endl;
	}

	std::cout << "Indices:\n";
	for(glm::vec3 index : indices)
	{
		std::cout << glm::to_string(index) << std::endl;
	}

	return 0;
}

bool loadOBJ(std::string path, std::vector<glm::vec3> &out_vertices, std::vector<glm::vec3> &out_indices)
{
	FILE *mesh_file = std::fopen(path.c_str(), "r");
	if(mesh_file == NULL)
		return false;

	while(true)
	{
		char line_header[128];
		int result = std::fscanf(mesh_file, "%s", line_header);
		if(result == EOF)
			break;

		if(strcmp(line_header, "v") == 0)
		{
			glm::vec3 vertex;
			std::fscanf(mesh_file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			out_vertices.push_back(vertex);
		}

		else if(strcmp(line_header, "f") == 0)
		{
			// std::string vertex1, vertex2, vertex3;
			unsigned int vertex_index[3];
			unsigned int trashcan[3];
			int matches = std::fscanf(mesh_file, "%d/%d %d/%d %d/%d\n", &vertex_index[0], &trashcan[0], &vertex_index[1], &trashcan[1], &vertex_index[2], &trashcan[2]);
			
			if(matches != 6)
				return false;

			glm::vec3 temporary_index = glm::vec3(vertex_index[0], vertex_index[1], vertex_index[2]);
			
			out_indices.push_back(temporary_index);
		}
	}

	return true;
}