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
		white.Specular = 0.0f;
		white.Roughness = 0.6f;

		Material& glossy = m_Scene.Materials.emplace_back();
		glossy.Albedo = { 0.4f, 0.8f ,0.8f };
		glossy.Specular = 0.0f;
		glossy.Roughness = 0.2f;
		glossy.IOR = 1.4f;

		Material& metal = m_Scene.Materials.emplace_back();
		metal.Albedo = { 0.9f, 0.9f ,0.9f };
		metal.Specular = 1.0f;
		metal.Roughness = 0.1f;
		metal.IOR = 1.5f;

		Material& glass= m_Scene.Materials.emplace_back();
		glass.Albedo = { 1.0f, 1.0f ,1.0f };
		glass.Specular = 0.0f;
		glass.Roughness = 0.1f;
		glass.IOR = 1.5f;
		glass.Transmission = 1.0f;
		glass.TransmissionCoeff = { 0.2f, 0.2f ,0.2f };
		
		Material& whiteLight = m_Scene.Materials.emplace_back();
		whiteLight.Albedo = { 1.0f, 1.0f , 1.0f }; 
		whiteLight.Roughness = 1.0f; 
		whiteLight.EmissionColor = whiteLight.Albedo;
		whiteLight.EmissionPower = 10.0f;

		Tri triangle;
		triangle.Position = { 0.0f, 0.0f, 0.0f };
		triangle.v0 = { -1.0f, 0.0f, 0.0f };
		triangle.v1 = { 1.0f, 0.0f, 0.0f };
		triangle.v2 = { 1.0f, 0.0f, 1.0f };
		triangle.MaterialIndex = 4;
		m_Scene.Tris.push_back(triangle);
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
		ImGui::ColorEdit3("Sky Color", glm::value_ptr(m_Renderer.GetSettings().SkyColor));
		ImGui::DragFloat("Exposure", &m_Renderer.GetSettings().Exposure, 0.1f, 0.0f, 4.0f);
		ImGui::DragInt("Bounces", &m_Renderer.GetSettings().Bounces, 1.0f, 2.0f, 8.0f);
		ImGui::Checkbox("AA", &m_Renderer.GetSettings().Antialiasing); 


		if (ImGui::Button("Reset"))
		{
			m_Renderer.ResetFrameIndex();
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

