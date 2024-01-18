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
#include "texturecubemap.h"

#define macro_str(s) #s
#define macro_xstr(s) macro_str(s)
#define DATA_PATH std::string(macro_xstr(DEF_DATA_PATH))
#define SHADERS_PATH std::string(macro_xstr(DEF_SHADERS_PATH))

// using namespace EZCOGL;

class Viewer: public EZCOGL::GLViewer
{
	EZCOGL::FBO_DepthTexture::SP fbo_depth;

	EZCOGL::VAO::UP vao;
	EZCOGL::EBO::SP ebo;

	EZCOGL::ShaderProgram::UP shaderPrg;
	EZCOGL::ShaderProgram::UP shadowShader;
	EZCOGL::ShaderProgram::UP skyboxShader;
	EZCOGL::ShaderProgram::UP grassShader;
	EZCOGL::ShaderProgram::UP landscapeShader;


	EZCOGL::MeshRenderer::UP renderer_landscape;
	std::vector<EZCOGL::MeshRenderer::UP> sponzaRenderer;
	EZCOGL::MeshRenderer::UP sphereRend;
	EZCOGL::MeshRenderer::UP skyRend;

	size_t nbMeshParts;
	EZCOGL::GLVec3 lightPos;
	EZCOGL::GLVec3 lightDir;

	float intensity;

	std::vector<EZCOGL::GLVec3> ka; //Ambient
	std::vector<EZCOGL::GLVec3> kd; // Diffus
	std::vector<EZCOGL::GLVec3> ks; // Specular
	std::vector<float> ns; // Shininess

	std::vector<EZCOGL::Texture2D::SP> tex_kd;
	std::vector<EZCOGL::Texture2D::SP> tex_normal_map;
	
	std::vector<EZCOGL::GLVec3> positions;


	EZCOGL::TextureCubeMap::SP tex_envMap;
	EZCOGL::Texture2D::SP tex_FBO;
	EZCOGL::Texture2D::SP tex_grass;
	EZCOGL::Texture2D::SP tex_wind;
	EZCOGL::Texture2D::SP atlas_grass;

	bool normalMapping;
	bool nav_mode;

	bool wKeyPressed;
	float movementSpeed;


	float gamma;
	float exposure;
	float bias;
	float grass_min_size;
	float wind_speed;
	float more_flowers;

	int resolution;
	int scale;

	bool sponza;
	bool camera_set;

	uint32_t nbVertex;

	EZCOGL::GLVec3 camera_position;


public:
	Viewer();
	void init_ogl() override;
	void draw_ogl() override;
	void interface_ogl() override;
	void key_press_ogl(int32_t) override;
};

int main(int, char**)
{
	Viewer v;
	return v.launch3d();
}

Viewer::Viewer()
{
	sponza = true;
	resolution = 50;
	scale = 5;
	lightPos = EZCOGL::GLVec3(0.f, 3200.f, 0.01f);
	gamma = 0.6f;
	exposure = 0.4f;
	intensity = 25.f;
	bias = 1.f;
	nav_mode = true;
	wKeyPressed = false;
	movementSpeed = 1.0f;
	grass_min_size = 2.4f;
	wind_speed = 0.15;
	camera_set = false;
	more_flowers = 0.1f;
}

void Viewer::key_press_ogl(int32_t key_code) {
	if (this->cam_.is_navigation_mode()) {
		switch (key_code) {
		case 'W':
			wKeyPressed = true;
			break;
		default:
			wKeyPressed = false;
			break;
		}
	}
}



void Viewer::init_ogl()
{
	// ***********************************
	// Shader Programs
	// ***********************************
	skyboxShader = EZCOGL::ShaderProgram::create({ {GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/skybox.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/skybox.fs")} }, "Skybox");
	shaderPrg = EZCOGL::ShaderProgram::create({{GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.fs")}}, "Sponza");
	shadowShader = EZCOGL::ShaderProgram::create({ {GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/shadow.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/shadow.fs")} }, "Shadow");
	grassShader = EZCOGL::ShaderProgram::create({ {GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/grass.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/grass.fs")}, {GL_GEOMETRY_SHADER, EZCOGL::load_src(SHADERS_PATH + "/grass.gs")} }, "Grass");
	landscapeShader = EZCOGL::ShaderProgram::create({ {GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/landscape.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/landscape.fs")} }, "landscape");


	// ***********************************
	// Geometry - GRASS
	// ***********************************

	auto rend = EZCOGL::Mesh::Grid();
	// and it's associate renderer with VBO positions (1) and VBO tex coords (2)
	renderer_landscape = rend->renderer(1, 3, 2, -1, -1);

	for (float x = -75.0f; x < 75.0f; x += 0.2f)
		for (float z = -75.0f; z < 75.0f; z += 0.2f)
		{
			positions.push_back(EZCOGL::GLVec3(x, 0, z));
		}


	auto vbo_p = EZCOGL::VBO::create(positions);
	nbVertex = vbo_p->length();
	vao = EZCOGL::VAO::create({ {1, vbo_p} });

	tex_grass = EZCOGL::Texture2D::create();
	tex_grass->load(DATA_PATH + "/textures/grass_texture.png");

	atlas_grass = EZCOGL::Texture2D::create();
	atlas_grass->load(DATA_PATH + "/textures/grass_atlas.png");


	tex_wind = EZCOGL::Texture2D::create();
	tex_wind->load(DATA_PATH + "/textures/wind.jpg");


	// ***********************************
	// Shadow Texture
	// ***********************************
	tex_FBO = EZCOGL::Texture2D::create({ GL_NEAREST, GL_REPEAT });
	tex_FBO->init(GL_RGBA8);
	fbo_depth = EZCOGL::FBO_DepthTexture::create({ tex_FBO });
	fbo_depth->resize(8192, 8192);

	// ***********************************
	// Skybox renderer + Texture
	// ***********************************
	auto meshCube = EZCOGL::Mesh::CubePosOnly();
	skyRend = meshCube->renderer(1, -1, -1, -1, -1); // We only need the 3D position of vertices

	tex_envMap = EZCOGL::TextureCubeMap::create({ GL_LINEAR, GL_REPEAT });
	tex_envMap->load({ DATA_PATH + "/skybox/miramar_rt.tga", 
					   DATA_PATH + "/skybox/miramar_lf.tga", 
					   DATA_PATH + "/skybox/miramar_up.tga", 
		               DATA_PATH + "/skybox/miramar_dn.tga", 
					   DATA_PATH + "/skybox/miramar_bk.tga", 
					   DATA_PATH + "/skybox/miramar_ft.tga" });

	// ***********************************
	// Sponza renderer
	// ***********************************
	auto sponza = EZCOGL::Mesh::load(DATA_PATH + "/Sponza/sponza.obj")->data();
	nbMeshParts = sponza.size();

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

	// ***********************************
	// Sphere renderer
	// ***********************************
    auto me = EZCOGL::Mesh::Sphere(64);
	sphereRend = me->renderer(1, -1, -1, -1, -1);

	this->cam_.set_mode(EZCOGL::Camera::Mode::NAVIGATION);
	this->cam_.show_entire_scene();


	// set scene center and radius for the init of matrix view/proj
	set_scene_center(EZCOGL::GLVec3(0.f, 0.f, 0.f));
	set_scene_radius(5000.f);

	// Define the color to use when refreshing screen
    glClearColor(0.1, 0.1, 0.1, 1.0);

}

void Viewer::draw_ogl()
{
	// Get the view and projection matrix
	const EZCOGL::GLMat4& view = this->get_view_matrix();
	const EZCOGL::GLMat4& proj = this->get_projection_matrix();
	// Construct a model matrix
	const EZCOGL::GLMat4& model = EZCOGL::Transfo::scale(1.5f);


	if (sponza) {
		camera_set = false;
		if (wKeyPressed) {
			this->cam_.frame_.translation().z() += movementSpeed;
			this->ask_update();
		}
		set_scene_center(EZCOGL::GLVec3(0.f, 0.f, 0.f));
		set_scene_radius(5000.f);


		// ***********************************
		// FIRST PASS - SHADOWS
		// ***********************************
		EZCOGL::GLVec3 lookDir = this->get_camera().pivot_point_f() - lightPos;
		EZCOGL::GLMat4 lView = EZCOGL::Transfo::look_dir(lightPos, lookDir, EZCOGL::GLVec3(0.f, 1.f, 0.f));
		float radius = this->get_camera().scene_radius();
		EZCOGL::GLMat4 lProj = (EZCOGL::Transfo::ortho(radius, radius, std::max(lookDir.norm() - radius, 0.01f), lookDir.norm() + radius)).transpose();
		EZCOGL::GLMat4 lightSpaceMatrix = lProj * lView;

		glEnable(GL_DEPTH_TEST);

		EZCOGL::FBO::push();
		fbo_depth->bind();

		//Clear color buffer bit (no depth here)
		glClear(GL_DEPTH_BUFFER_BIT);

		shadowShader->bind();

		EZCOGL::set_uniform_value(0, model);
		EZCOGL::set_uniform_value(1, lightSpaceMatrix);

		for (int i = 0; i < nbMeshParts; ++i)
		{
			sponzaRenderer[i]->draw(GL_TRIANGLES);
		}

		// ***********************************
		// SECOND PASS - SKYBOX
		// ***********************************

		EZCOGL::FBO::pop();

		// Clear the buffer before to draw the next frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Construct view matrix for skybox
		EZCOGL::GLMat4 vi = view;
		vi.block<3, 1>(0, 3).setZero();	// remove translation
		vi.block<3, 1>(0, 1).normalize();
		vi.block<3, 1>(0, 2).normalize();
		vi.block<3, 1>(0, 3).normalize();

		glDisable(GL_DEPTH_TEST);

		skyboxShader->bind();
		EZCOGL::set_uniform_value(0, vi);
		EZCOGL::set_uniform_value(1, proj);
		tex_envMap->bind(0);
		skyRend->draw(GL_TRIANGLES);

		glEnable(GL_DEPTH_TEST);


		// ***********************************
		// THIRD PASS - SPONZA MODEL PASS
		// ***********************************
		shaderPrg->bind();

		const EZCOGL::GLMat4& modelSphere = EZCOGL::Transfo::translate(lightPos) * EZCOGL::Transfo::scale(100.f);
		EZCOGL::set_uniform_value(0, modelSphere);
		EZCOGL::set_uniform_value(3, EZCOGL::Transfo::inverse_transpose(modelSphere));
		sphereRend->draw(GL_TRIANGLES);

		// Uniforms variables send to the GPU
		EZCOGL::set_uniform_value(0, model);
		EZCOGL::set_uniform_value(1, view);
		EZCOGL::set_uniform_value(2, proj);
		EZCOGL::set_uniform_value(3, EZCOGL::Transfo::inverse_transpose(model * view));
		EZCOGL::set_uniform_value(4, EZCOGL::GLVec3(intensity, intensity, intensity));
		EZCOGL::set_uniform_value(5, EZCOGL::Transfo::sub33(model * view) * lightPos);
		EZCOGL::set_uniform_value(10, normalMapping);
		EZCOGL::set_uniform_value(11, gamma);
		EZCOGL::set_uniform_value(12, exposure);
		EZCOGL::set_uniform_value(13, lightSpaceMatrix);
		EZCOGL::set_uniform_value(15, bias);

		tex_envMap->bind(3);

		for (int i = 0; i < nbMeshParts; ++i)
		{
			EZCOGL::set_uniform_value(6, ka[i]);
			EZCOGL::set_uniform_value(7, kd[i]);
			EZCOGL::set_uniform_value(8, ks[i]);
			EZCOGL::set_uniform_value(9, ns[i]);
			if (tex_kd[i]) {
				tex_kd[i]->bind(0);
			}
			if (tex_normal_map[i])
			{
				tex_normal_map[i]->bind(1);
			}
			fbo_depth->depth_texture()->bind(2);
			sponzaRenderer[i]->draw(GL_TRIANGLES);
		}
	}
	else {
		// ***********************************
		// FOURTH PASS - GRASS PASS
		// ***********************************
		// Clear the buffer before to draw the next frame

		if (wKeyPressed) {
			std::cout << "W Pressed" << std::endl;
			this->cam_.frame_.translation().z() += 0.3f;
			this->ask_update();
		}

		if (!camera_set) {
			this->cam_.frame_.translation().x() = 0.f;
			this->cam_.frame_.translation().y() = 0.f;
			this->cam_.frame_.translation().z() = -15.f;
			this->cam_.show_entire_scene();
			set_scene_center(EZCOGL::GLVec3(0.f, 0.f, 0.f));
			set_scene_radius(150.f);
			camera_set = true;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		landscapeShader->bind();
		EZCOGL::set_uniform_value(1, model* EZCOGL::Transfo::translate(EZCOGL::GLVec3(0.f, 0.1f, 0.f))* EZCOGL::Transfo::rotateX(90)* EZCOGL::Transfo::scale(50));
		EZCOGL::set_uniform_value(2, view);
		EZCOGL::set_uniform_value(3, proj);
		renderer_landscape->draw(GL_TRIANGLES);

		float time = EZCOGL::current_time();

		camera_position = EZCOGL::GLVec3(this->cam_.get_view_matrix().block<3, 1>(0, 3));

		grassShader->bind();
		EZCOGL::set_uniform_value(1, model);
		EZCOGL::set_uniform_value(2, view);
		EZCOGL::set_uniform_value(3, proj);
		EZCOGL::set_uniform_value(4, tex_grass->bind(0));
		EZCOGL::set_uniform_value(9, atlas_grass->bind(2));
		EZCOGL::set_uniform_value(5, tex_wind->bind(1));
		EZCOGL::set_uniform_value(6, time);
		EZCOGL::set_uniform_value(7, grass_min_size);
		EZCOGL::set_uniform_value(8, wind_speed);
		EZCOGL::set_uniform_value(10, more_flowers);

		vao->bind();
		glPointSize(5.0f);
		glDrawArrays(GL_POINTS, 0, nbVertex);
	}
}

void Viewer::interface_ogl()
{
	ImGui::GetIO().FontGlobalScale = 1.0f;
	ImGui::Begin("Sponza", nullptr, ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize({0,0});

	ImGui::Text("FPS :(%2.2lf)", fps_);
	if (ImGui::Button("Reload shaders")) {
		skyboxShader = EZCOGL::ShaderProgram::create({ {GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/skybox.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/skybox.fs")} }, "Skybox");
		shaderPrg = EZCOGL::ShaderProgram::create({ {GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/projet.fs")} }, "Sponza");
		shadowShader = EZCOGL::ShaderProgram::create({ {GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/shadow.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/shadow.fs")} }, "Shadow");
		grassShader = EZCOGL::ShaderProgram::create({ {GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/grass.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/grass.fs")}, {GL_GEOMETRY_SHADER, EZCOGL::load_src(SHADERS_PATH + "/grass.gs")} }, "Grass");
		landscapeShader = EZCOGL::ShaderProgram::create({ {GL_VERTEX_SHADER, EZCOGL::load_src(SHADERS_PATH + "/landscape.vs")}, {GL_FRAGMENT_SHADER, EZCOGL::load_src(SHADERS_PATH + "/landscape.fs")} }, "landscape");
	}

	ImGui::Checkbox("Sponza ON", &sponza);

	if (ImGui::CollapsingHeader("Sponza")) {
		ImGui::SliderFloat("Light Intensity", &intensity, 0.f, 300.f);
		ImGui::SliderFloat("Light Position X", &lightPos[0], -10000.f, 10000.f);
		ImGui::SliderFloat("Light Position Y", &lightPos[1], -10000.f, 10000.f);
		ImGui::SliderFloat("Light Position Z", &lightPos[2], -10000.f, 10000.f);

		ImGui::SliderFloat("Gamma", &gamma, 0.f, 3.f);
		ImGui::SliderFloat("Exposure", &exposure, 0.f, 5.f);

		ImGui::Checkbox("Normal map", &normalMapping);
	}

	if (ImGui::CollapsingHeader("Grass")) {
		ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", camera_position.x(), camera_position.y(), camera_position.z());

		ImGui::SliderFloat("Grass size", &grass_min_size, 1.5f, 10.f);
		ImGui::SliderFloat("Flower quantity", &more_flowers, 0.f, 1.f);
		ImGui::SliderFloat("Wind speed", &wind_speed, 0.0f, 3.f);
	}


	if (ImGui::CollapsingHeader("FBO texture content"))
		ImGui::Image(reinterpret_cast<ImTextureID>(fbo_depth->depth_texture()->id()), ImVec2(400, 400), ImVec2(0, 1), ImVec2(1, 0));


	ImGui::End();
}
