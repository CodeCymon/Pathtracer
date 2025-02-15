#pragma once

#include "Walnut/Image.h"

#include "Camera.h"
#include "ray.h"
#include "Scene.h"

#include <memory>
#include <glm/glm.hpp>

class Renderer
{
public:
	struct Settings 
	{
		bool Accumulate = true;
		bool UseSkylight = true;
		bool Antialiasing = true;
		float Exposure = 1.0f;

		glm::vec3 SkyColor{0.3f};

		int Bounces = 5;
	};

public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Scene& scene, const Camera& camera);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

	void ResetFrameIndex() { m_FrameIndex = 1; }
	Settings& GetSettings() { return m_Settings; }

private:

	struct HitPayload
	{
		float hitDistance{};
		glm::vec3 WorldPosition{};
		glm::vec3 Normal{};
		bool fromInside = false;

		int ObjectIndex = 0;
	};

	glm::vec4 PerPixel(uint32_t x, uint32_t y);
	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex, bool fromInside);
	HitPayload Miss(const Ray& ray);

private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	Settings m_Settings;

	std::vector<uint32_t> m_ImageHorizontalIter, m_ImageVerticalIter;

	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;


	uint32_t* m_ImageData = nullptr;
	glm::vec4* m_AccumulationData = nullptr;

	uint32_t m_FrameIndex = 1;

	const float c_RayBounceNormalNudge = 0.001f;
	const float c_Skydistance = 10000.0f;
	const float c_kEpsilon = 0.0001f;
};