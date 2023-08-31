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
		white.Specular = 0.02f;
		white.Roughness = 0.7f;

		Material& red = m_Scene.Materials.emplace_back();
		red.Albedo = { 0.8f, 0.3f ,0.2f };
		red.Specular = 0.02f;
		red.Roughness = 0.7f;

		Material& green = m_Scene.Materials.emplace_back();
		green.Albedo = { 0.3f, 0.8f ,0.2f };
		green.Specular = 0.02f;
		green.Roughness = 0.7f;
		
		Material& whiteLight = m_Scene.Materials.emplace_back();
		whiteLight.Albedo = { 1.0f, 1.0f , 1.0f }; 
		whiteLight.Roughness = 1.0f; 
		whiteLight.EmissionColor = whiteLight.Albedo;
		whiteLight.EmissionPower = 10.0f;

		{
			Object cube;
			int numTris = 0;
			// find out how many triangles we need to create for this mesh
			for (uint32_t i = 0; i < cube.numFaces; ++i) {
				numTris += cube.faceIndex[i] - 2;
			}

			cube.trisIndex[numTris * 3];

			// generate the triangle index array
			for (uint32_t i = 0, k = 0; i < cube.numFaces; ++i) {  //for each  face  
				for (uint32_t j = 0; j < cube.faceIndex[i] - 2; ++j) {  //for each triangle in the face  
					
					Tri triangle;
					triangle.v0 = cube.verts[cube.vertexIndex[k]];				// v0
					triangle.v1 = cube.verts[cube.vertexIndex[k + j + 1]];		// v1
					triangle.v2 = cube.verts[cube.vertexIndex[k + j + 2]];		// v2
					triangle.RecalculateFlippedNormal();
					triangle.MaterialIndex = 0;
					m_Scene.Tris.push_back(triangle);

				}
				k += cube.faceIndex[i];
			}
		}
		
		{
			Tri tr;
			tr.v0 = {0.2f,  0.99f, -0.2f };
			tr.v1 = {0.2f,  0.99f, 0.2f };
			tr.v2 = {-0.2f, 0.99f, 0.2f };
			tr.RecalculateNormal();
			tr.MaterialIndex = 3;
			m_Scene.Tris.push_back(tr);
		}

		{
			Tri tr;
			tr.v0 = { 0.2f,  0.99f, -0.2f };
			tr.v1 = { -0.2f, 0.99f, 0.2f };
			tr.v2 = { -0.2f, 0.99f, -0.2f };
			tr.RecalculateNormal();
			tr.MaterialIndex = 3;
			m_Scene.Tris.push_back(tr);
		}

		

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
		ImGui::Text("Triangles: %i", m_Scene.Tris.size());
		
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

		ImGui::Begin("Materials");

		for (size_t i = 0; i < m_Scene.Materials.size(); i++)
		{
			ImGui::PushID(i);


			Material& material = m_Scene.Materials[i];
			ImGui::Text("Material %i", i);
			ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
			ImGui::DragFloat("Roughness", &material.Roughness, 0.05f, 0.0f, 1.0f); 
			ImGui::DragFloat("Metallic", &material.Specular, 0.05f, 0.0f, 1.0f); 
			ImGui::DragFloat("Transmission", &material.Transmission, 0.05f, 0.0f, 1.0f); 
			ImGui::DragFloat("Emission Strength", &material.EmissionPower, 0.05f, 0.0f, FLT_MAX); 
			ImGui::ColorEdit3("Emission", glm::value_ptr(material.EmissionColor));

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

