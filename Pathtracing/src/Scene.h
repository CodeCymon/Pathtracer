#pragma once

#include "glm/glm.hpp"

#include <vector>

struct Material
{
	glm::vec3 Albedo{ 0.8f };
	float Specular = 0.05f;
	float Roughness = 1.0f;
	glm::vec3 SpecularColor{ 0.9f };
	float IOR = 1.4f;
	float Transmission = 0.0f;
	glm::vec3 TransmissionCoeff{ 0.0f };

	glm::vec3 EmissionColor{ 0.0f };
	float EmissionPower = 0.0f;

	glm::vec3 GetEmission() const { return EmissionColor * EmissionPower; }
};

struct Sphere
{
	glm::vec3 Position{0.0f};
	float Radius = 0.5f;

	int MaterialIndex = 0;
};

struct Tri
{
	glm::vec3 Position{ 0.0f };
	glm::vec3 v0{0.0f};
	glm::vec3 v1{1.0f};
	glm::vec3 v2{-1.0f};

	glm::vec3 normal{0.0f,1.0f,0.0f};

	int MaterialIndex = 0;

	void RecalculateNormal()  {

		glm::vec3 tmp = glm::normalize(glm::cross((v1 - v0), (v2 - v0)));
		if (tmp.x == 0.0) tmp.x = 0.0;
		if (tmp.y == 0.0) tmp.y = 0.0;
		if (tmp.z == 0.0) tmp.z = 0.0;
		normal = tmp;
	}

	void RecalculateFlippedNormal() {

		glm::vec3 tmp = glm::normalize(glm::cross((v1 - v0), (v0 - v2)));
		if (tmp.x == 0.0) tmp.x = 0.0;
		if (tmp.y == 0.0) tmp.y = 0.0;
		if (tmp.z == 0.0) tmp.z = 0.0;
		normal = tmp;
	}
};

struct Object
{
	uint32_t numFaces = 5;
	int faceIndex[6] = { 4, 4, 4, 4, 4, 4 };
	int vertexIndex[24] = { 0, 1, 3, 2, 0, 5, 6, 1, 1, 6, 7, 3, 7, 4, 2, 3, 5, 4, 7, 6, 2, 4, 5, 0 };
	glm::vec3 verts[8] = { { 1.0f, -1.0f, 1.0f }, { 1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, -1.0f }, { -1.0f, 1.0f, 1.0f }, { -1.0f, -1.0f, 1.0f }, { -1.0f, -1.0f, -1.0f }, { -1.0f, 1.0f, -1.0f } };
	int trisIndex[];
};


struct Scene
{
	std::vector<Sphere> Spheres;
	std::vector<Tri> Tris;
	std::vector<Object> Objects;
	std::vector<Material> Materials;
};