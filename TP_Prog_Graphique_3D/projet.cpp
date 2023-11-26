/*******************************************************************************
* EasyCppOGL:   Copyright (C) 2019,                                            *
* Sylvain Thery, IGG Group, ICube, University of Strasbourg, France            *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: thery@unistra.fr                                        *
*******************************************************************************/

#include <iostream>
#include "shader_program.h"
#include "gl_viewer.h"
#include "mesh.h"
#include "fbo.h"

#define macro_str(s) #s
#define macro_xstr(s) macro_str(s)
#define DATA_PATH std::string(macro_xstr(DEF_DATA_PATH))
#define SHADERS_PATH std::string(macro_xstr(DEF_SHADERS_PATH))

// using namespace EZCOGL;

class Viewer: public EZCOGL::GLViewer
{
	EZCOGL::FBO_DepthTexture::SP fbo_depth;

	EZCOGL::ShaderProgram::UP shaderPrg;
	std::vector<EZCOGL::MeshRenderer::UP> sponzaRenderer;
	EZCOGL::MeshRenderer::UP sphereRend;
	int nbMeshParts;
	EZCOGL::GLVec3 lightPos;
	EZCOGL::GLVec3 lightDir;

	float intensity;

	std::vector<EZCOGL::GLVec3> ka; //Ambient
	std::vector<EZCOGL::GLVec3> kd; // Diffus
	std::vector<EZCOGL::GLVec3> ks; // Specular
	std::vector<float> ns; // Shininess

	std::vector<EZCOGL::Texture2D::SP> tex_kd;
	std::vector<EZCOGL::Texture2D::SP> tex_normal_map;

	bool normalMapping;

	float gamma;
	float exposure;


public:
	Viewer();
	void init_ogl() override;
	void draw_ogl() override;
	void interface_ogl() override;
};

int main(int, char**)
{
	Viewer v;
	return v.launch3d();
}

Viewer::Viewer()
{
	lightPos = EZCOGL::GLVec3(0.f, 5000.f, 0.f);
	gamma = 2.5f;
	exposure = 0.f;
	intensity = 1.f;
}

void Viewer::init_ogl()
{
	shaderPrg = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.fs")}}, "Sponza");
	
	auto tex_FBO = EZCOGL::Texture2D::create({ GL_NEAREST, GL_REPEAT });
	fbo_depth = EZCOGL::FBO_DepthTexture::create({ tex_FBO });

	// ***********************************
	// Geometry
	// ***********************************

	auto sponza = EZCOGL::Mesh::load(DATA_PATH + "/Sponza/sponza.obj")->data();
	nbMeshParts = sponza.size();

    auto me = EZCOGL::Mesh::Sphere(64);
	sphereRend = me->renderer(1, -1, -1, -1, -1);

	for (int i = 0; i < nbMeshParts; ++i)
	{
		sponzaRenderer.push_back(sponza[i]->renderer(1, 2, 3, 4, -1));
		ka.push_back(sponza[i]->material()->Ka / 100.f);
		kd.push_back(sponza[i]->material()->Kd);
		ks.push_back(sponza[i]->material()->Ks);
		ns.push_back(sponza[i]->material()->Ns);
		tex_kd.push_back(sponza[i]->material()->tex_kd);
		tex_normal_map.push_back(sponza[i]->material()->tex_norm_map);
	}

	this->cam_.set_mode(EZCOGL::Camera::Mode::MANIPULATION);

	// set scene center and radius for the init of matrix view/proj
	set_scene_center(EZCOGL::GLVec3(0.f, 0.f, 0.f));
	set_scene_radius(5000.f);

	// Define the color to use when refreshing screen
    glClearColor(0.1, 0.1, 0.1, 1.0);

}

void Viewer::draw_ogl()
{

	// Clear the buffer before to draw the next frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    // Enable Depth test
	glEnable(GL_DEPTH_TEST);

    // Get the view and projection matrix
	const EZCOGL::GLMat4& view = this->get_view_matrix();
	const EZCOGL::GLMat4& proj = this->get_projection_matrix();
    // Construct a model matrix
    const EZCOGL::GLMat4& model = EZCOGL::Transfo::rotateX(-90.0) * EZCOGL::Transfo::scale(1.5f);

	// ***********************************
	// Rendering
	// ***********************************
	shaderPrg->bind();

	const EZCOGL::GLMat4& modelSphere = EZCOGL::Transfo::translate(200.f, 200.f, -200.f) * EZCOGL::Transfo::scale(0.7f);
	EZCOGL::set_uniform_value(0, modelSphere);
	EZCOGL::set_uniform_value(3, EZCOGL::Transfo::inverse_transpose(modelSphere));
	sphereRend->draw(GL_TRIANGLES);

	// Uniforms variables send to the GPU
	EZCOGL::set_uniform_value(0, model);
	EZCOGL::set_uniform_value(1, view);
	EZCOGL::set_uniform_value(2, proj);
	EZCOGL::set_uniform_value(3, EZCOGL::Transfo::inverse_transpose(view * model));
	EZCOGL::set_uniform_value(4, EZCOGL::GLVec3(intensity, intensity, intensity));
	EZCOGL::set_uniform_value(5, EZCOGL::Transfo::sub33(view * model) * lightPos);
	EZCOGL::set_uniform_value(10, normalMapping);
	EZCOGL::set_uniform_value(11, gamma);
	EZCOGL::set_uniform_value(12, exposure);



	for (int i = 0; i < nbMeshParts; ++i)
	{
		EZCOGL::set_uniform_value(6, ka[i]);
		EZCOGL::set_uniform_value(7, kd[i]);
		EZCOGL::set_uniform_value(8, ks[i]);
		EZCOGL::set_uniform_value(9, ns[i]);
		if(tex_kd[i]){
			tex_kd[i]->bind(0);
		}
		if (tex_normal_map[i])
		{
			tex_normal_map[i]->bind(1);
		}
		sponzaRenderer[i]->draw(GL_TRIANGLES);
	}
}

void Viewer::interface_ogl()
{
	ImGui::GetIO().FontGlobalScale = 1.0f;
	ImGui::Begin("Sponza", nullptr, ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize({0,0});

	ImGui::Text("FPS :(%2.2lf)", fps_);
	if (ImGui::Button("Reload shaders"))
		shaderPrg = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.fs")}}, "Sponza");

	ImGui::SliderFloat("Light Intensity", &intensity, 0.f, 100.f);
	ImGui::SliderFloat("Light Position X", &lightPos[0], 0.f, 200.f);
	ImGui::SliderFloat("Light Position Y", &lightPos[1], 0.f, 5000.f);
	ImGui::SliderFloat("Light Position Z", &lightPos[2], 0.f, 200.f);

	ImGui::SliderFloat("Gamma", &gamma, 0.f, 100.f);
	ImGui::SliderFloat("Exposure", &exposure, 0.5f, 5.f);

	ImGui::Checkbox("Normal map", &normalMapping);


	ImGui::End();
}
