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
	EZCOGL::VBO::SP vbo_p_light;

	EZCOGL::VAO::UP vao_area_light;
	EZCOGL::ShaderProgram::UP s_ltc;

	EZCOGL::VAO::UP vao_plane;
	EZCOGL::ShaderProgram::UP s_lightplane;

	GLuint ltc1;
	GLuint ltc2;

	EZCOGL::Texture2D::SP floor_texture;
	EZCOGL::Texture2D::SP floor_roughness;
	EZCOGL::Texture2D::SP floor_displacement;
	EZCOGL::Texture2D::SP floor_normals;


	EZCOGL::GLVVec3 area_light_pos;

	EZCOGL::GLVec3 specular_color;
	EZCOGL::GLVec3 diffuse_color;
	float intensity;
	float roughness;

	float height;
	float width;
	bool double_sided;
	bool texture_based;

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
	texture_based = false;
	height = 5.0f;
	width = 5.0f;

	intensity = 1.0f;
	specular_color = EZCOGL::GLVec3(0.f, 0.f, 0.0f);
	diffuse_color = EZCOGL::GLVec3(0.f, 0.f, 0.0f);
	s_color[0] = 1.0f;
	s_color[1] = 1.0f;
	s_color[2] = 1.0f;

	d_color[0] = 1.0f;
	d_color[1] = 1.0f;
	d_color[2] = 1.0f;

	roughness = 0.5f;
	double_sided = false;
}

unsigned int loadLTC(const float* LTC) {
	GLuint LTCTexMap;
	glGenTextures(1, &LTCTexMap);
	glBindTexture(GL_TEXTURE_2D, LTCTexMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_FLOAT, LTC);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return LTCTexMap;
}

void Viewer::init_ogl()
{
	// ***********************************
	// Shader Programs
	// ***********************************
	s_ltc = EZCOGL::ShaderProgram::create({ {GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/area_lights.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/area_lights.fs")} }, "Area Lights");
	s_lightplane = EZCOGL::ShaderProgram::create({ {GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/grid.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/grid.fs")} }, "Grid");


	// ***********************************
	// Light quad renderer
	// ***********************************

	EZCOGL::VBO::SP vbo_p_plane = EZCOGL::VBO::create(EZCOGL::GLVVec3{
		{-10.0f, 0.0f, -10.0f},
		{-10.0f, 0.0f,  10.0f},
		{10.0f, 0.0f,  10.0f},
		{-10.0f, 0.0f, -10.0f},
		{10.0f, 0.0f,  10.0f},
		{10.0f, 0.0f, -10.0f},
		});

	EZCOGL::VBO::SP vbo_n_plane = EZCOGL::VBO::create(EZCOGL::GLVVec3{
		{0.0f, 1.0f, 0.0f}, 
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		});

	EZCOGL::VBO::SP vbo_tex_plane = EZCOGL::VBO::create(EZCOGL::GLVVec2{
		{ 0.0f, 0.0f },
		{ 0.0f, 1.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 1.0f, 0.0f },
		});

	vao_plane = EZCOGL::VAO::create({ {1, vbo_p_plane} , {2, vbo_n_plane} , {3, vbo_tex_plane} });

	area_light_pos = EZCOGL::GLVVec3{
		{1.0f, 2.0, -1.0f}, // 0 1 5 4
		{1.0f, 2.0,  1.0f},
		{1.0f, 0.4f,  1.0f},
		{1.0f, 2.0, -1.0f},
		{1.0f, 0.4f,  1.0f},
		{1.0f, 0.4f, -1.0f},
	};

	vbo_p_light = EZCOGL::VBO::create(area_light_pos);

	EZCOGL::VBO::SP vbo_n_light = EZCOGL::VBO::create(EZCOGL::GLVVec3{
		{1.0f, 0.0f, 0.0f}, 
		{1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		});

	EZCOGL::VBO::SP vbo_tex_light = EZCOGL::VBO::create(EZCOGL::GLVVec2{
		{ 0.0f, 0.0f },
		{ 0.0f, 1.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 1.0f, 0.0f },
		});

	vao_area_light = EZCOGL::VAO::create({ {1, vbo_p_light} , {2, vbo_n_light} , {3, vbo_tex_light} });

	
	// ***********************************
	// Textures setup
	// ***********************************

	ltc1 = loadLTC(LTC1);
	ltc2 = loadLTC(LTC2);

	floor_texture = EZCOGL::Texture2D::create();
	floor_texture->load(DATA_PATH + "/area_light_textures/floor_tiles_06_diff_4k.jpg");

	floor_roughness = EZCOGL::Texture2D::create();
	floor_roughness->load(DATA_PATH + "/area_light_textures/floor_tiles_06_rough_4k.jpg");

	floor_displacement = EZCOGL::Texture2D::create();
	floor_displacement->load(DATA_PATH + "/area_light_textures/floor_tiles_06_disp_4k.png");

	floor_normals = EZCOGL::Texture2D::create();
	floor_normals->load(DATA_PATH + "/area_light_textures/floor_tiles_06_nor_gl_4k.exr");



	// set scene center and radius for the init of matrix view/proj
	set_scene_center(EZCOGL::GLVec3(0.f, 0.f, 0.f));
	set_scene_radius(120.f);

	glEnable(GL_TEXTURE_2D);
}

void setInt(unsigned int id, const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

EZCOGL::GLVVec3 updateAreaLightPositions(EZCOGL::VBO::SP vbo, float height, float width) {
	EZCOGL::GLVVec3 area_light_pos = {
		{1.0f, height, -width},
		{1.0f, height,  width},
		{1.0f, 0.6f,    width},
		{1.0f, height, -width},
		{1.0f, 0.6f,    width},
		{1.0f, 0.6f,   -width},
	};

	// Assuming vbo_p_light is a member variable of your class
	vbo->update(area_light_pos);

	return area_light_pos;
}

void Viewer::draw_ogl()
{
	specular_color[0] = s_color[0];
	specular_color[1] = s_color[1];
	specular_color[2] = s_color[2];

	diffuse_color[0] = d_color[0];
	diffuse_color[1] = d_color[1];
	diffuse_color[2] = d_color[2];

	area_light_pos = updateAreaLightPositions(vbo_p_light, height, width);

	// Get the view and projection matrix
	const EZCOGL::GLMat4& view = this->get_view_matrix();
	const EZCOGL::GLMat4& proj = this->get_projection_matrix();
	// Construct a model matrix
	const EZCOGL::GLMat4& model = EZCOGL::GLMat4::Identity();

	EZCOGL::GLVec3 camera_pos = this->cam_.getCameraPosition();

	// ***********************************
	// Light quad pass
	// ***********************************
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	s_ltc->bind();
	EZCOGL::set_uniform_value(1, model);
	EZCOGL::set_uniform_value(2, view);
	EZCOGL::set_uniform_value(3, proj);
	EZCOGL::set_uniform_value(4, EZCOGL::Transfo::inverse_transpose(model));
	EZCOGL::set_uniform_value(8, intensity);

	EZCOGL::set_uniform_value(9, specular_color);
	EZCOGL::set_uniform_value(10, diffuse_color);
	EZCOGL::set_uniform_value(11, roughness);

	EZCOGL::set_uniform_value(12, area_light_pos[0]);
	EZCOGL::set_uniform_value(13, area_light_pos[1]);
	EZCOGL::set_uniform_value(14, area_light_pos[4]);
	EZCOGL::set_uniform_value(15, area_light_pos[5]);
	EZCOGL::set_uniform_value(17, double_sided);
	//Precalculated textures from original authors
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ltc1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ltc2);
	setInt(s_ltc->id(), "LTC1", 0);
	setInt(s_ltc->id(), "LTC2", 1);
	floor_texture->bind(2);
	floor_roughness->bind(3);
	floor_displacement->bind(4);
	floor_normals->bind(5);
	EZCOGL::set_uniform_value(18, texture_based);

	//Draw
	vao_plane->bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	vao_plane->unbind();


	// ***********************************
	// Grid/Plane pass
	// ***********************************

	s_lightplane->bind();
	EZCOGL::set_uniform_value(1, model);
	EZCOGL::set_uniform_value(2, view);
	EZCOGL::set_uniform_value(3, proj);
	EZCOGL::set_uniform_value(4, intensity);

	//Draw
	vao_area_light->bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	vao_area_light->unbind();

}

void Viewer::interface_ogl()
{
	ImGui::GetIO().FontGlobalScale = 1.0f;

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);


	ImGui::Begin("Area lights - LTC", nullptr, ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize({ 0,0 });

	ImGui::Text("FPS :(%2.2lf)", fps_);
	if (ImGui::Button("Reload shaders")) {
		s_ltc = EZCOGL::ShaderProgram::create({ {GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/area_lights.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/area_lights.fs")} }, "Area Lights");
		s_lightplane = EZCOGL::ShaderProgram::create({ {GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/grid.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/grid.fs")} }, "Grid");
	}

	ImGui::SliderFloat("Light Intensity", &intensity, 0.f, 10.f);

	ImGui::SliderFloat("Roughness", &roughness, 0.f, 1.f);
	ImGui::Checkbox("Texture-based roughness", &texture_based);

	ImGui::SliderFloat("Height", &height, 1.f, 20.);
	ImGui::SliderFloat("Width", &width, 1.f, 20.f);
	ImGui::Checkbox("Double-sided", &double_sided);


	if (ImGui::CollapsingHeader("Specular color picker")) {
		ImGui::ColorPicker3("Specular color", s_color);
	}

	if (ImGui::CollapsingHeader("Diffuse color picker")) {
		ImGui::ColorPicker3("Diffuse color", d_color);
	}

	ImGui::End();
}
