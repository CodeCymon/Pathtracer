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
	glm::vec3 v0{ 0.0f , 0.0f,  1.0f};
	glm::vec3 v1{ 0.0f , 0.0f, -1.0f};
	glm::vec3 v2{ 0.0f , 2.0f,  1.0f};

	int MaterialIndex = 0;

	glm::vec3 GetNormal() const {
		return glm::normalize(glm::cross((v1 - v0), (v2 - v0)));
	}
};

struct Scene
{
	std::vector<Sphere> Spheres;
	std::vector<Tri> Tris;
	std::vector<Material> Materials;
};