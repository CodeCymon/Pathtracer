#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"

#include "glm/gtc/type_ptr.hpp"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.0f) 
	{
		Material& white = m_Scene.Materials.emplace_back();
		white.Albedo = { 0.7f, 0.7f ,0.7f };
		white.Roughness = 0.0f;
		white.Specular = 0.0f;
		white.SpecularColor = { 0.0f, 0.0f, 0.0f };

		Material& red = m_Scene.Materials.emplace_back();
		red.Albedo = { 0.7f, 0.3f ,0.2f };
		red.Roughness = 0.0f;
		red.Specular = 0.0f;
		red.SpecularColor = { 0.0f, 0.0f, 0.0f };
		 
		Material& green = m_Scene.Materials.emplace_back();
		green.Albedo = { 0.3f, 0.7f ,0.2f };
		green.Roughness = 0.0f;
		green.Specular = 0.0f;
		green.SpecularColor = { 0.0f, 0.0f, 0.0f };

		Material& glossy = m_Scene.Materials.emplace_back();
		glossy.Albedo = { 0.9f, 0.9f ,0.5f };
		glossy.Specular = 0.03f;
		glossy.Roughness = 0.2f;
		glossy.IOR = 1.4f;

		Material& glossyPink = m_Scene.Materials.emplace_back();
		glossyPink.Albedo = { 0.9f, 0.5f ,0.9f };
		glossyPink.Specular = 0.03f;
		glossyPink.Roughness = 0.2f;
		glossyPink.IOR = 1.5f;

		Material& glass= m_Scene.Materials.emplace_back();
		glass.Albedo = { 1.0f, 1.0f ,1.0f };
		glass.Specular = 0.03f;
		glass.Roughness = 0.1f;
		glass.IOR = 1.5f;
		glass.Transmission = 1.0f;
		glass.TransmissionCoeff = { 0.8f, 0.8f ,0.8f };
		

		Material& whiteLight = m_Scene.Materials.emplace_back();
		whiteLight.Albedo = { 1.0f, 1.0f , 1.0f }; 
		whiteLight.Roughness = 1.0f; 
		whiteLight.EmissionColor = whiteLight.Albedo;
		whiteLight.EmissionPower = 10.0f;

		Material& SkySphere= m_Scene.Materials.emplace_back();
		SkySphere.Albedo = { 0.7f, 0.7f , 0.98f };
		SkySphere.Roughness = 1.0f;
		SkySphere.EmissionColor = SkySphere.Albedo;
		SkySphere.EmissionPower = 0.75f;

		{
			Material& roughnessMat = m_Scene.Materials.emplace_back();
			roughnessMat.Albedo = { 0.9f, 0.9f,0.9f };
			roughnessMat.Specular = 1.0f;
			roughnessMat.Roughness = 0.0f;
			roughnessMat.SpecularColor = { 0.9f, 0.9f,0.9f };
		}

		{
			Material& roughnessMat = m_Scene.Materials.emplace_back();
			roughnessMat.Albedo = { 0.9f, 0.9f,0.9f };
			roughnessMat.Specular = 1.0f;
			roughnessMat.Roughness = 0.5f;
			roughnessMat.SpecularColor = { 0.9f, 0.9f,0.9f };
		}

		{
			Material& roughnessMat = m_Scene.Materials.emplace_back();
			roughnessMat.Albedo = { 0.9f, 0.9f,0.9f };
			roughnessMat.Specular = 1.0f;
			roughnessMat.Roughness = 1.0f;
			roughnessMat.SpecularColor = { 0.9f, 0.9f,0.9f };
		}
		{
			Material& roughnessMat = m_Scene.Materials.emplace_back();
			roughnessMat.Albedo = { 0.9f, 0.9f,0.9f };
			roughnessMat.Specular = 1.0f;
			roughnessMat.Roughness = 0.75f;
			roughnessMat.SpecularColor = { 0.9f, 0.9f,0.9f };
		}
		{
			Material& roughnessMat = m_Scene.Materials.emplace_back();
			roughnessMat.Albedo = { 0.9f, 0.9f ,0.9f };
			roughnessMat.Specular = 1.0f;
			roughnessMat.Roughness = 0.25f;
			roughnessMat.SpecularColor = { 0.9f, 0.9f,0.9f };
		}

		{
			Sphere floor;
			floor.Position = { 0.0f, -1002.0f, 0.0f };
			floor.Radius = 1000.0f;
			floor.MaterialIndex = 0;
			m_Scene.Spheres.push_back(floor);
		}

		{
			Sphere lightBall;
			lightBall.Position = { 0.0f, 2.0f, 20.0f };
			lightBall.Radius = 5.0f;
			lightBall.MaterialIndex = 6;
			m_Scene.Spheres.push_back(lightBall);
		}

		{
			Sphere lime;
			lime.Position = { -1.4f, -1.5f, 0.0f };
			lime.Radius = 0.5f;
			lime.MaterialIndex = 3;
			m_Scene.Spheres.push_back(lime);
		}

		{
			Sphere pink;
			pink.Position = { 0.0f, -1.5f, 0.0f };
			pink.Radius = 0.5f;
			pink.MaterialIndex = 4;
			m_Scene.Spheres.push_back(pink);
		}

		{
			Sphere blue;
			blue.Position = { 1.4f, -1.5f, 0.0f };
			blue.Radius = 0.5f;
			blue.MaterialIndex = 5;
			m_Scene.Spheres.push_back(blue);
		}
#define scene 0

#if scene
		{
			Sphere lightBall;
			lightBall.Position = { 0.0f, 0.0f, 1010.0f };
			lightBall.Radius = 1000.0f;
			lightBall.MaterialIndex = 7;
			m_Scene.Spheres.push_back(lightBall);
		}

		{
			Sphere roughnessBall;
			roughnessBall.Position = { 1.6f, 0.0f, -1.0f };
			roughnessBall.Radius = 0.3f;
			roughnessBall.MaterialIndex = 10;
			m_Scene.Spheres.push_back(roughnessBall);
		}

		{
			Sphere roughnessBall;
			roughnessBall.Position = { 0.0f, 0.0f, -1.0f };
			roughnessBall.Radius = 0.3f;
			roughnessBall.MaterialIndex = 9;
			m_Scene.Spheres.push_back(roughnessBall);
		}

		{
			Sphere roughnessBall;
			roughnessBall.Position = { -1.6f, 0.0f, -1.0f };
			roughnessBall.Radius = 0.3f;
			roughnessBall.MaterialIndex = 8;
			m_Scene.Spheres.push_back(roughnessBall);
		}
		{
			Sphere roughnessBall;
			roughnessBall.Position = { -0.8f, 0.0f, -1.0f };
			roughnessBall.Radius = 0.3f;
			roughnessBall.MaterialIndex = 12;
			m_Scene.Spheres.push_back(roughnessBall);
		}

		{
			Sphere roughnessBall;
			roughnessBall.Position = { 0.8f, 0.0f, -1.0f };
			roughnessBall.Radius = 0.3f;
			roughnessBall.MaterialIndex = 11;
			m_Scene.Spheres.push_back(roughnessBall);
		}

		{
			Sphere wallLeft;
			wallLeft.Position = { -1002.0f, 0.0f, 0.0f };
			wallLeft.Radius = 1000.0f;
			wallLeft.MaterialIndex = 1;
			m_Scene.Spheres.push_back(wallLeft);
		}

		{
			Sphere wallRight;
			wallRight.Position = { 1002.0f, 0.0f, 0.0f };
			wallRight.Radius = 1000.0f;
			wallRight.MaterialIndex = 2;
			m_Scene.Spheres.push_back(wallRight);
		}

		{
			Sphere backWall;
			backWall.Position = { 0.0f, 0.0f, -1001.3f };
			backWall.Radius = 1000.0f;
			backWall.MaterialIndex = 0;
			m_Scene.Spheres.push_back(backWall);
		}

		{
			Sphere roof;
			roof.Position = { 0.0f, 1002.0f, 0.0f };
			roof.Radius = 1000.0f;
			roof.MaterialIndex = 0;
			m_Scene.Spheres.push_back(roof);
		}
#endif
	}

	virtual void OnUpdate(float ts) override
	{
		if (m_Camera.OnUpdate(ts)) {
			m_Renderer.ResetFrameIndex();
		}
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Render Time: %.3fms", m_LastRenderTime);
		if (ImGui::Button("Render"))
		{
			Render();
		}

		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);
		ImGui::Checkbox("UseSkylight", &m_Renderer.GetSettings().UseSkylight); 
		ImGui::DragFloat("Exposure", &m_Renderer.GetSettings().Exposure, 0.1f, 0.0f, 4.0f);
		ImGui::DragInt("Bounces", &m_Renderer.GetSettings().Bounces, 1.0f, 2.0f, 8.0f);
		ImGui::Checkbox("AA", &m_Renderer.GetSettings().Antialiasing); 
		ImGui::DragInt("Samples Per Pixel", &m_Renderer.GetSettings().SamplesPerPixel, 1.0f, 1.0f, 4.0f);


		if (ImGui::Button("Reset"))
		{
			m_Renderer.ResetFrameIndex();
		}
		ImGui::End();


		ImGui::Begin("Scene");
		for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
		{
			ImGui::PushID(i);

			Sphere& _sphere = m_Scene.Spheres[i];
			ImGui::DragFloat3("Position", glm::value_ptr(_sphere.Position), 0.1f);
			ImGui::DragFloat("Radius", &_sphere.Radius, 0.1f);
			ImGui::DragInt("Material", &_sphere.MaterialIndex, 1.0f, 0.0f, (int)m_Scene.Materials.size() - 1);

			ImGui::Separator();

			ImGui::PopID();
		}
		for (size_t i = 0; i < m_Scene.Materials.size(); i++)
		{
			ImGui::PushID(i);

			Material& material = m_Scene.Materials[i];
			ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
			ImGui::DragFloat("Metallic", &material.Metallic, 0.05f, 0.0f, 1.0f);
			ImGui::DragFloat("Specular", &material.Specular, 0.05f, 0.0f, 1.0f);
			ImGui::DragFloat("Roughness", &material.Roughness, 0.05f, 0.0f, 1.0f);
			ImGui::DragFloat("Transmission", &material.Transmission, 0.05f, 0.0f, 1.0f);
			ImGui::DragFloat("Transmission Roughness", &material.TransmissionRoughness, 0.05f, 0.0f, 1.0f);
			ImGui::ColorEdit3("Transmission Color", glm::value_ptr(material.TransmissionCoeff));
			ImGui::ColorEdit3("Emission Color", glm::value_ptr(material.EmissionColor));
			ImGui::DragFloat("Emission Power", &material.EmissionPower, 0.05f, 0.0f, FLT_MAX);

			ImGui::Separator();

			ImGui::PopID();
		}

		
		ImGui::End();

		ImGui::Begin("Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		std::shared_ptr<Walnut::Image> image = m_Renderer.GetFinalImage();
		if (image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }, 
				ImVec2(0,1), ImVec2(1,0));

		ImGui::End();

		Render();
	}

	void Render() 
	{
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene, m_Camera);
		
		m_LastRenderTime = timer.ElapsedMillis();
	}

private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;
};


Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Pathtracer";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}

