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
#include "ltc_matrix.hpp"

#define macro_str(s) #s
#define macro_xstr(s) macro_str(s)
#define DATA_PATH std::string(macro_xstr(DEF_DATA_PATH))
#define SHADERS_PATH std::string(macro_xstr(DEF_SHADERS_PATH))

// using namespace EZCOGL;

class Viewer : public EZCOGL::GLViewer
{
	EZCOGL::ShaderProgram::UP s_arealight;
	EZCOGL::MeshRenderer::UP r_arealight;

	EZCOGL::MeshRenderer::UP r_grid;
	EZCOGL::ShaderProgram::UP s_grid;

	GLuint ltc1;
	GLuint ltc2;

	EZCOGL::GLVec3 points[4];

	EZCOGL::GLVec3 specular_color;
	EZCOGL::GLVec3 diffuse_color;
	float intensity;
	float roughness;

	float s_color[3];
	float d_color[3];

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

Viewer::Viewer(){
	intensity = 1.0f;
	specular_color = EZCOGL::GLVec3(0.f, 0.f, 0.0f);
	diffuse_color = EZCOGL::GLVec3(0.f, 0.f, 0.0f);
	roughness = 0.5f;
}

unsigned int loadMTexture(float* matrixTable) {
	unsigned int texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_FLOAT, matrixTable);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

void Viewer::init_ogl()
{
	// ***********************************
	// Shader Programs
	// ***********************************
	s_arealight = EZCOGL::ShaderProgram::create({ {GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/area_lights.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/area_lights.fs")} }, "Area Lights");
	s_grid = EZCOGL::ShaderProgram::create({ {GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/grid.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/grid.fs")} }, "Grid");


	// ***********************************
	// Light quad renderer
	// ***********************************
	auto light_quad = EZCOGL::Mesh::Grid();
	// positions (1), normals(2), tex coords (3)
	r_arealight = light_quad->renderer(1, 2, 3, -1, -1);
	auto vert = light_quad->vertices();

	EZCOGL::GLVec3 topLeft = vert[0];
	EZCOGL::GLVec3 topRight = vert[3];
	EZCOGL::GLVec3 bottomLeft = vert[12];
	EZCOGL::GLVec3 bottomRight = vert[15];

	points[0] = topLeft;
	points[1] = topRight;
	points[2] = bottomLeft;
	points[3] = bottomRight;

	// ***********************************
	// Grid renderer
	// ***********************************
	auto grid = EZCOGL::Mesh::Grid();
	// positions (1), normals(2), tex coords (3)
	r_grid = grid->renderer(1, 2, 3, -1, -1);


	// ***********************************
	// Textures setup
	// ***********************************
	ltc1 = loadMTexture(LTC1);
	ltc2 = loadMTexture(LTC2);

	// ***********************************
	// Camera setup
	// ***********************************
	this->cam_.set_mode(EZCOGL::Camera::Mode::MANIPULATION);
	this->cam_.show_entire_scene();

	// set scene center and radius for the init of matrix view/proj
	set_scene_center(EZCOGL::GLVec3(0.f, 0.f, 0.f));
	set_scene_radius(120.f);

	glEnable(GL_TEXTURE_2D);
}

void Viewer::draw_ogl()
{
	specular_color[0] = s_color[0];
	specular_color[1] = s_color[1];
	specular_color[2] = s_color[2];

	diffuse_color[0] = d_color[0];
	diffuse_color[1] = d_color[1];
	diffuse_color[2] = d_color[2];


	// Get the view and projection matrix
	const EZCOGL::GLMat4& view = this->get_view_matrix();
	const EZCOGL::GLMat4& proj = this->get_projection_matrix();
	// Construct a model matrix
	const EZCOGL::GLMat4& model = EZCOGL::Transfo::scale(1.5f);

	// ***********************************
	// Light quad pass
	// ***********************************
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	s_arealight->bind();

	const EZCOGL::GLMat3& normalMatrix = model.block<3, 3>(0, 0);;

	EZCOGL::set_uniform_value(1, model * EZCOGL::Transfo::translate(EZCOGL::GLVec3(0.f, 0.1f, 0.f)) * EZCOGL::Transfo::rotateX(90) * EZCOGL::Transfo::scale(50));
	EZCOGL::set_uniform_value(2, view);
	EZCOGL::set_uniform_value(3, proj);
	EZCOGL::set_uniform_value(4, normalMatrix);

	//Precalculated textures from original authors
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ltc1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ltc2);

	EZCOGL::set_uniform_value(8, EZCOGL::GLVec3(intensity, intensity, intensity));

	EZCOGL::set_uniform_value(9, specular_color);
	EZCOGL::set_uniform_value(10, specular_color);
	EZCOGL::set_uniform_value(11, roughness);

	EZCOGL::set_uniform_value(12, points[0]);
	EZCOGL::set_uniform_value(13, points[1]);
	EZCOGL::set_uniform_value(14, points[2]);
	EZCOGL::set_uniform_value(15, points[3]);

	r_grid->draw(GL_TRIANGLES);

	// ***********************************
	// Grid/Plane pass
	// ***********************************

	s_grid->bind();
	EZCOGL::set_uniform_value(1, model * EZCOGL::Transfo::scale(10) * EZCOGL::Transfo::translate(EZCOGL::GLVec3(0.f, 1.5f, 0.f)));
	EZCOGL::set_uniform_value(2, view);
	EZCOGL::set_uniform_value(3, proj);
	r_arealight->draw(GL_TRIANGLES);
}

void Viewer::interface_ogl()
{
	ImGui::GetIO().FontGlobalScale = 1.0f;
	ImGui::Begin("Area lights - LTC", nullptr, ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize({ 0,0 });

	ImGui::Text("FPS :(%2.2lf)", fps_);
	if (ImGui::Button("Reload shaders")) {
		s_arealight = EZCOGL::ShaderProgram::create({ {GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/area_lights.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/area_lights.fs")} }, "Area Lights");
		s_grid = EZCOGL::ShaderProgram::create({ {GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/grid.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/grid.fs")} }, "Grid");
	}

	ImGui::SliderFloat("Light Intensity", &intensity, 0.f, 10.);
	ImGui::SliderFloat("Roughness", &roughness, 0.f, 1.f);

	ImGui::ColorPicker3("Specular color", s_color);
	ImGui::ColorPicker3("Diffuse color", d_color);

	if (ImGui::CollapsingHeader("LTC1 texture content")){
		ImVec2 imageSize(200.0f, 200.0f);
		ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(ltc1)), imageSize);
	}
	if (ImGui::CollapsingHeader("LTC2 texture content")) {
		ImVec2 imageSize(200.0f, 200.0f);
		ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(ltc2)), imageSize);
	}

	ImGui::End();
}
