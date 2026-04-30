#include <iostream>
#include <map>
#include <string>
#include <memory>
#include <set>
#include "mesh.h"

class ResourceManager
{
public:

	std::map<std::string, std::shared_ptr<Mesh>> meshCache{};

	ResourceManager()
	{

	};


	std::shared_ptr<Mesh> getMesh(const std::string& name)
	{
		if (meshCache.find(name) != meshCache.end()) {
			std::cout << "Found mesh" << '\n';
			return meshCache[name];
		};

		std::cout << name <<  " mesh not in cache" << '\n';
		return NULL;
	};

	std::shared_ptr<Mesh> createMesh(const std::string& name ,const std::vector<float> vertexData, const std::vector<int> indexData , const int vertexCount)
	{
		if (getMesh(name) != NULL) {
			std::cout << name << " mesh in cache already, returning" << '\n';
			return meshCache[name];
		}

		std::cout << "Creating " << name << " mesh..." << '\n';
		meshCache[name] = std::make_shared<Mesh>(vertexData, indexData, vertexCount);
		return meshCache[name];

	};
};