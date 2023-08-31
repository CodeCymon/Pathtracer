#include "Renderer.h"

#include "Walnut/Random.h"

#include <execution>

namespace Utils {
	static uint32_t ConvertToRGBA(const glm::vec4& light) 
	{
		uint8_t r = (light.r * 255.0f);
		uint8_t g = (light.g * 255.0f);
		uint8_t b = (light.b * 255.0f);
		uint8_t a = (light.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}

	static glm::vec3 mix(glm::vec3 va, glm::vec3 vb, float alpha)
	{
		return glm::vec3(
			(va.x * (1.0f - alpha) + (vb.x * alpha)),
			(va.y * (1.0f - alpha) + (vb.y * alpha)),
			(va.z * (1.0f - alpha) + (vb.z * alpha)));
	}

	static float mix(float a, float b, float alpha)
	{
		return (a * (1.0f - alpha) + (b * alpha));
	}

	static glm::vec3 LessThan(glm::vec3 f, float value)
	{
		return glm::vec3(
			(f.x < value) ? 1.0f : 0.0f,
			(f.y < value) ? 1.0f : 0.0f, 
			(f.z < value) ? 1.0f : 0.0f);
	}

	static glm::vec4 LinearToSRGB(glm::vec4 rgba) 
	{
		rgba = glm::clamp(rgba, glm::vec4(0.0f), glm::vec4(1.0f));

		return glm::vec4(
			glm::mix(pow(rgba.x, (1.0f / 2.4f)) * 1.055f - 0.055f, rgba.x * 12.92f, LessThan(rgba, 0.0031308f).x), 
			glm::mix(pow(rgba.y, (1.0f / 2.4f)) * 1.055f - 0.055f, rgba.y * 12.92f, LessThan(rgba, 0.0031308f).y), 
			glm::mix(pow(rgba.z, (1.0f / 2.4f)) * 1.055f - 0.055f, rgba.z * 12.92f, LessThan(rgba, 0.0031308f).z), 
			1.0f);
	}

	static glm::vec4 ACESTransform(glm::vec4 x)
	{
		float a = 2.51f;
		float b = 0.03f;
		float c = 2.43f;
		float d = 0.59f;
		float e = 0.14f;
		return glm::clamp((x * (a * x + b)) / (x * (c * x + d) + e), glm::vec4(0.0f), glm::vec4(1.0f));
	}

	static uint32_t PCG_Hash(uint32_t input)
	{
		uint32_t state = input * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	static float RandomFloat(uint32_t& seed)
	{
		seed = PCG_Hash(seed);
		return (float)seed / (float)std::numeric_limits<uint32_t>::max();
	}

	static glm::vec3 InUnitSphere(uint32_t& seed)
	{
		return glm::normalize(glm::vec3(
			RandomFloat(seed) * 2.0f - 1.0f, 
			RandomFloat(seed) * 2.0f - 1.0f, 
			RandomFloat(seed) * 2.0f - 1.0f));
	}

	static glm::vec2 InUnitDisk(uint32_t& seed)
	{
		return glm::normalize(glm::vec2(
			RandomFloat(seed) * 2.0f - 1.0f,
			RandomFloat(seed) * 2.0f - 1.0f));
	}

	static float FresnelReflectAmount(float n1, float n2, glm::vec3 direction, glm::vec3 normal, float f0, float f90)
	{
		float r0 = (n1 - n2) / (n1 + n2); 
		r0 *= r0;
		float cosX = -glm::dot(normal, direction);
		if (n1 > n2) 
		{
			float n = n1 / n2; 
			float sinT2 = n * n * (1.0f - cosX * cosX);

			if (sinT2 > 1.0f)
				return f90;
			cosX = sqrt(1.0f - sinT2);
		}

		float x = 1.0f - cosX;
		float ret = r0 + (1.0f - r0) * x * x * x * x * x;

		return Utils::mix(f0, f90, ret);
	}

	static float Remap(float value, float fromMin, float fromMax, float toMin, float toMax)
	{
		return toMin + (toMax - toMin) * ((value - fromMin) / (fromMax - fromMin));
	}

}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		// no resize necessary
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

	delete[] m_AccumulationData;
	m_AccumulationData = new glm::vec4[width * height];

	m_ImageHorizontalIter.resize(width);
	m_ImageVerticalIter.resize(height);

	for (uint32_t i = 0; i < width; i++)
		m_ImageHorizontalIter[i] = i;

	for (uint32_t i = 0; i < height; i++)
		m_ImageVerticalIter[i] = i;


}


void Renderer::Render(const Scene& scene, const Camera& camera)
{	
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;

	if (m_FrameIndex == 1)
		memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));

#define MT 1
#if MT

	std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(), 
		[this](uint32_t y) 
		{
			std::for_each(std::execution::par, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
			[this, y](uint32_t x)
				{
					glm::vec4 light(0.0f);

					light += PerPixel(x, y);

					// accumulate sample data over time
					m_AccumulationData[x + y * m_FinalImage->GetWidth()] += light;

					// continuesly average the accumulated information for the final image 
					glm::vec4 accumulatedCol = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
					accumulatedCol /= (float)m_FrameIndex;

					accumulatedCol *= m_Settings.Exposure;

					accumulatedCol = Utils::ACESTransform(accumulatedCol);

					// clamp the light to 0-1 and convert to sRGB
					accumulatedCol = Utils::LinearToSRGB(accumulatedCol);
					// write image data to buffer
					m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedCol);
				});
		});

#else

	// render every pixel
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec4 light = PerPixel(x, y);
			m_AccumulationData[x + y * m_FinalImage->GetWidth()] += light;

			// average the light for final image
			glm::vec4 accumulatedCol = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
			accumulatedCol /= (float)m_FrameIndex;

			// clamp the light to 0-1 and write it to the image data
			accumulatedCol = glm::clamp(accumulatedCol, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedCol);
		}
	}
#endif

	m_FinalImage->SetData(m_ImageData);

	if (m_Settings.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;

}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];
	

	glm::vec3 light(0.0f);
	glm::vec3 contribution(1.0f);

	uint32_t seed = x + y * m_FinalImage->GetWidth();
	seed *= m_FrameIndex;

	// offset each pixel for anti-aliasing
	if(m_Settings.Antialiasing) // add small random directional offset to each ray
		ray.Direction += glm::vec3(Utils::InUnitDisk(seed) * glm::vec2(-1.0f / (float)m_FinalImage->GetWidth(), 1.0f / (float)m_FinalImage->GetHeight()), 0.0f); // scale unitdisk by screen space

	int bounces = m_Settings.Bounces;
	for (int i = 0; i < bounces; i++)
	{
		seed += i;

		// actually trace the ray
		Renderer::HitPayload payload;
		payload.fromInside = false; 
		payload = TraceRay(ray);

		// return sky color or black when scene is missed
		if (payload.hitDistance < 0.0f)
		{
			if(m_Settings.UseSkylight)
				light += m_Settings.SkyColor * contribution;
			break;
		}


		// get some scene info from the traced ray about the hit object and the material there
		//const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];

		const Tri& tri = m_ActiveScene->Tris[payload.ObjectIndex];
		const Material& material = m_ActiveScene->Materials[tri.MaterialIndex];

		glm::vec3 specularCol = Utils::mix(glm::vec3(0.9f), material.Albedo, material.Specular);

		if (payload.fromInside)
			contribution *= exp(-material.TransmissionCoeff * payload.hitDistance);

		// calculate the specular amount via the fresnel
		float specularChance = Utils::Remap(material.Specular, 0.0f,1.0f,0.05f,1.0f);
		float materialSpec = specularChance;
		float refreactionChance = material.Transmission;

		float rayProbability = 1.0f;
		if (specularChance > 0.0f)
		{
			specularChance = Utils::FresnelReflectAmount(
				payload.fromInside ? material.IOR : 1.0f,
				!payload.fromInside ? material.IOR : 1.0f,
				ray.Direction, payload.Normal, materialSpec, 1.0f);

			float chanceMutliplier = (1.0f - specularChance) / (1.0f - materialSpec);
			refreactionChance *= chanceMutliplier;
		}

		// decide for either diffuse, specular or refraction ray
		float doSpecular = 0.0f;
		float doRefraction = 0.0f;
		float raySelectRoll = Utils::RandomFloat(seed);
		if (specularChance > 0.0f && raySelectRoll < specularChance)
		{
			doSpecular = 1.0f;
			rayProbability = specularChance;
		}
		else if (refreactionChance > 0.0f && raySelectRoll < specularChance + refreactionChance)
		{
			doRefraction = 1.0f;
			rayProbability = refreactionChance;
		}
		else
		{
			rayProbability = 1.0f - (specularChance + refreactionChance);
		}

		// avoid divide by 0 error
		rayProbability = glm::max(rayProbability, 0.001f);
		if (doRefraction == 1.0f)
		{
			// nudge the ray position inward a bit to avoid intersecting self intersecting with the sphere
			ray.Origin = payload.WorldPosition - payload.Normal * c_RayBounceNormalNudge;
		}
		else
		{
			// nudge the ray position outward a bit to avoid intersecting self intersecting with the sphere
			ray.Origin = payload.WorldPosition + payload.Normal * c_RayBounceNormalNudge;
		}

		// calculate the new ray direction
		glm::vec3 diffuseRayDir = glm::normalize(payload.Normal + Utils::InUnitSphere(seed));

		glm::vec3 specularRayDir = glm::reflect(ray.Direction, payload.Normal);
		specularRayDir = glm::normalize(Utils::mix(specularRayDir, diffuseRayDir, material.Roughness * material.Roughness));

		glm::vec3 refractionRayDir = glm::refract(ray.Direction, payload.Normal, payload.fromInside ? material.IOR : 1.0f / material.IOR);
		refractionRayDir = glm::normalize(Utils::mix(refractionRayDir, glm::normalize(-payload.Normal + Utils::InUnitSphere(seed)), material.Roughness * material.Roughness));

		ray.Direction = Utils::mix(diffuseRayDir, specularRayDir, doSpecular); 
		ray.Direction = Utils::mix(ray.Direction, refractionRayDir, doRefraction); 

		// add emissive lighting
		light += material.GetEmission() * contribution;

		// update the color multiplier
		if(doRefraction == 0.0f)
			contribution *= Utils::mix(material.Albedo, specularCol, doSpecular);

		contribution /= rayProbability;

		// russian roulette
		{
			float p = glm::max(contribution.r, glm::max(contribution.g, contribution.b));
			if (Utils::RandomFloat(seed) > p)
				break;

			contribution *= 1.0f / p;
		}
	}

	return glm::vec4(light, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	int closestObjectIndex = -1;
	float hitDistance = std::numeric_limits<float>::max();
	
	for (size_t i = 0; i < m_ActiveScene->Tris.size(); i++)
	{
		const Tri& tri = m_ActiveScene->Tris[i];

		// ray origin adjusted with global offset of the triangle
		glm::vec3 origin = ray.Origin - tri.Position;

		//Edge calculation
		glm::vec3 v0v1 = tri.v1 - tri.v0; 
		glm::vec3 v0v2 = tri.v2 - tri.v0; 

		// pvec is negative if ray and normal are the same direction
		glm::vec3 pvec = glm::cross(ray.Direction, v0v2);
		float determinant = glm::dot(v0v1, pvec);  
 
		// Backface Culling based on pvec relation to edge1(v0v1) (determinant)
		//if (determinant < c_kEpsilon) 
		//	continue;

		// inverted determinant
		float invDet = 1 / determinant; 
		// check for intersection A
		glm::vec3 tvec = origin - tri.v0;
		float u = glm::dot(tvec, pvec) * invDet;
		if (u < 0 || u > 1)
			continue;

		// check for intersection B
		glm::vec3 qvec = glm::cross(tvec, v0v1);
		float v = glm::dot(ray.Direction, qvec) * invDet;
		if (v < 0 || u + v > 1)
			continue;

		// calculate the hitdistance t
		float t = glm::dot(v0v2, qvec) * invDet; 

		if (t < 0.0f)
			continue;

		if (t < hitDistance)
		{
			hitDistance = t;
			closestObjectIndex = (int)i; // set the closest Object
		}

	}

	if (closestObjectIndex < 0)
		return Miss(ray);

	return ClosestHit(ray, hitDistance, closestObjectIndex, false);
}


Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex, bool fromInside)
{
	Renderer::HitPayload payload;
	payload.hitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;

	const Tri& closestTri = m_ActiveScene->Tris[objectIndex];
	
	glm::vec3 origin = ray.Origin - closestTri.Position;
	payload.WorldPosition = origin + hitDistance * ray.Direction;
	payload.Normal = closestTri.normal; 

	payload.WorldPosition += closestTri.Position;
	payload.fromInside = fromInside;

	return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayload payload;
	payload.hitDistance = -1.0f;
	payload.fromInside = false;
	return payload;
}