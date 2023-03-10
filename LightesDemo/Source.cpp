
#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include<gl\glm\glm.hpp>
#include<gl\glm\gtc\type_ptr.hpp>
#include<gl\glm\gtx\transform.hpp>
#include<vector>

using namespace std;
using namespace glm;

struct vertex
{

	vec3 position;
	vec3 color;
	vec2 uv;
	vertex() {}
	vertex(vec3 _position, vec3 _color) :position{ _position }, color{ _color }, uv(1, 1)//normal
	{
	}
	vertex(vec3 _position) :position{ _position }, color(1, 1, 1), uv(1, 1)
	{
	}
	vertex(vec3 _position, vec3 _norm, vec2 _uv) :position{ _position }, color{ _norm }, uv{ _uv }
	{
	}
};

const GLint WIDTH = 720, HEIGHT = 720;
GLuint Cube_VBO, IBO,Sphere_VBO_Bulb,Sphere_VBO_Fly=0;
GLuint programId, phongProgramId, skyboxProgramId, reflectionProgramId,refractionProgramId;
GLuint skyboxTextureId;
GLuint modelMatLoc, viewMatLoc, projMatLoc;
GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name);
mat4 ModelMat, viewMat, projMat;

#pragma region Sphere
vector<vertex> sphere_vertices_bulb{};
vector<vertex> sphere_vertices_fly{};
vec3 Sphere_Core_vertices[4] = {
	vec3(0.0, 0.0, 1.0),
	vec3(0.0, 0.942809, -0.333333),
	vec3(-0.816497, -0.471405, -0.333333),
	vec3(0.816497, -0.471405, -0.333333)
};


void Triangle(vec3 a, vec3 b, vec3 c,vec3 color,vector<vertex> & sphere_vertices)
{
	vec3 normal = (a + b + c) / 3.0f;
	vec3 normal2 = cross((c - a), (b - a));
	sphere_vertices.push_back(vertex(a, color));
	sphere_vertices.push_back(vertex(b, color));
	sphere_vertices.push_back(vertex(c, color));
	
}

void dividTriangle(vec3 a, vec3 b, vec3 c,vec3 color, vector<vertex> & sphere_vertices, int itertions)
{
	if (itertions > 0)
	{
		vec3 v1 = normalize(a + b);
		vec3 v2 = normalize(a + c);
		vec3 v3 = normalize(b + c);

		dividTriangle(a, v1, v2, color, sphere_vertices, itertions - 1);
		dividTriangle(v1, b, v3, color, sphere_vertices, itertions - 1);
		dividTriangle(v1, v3, v2, color, sphere_vertices, itertions - 1);
		dividTriangle(v2, v3, c, color, sphere_vertices, itertions - 1);
		
	}
	else
	{
		Triangle(a, b, c, color, sphere_vertices);
		
	}
}
void CreateSphere(GLuint & VBO ,vec3 color, vector<vertex> & sphere_vertices,int iterations)
{
	sphere_vertices.clear();
	dividTriangle(Sphere_Core_vertices[0], Sphere_Core_vertices[1], Sphere_Core_vertices[2], color, sphere_vertices, iterations);
	dividTriangle(Sphere_Core_vertices[0], Sphere_Core_vertices[3], Sphere_Core_vertices[1], color, sphere_vertices, iterations);
	dividTriangle(Sphere_Core_vertices[0], Sphere_Core_vertices[2], Sphere_Core_vertices[3], color, sphere_vertices, iterations);
	dividTriangle(Sphere_Core_vertices[3], Sphere_Core_vertices[2], Sphere_Core_vertices[1], color, sphere_vertices, iterations);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sphere_vertices.size() * sizeof(vertex), sphere_vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(vertex), (char*)(2 * sizeof(vec3)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(3);

	
}
void BindSphere(GLuint VBO)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(vertex), (char*)(2 * sizeof(vec3)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(3);
	
}


#pragma endregion 

#pragma region Cube

vertex cube_core_vertices2[] = {
{vec3(-0.5, 0.5, 0.5),vec3(1,1,0),vec2(0,0.5)},
{vec3(-0.5, -0.5, 0.5),vec3(0,1,0),vec2(0,0) },
{vec3(0.5, -0.5, 0.5),vec3(1,0,1),vec2(0.5,0)},
{vec3(0.5, 0.5, 0.5),vec3(0,0,1),vec2(0.5,0.5)},
{vec3(0.5, 0.5, -0.5),vec3(1,1,0), vec2(0.5,1)},
{vec3(0.5, -0.5, -0.5),vec3(0,1,1), vec2(0.5,0.5)},
{vec3(-0.5, -0.5, -0.5),vec3(1,1,1), vec2(1,0.5)},
{vec3(-0.5, 0.5, -0.5),vec3(0,0,0), vec2(1,1)}
};
vertex cube_core_vertices[] = {
{vec3(-0.5, 0.5, 0.5),  vec3(0.12941,0.14901,0.16470),vec2(0,0.5)},
{vec3(-0.5, -0.5, 0.5), vec3(0.12941,0.14901,0.16470),vec2(0,0) },
{vec3(0.5, -0.5, 0.5),  vec3(0.12941,0.14901,0.16470),vec2(0.5,0)},
{vec3(0.5, 0.5, 0.5),   vec3(0.12941,0.14901,0.16470),vec2(0.5,0.5)},
{vec3(0.5, 0.5, -0.5),  vec3(0.12941,0.14901,0.16470), vec2(0.5,1)},
{vec3(0.5, -0.5, -0.5), vec3(0.12941,0.14901,0.16470), vec2(0.5,0.5)},
{vec3(-0.5, -0.5, -0.5),vec3(0.12941,0.14901,0.16470), vec2(1,0.5)},
{vec3(-0.5, 0.5, -0.5), vec3(0.12941,0.14901,0.16470), vec2(1,1)}
};
void CreateCube()
{
	int vertices_Indeces[] = {
		//front
		0,
		1,
		2,

		0,
		2,
		3,
		//Right
		3,
		2,
		5,

		3,
		5,
		4,
		//Back
		4,
		5,
		6,

		4,
		6,
		7,
		//Left
		7,
		6,
		1,

		7,
		1,
		0,
		//Top
		7,
		0,
		3,

		7,
		3,
		4,
		//Bottom
		2,
		1,
		6,

		2,
		6,
		5
	};
	// create VAO
	//glGenVertexArrays(1, &VAO);
	//glBindVertexArray(VAO);

	// create VBO
	glGenBuffers(1, &Cube_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Cube_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_core_vertices), cube_core_vertices, GL_STATIC_DRAW);

	// Index Buffer
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices_Indeces), vertices_Indeces, GL_STATIC_DRAW);

	// shader
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(vertex), (char*)(2 * sizeof(vec3)));
	glEnableVertexAttribArray(2);

}

void BindCube()
{
	glBindBuffer(GL_ARRAY_BUFFER, Cube_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(vertex), (char*)(2 * sizeof(vec3)));
	glEnableVertexAttribArray(2);
}

#pragma endregion

void UseShader(uint ShaderId)
{
	glUseProgram(ShaderId);

	modelMatLoc = glGetUniformLocation(ShaderId, "modelMat");
	viewMatLoc = glGetUniformLocation(ShaderId, "viewMat");
	projMatLoc = glGetUniformLocation(ShaderId, "projMat");

	viewMat = lookAt(vec3(0, 0, 3), vec3(0, 0, 0), vec3(0, 1, 0));
	glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, value_ptr(viewMat));

	projMat = perspectiveFov(60.0f, 1.0f, 1.0f, 1.0f, 50.0f);
	glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));
}

void CompileShader()
{
	programId = InitShader("V_BasicShader.glsl", "F_BasicShader.glsl");
	phongProgramId = InitShader("V_phong.glsl", "F_phong.glsl");
	skyboxProgramId = InitShader("V_skybox.glsl", "F_skybox.glsl");
	reflectionProgramId = InitShader("V_Reflection.glsl", "F_Reflection.glsl");
	refractionProgramId = InitShader("V_Refrection.glsl", "F_Refrection.glsl");
}

void CreateCubeMap()
{
	string cub_maps[6] = {
		"skybox5\\rt.png",
		"skybox5\\lf.png",
		"skybox5\\up.png",
		"skybox5\\dn.png",
		"skybox5\\bk.png",
		"skybox5\\ft.png"
	};

	glGenTextures(1, &skyboxTextureId);
	sf::Image img;

	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureId);

	for (GLuint i = 0; i < 6; i++)
	{
		if (!img.loadFromFile(cub_maps[i]))
			cout << "error loading file";

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

int Init()
{
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		cout << "Error";
		getchar();
		return 1;
	}
	else
	{
		if (GLEW_VERSION_3_0)
			cout << "Driver support OpenGL 3.0\nDetails:\n";
	}
	cout << "\tUsing glew " << glewGetString(GLEW_VERSION) << endl;
	cout << "\tVendor: " << glGetString(GL_VENDOR) << endl;
	cout << "\tRenderer: " << glGetString(GL_RENDERER) << endl;
	cout << "\tVersion: " << glGetString(GL_VERSION) << endl;
	cout << "\tGLSL:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	cout << sphere_vertices_bulb.size();
	cout << sphere_vertices_fly.size();
	CompileShader();
	CreateSphere(Sphere_VBO_Bulb, vec3(1, 0.94901, 0), sphere_vertices_bulb,6);
	CreateSphere(Sphere_VBO_Fly, vec3(0.5, 0.2, 0.1), sphere_vertices_fly, 4);
	/*cout << sphere_vertices_bulb.size();
	cout << sphere_vertices_fly.size();*/
	CreateCube();
	CreateCubeMap();

	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);

	return 0;
}

float theta = 0;


void Update(float time)
{
	
	theta = time;
	//cout << theta << endl;
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glDepthMask(GL_FALSE);
	UseShader(skyboxProgramId);
	BindCube();
	ModelMat = glm::translate(glm::vec3(0, 0, 0)) *
		glm::rotate( 90 / 3.14f, glm::vec3(0, 1, 0)) *
		glm::scale(glm::vec3(10, 10, 10));
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(ModelMat));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
	//glDepthMask(GL_TRUE);
	


	//fly1-------------------

	UseShader(phongProgramId);
	BindSphere(Sphere_VBO_Fly);
	ModelMat = glm::translate(glm::vec3(1.5 * cos(theta), sin(1 * theta) + cos(5 * theta), sin(theta) /*5*sin( 5*theta)*/)) *
		glm::rotate(theta * 180 / 3.14f, glm::vec3(1, 0, 0)) *
		glm::scale(glm::vec3(0.01f, 0.01f, 0.01f));
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(ModelMat));
	glDrawArrays(GL_TRIANGLES, 0, sphere_vertices_fly.size());
	

	//fly2-------------------

	UseShader(phongProgramId);
	BindSphere(Sphere_VBO_Fly);
	ModelMat = glm::translate(glm::vec3(1 * cos(2*theta), sin(2.5 * theta) + cos(1 * theta), 2.2*sin(1.1*theta) /*5*sin( 5*theta)*/)) *
		glm::rotate(theta * 180 / 3.14f, glm::vec3(1, 0, 0)) *
		glm::scale(glm::vec3(0.01f, 0.01f, 0.01f));
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(ModelMat));
	glDrawArrays(GL_TRIANGLES, 0, sphere_vertices_fly.size());

	//fly3-------------------

	UseShader(phongProgramId);
	BindSphere(Sphere_VBO_Fly);
	ModelMat = glm::translate(glm::vec3(0.5 * cos(1 * theta), sin(4 * theta) + cos(1 * theta), 0.2 * sin(1.1 * theta) /*5*sin( 5*theta)*/)) *
		glm::rotate(theta * 180 / 3.14f, glm::vec3(1, 0, 0)) *
		glm::scale(glm::vec3(0.01f, 0.01f, 0.01f));
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(ModelMat));
	glDrawArrays(GL_TRIANGLES, 0, sphere_vertices_fly.size());

	//fly4-------------------

	UseShader(phongProgramId);
	BindSphere(Sphere_VBO_Fly);
	ModelMat = glm::translate(glm::vec3(0.4 * cos(2 * theta), sin(1.9 * theta) +2* cos(1.5 * theta), 0.2 * sin(1.9 * theta) /*5*sin( 5*theta)*/)) *
		glm::rotate(theta * 180 / 3.14f, glm::vec3(1, 0, 0)) *
		glm::scale(glm::vec3(0.01f, 0.01f, 0.01f));
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(ModelMat));
	glDrawArrays(GL_TRIANGLES, 0, sphere_vertices_fly.size());

	
	//light bulb-------------------
	UseShader(phongProgramId);
	BindSphere(Sphere_VBO_Bulb);
	
	ModelMat = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)) *
		glm::rotate( 180 / 3.14f, glm::vec3(0, 1, 0)) *
		glm::scale(glm::vec3(0.2f, 0.2f, 0.2f));
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(ModelMat));
	glDrawArrays(GL_TRIANGLES, 0, sphere_vertices_bulb.size());


	//light light pole-------------------
	UseShader(phongProgramId);
	glBindBuffer(GL_ARRAY_BUFFER, Cube_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(vertex), (char*)(2 * sizeof(vec3)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(3);
	ModelMat = glm::translate(glm::vec3(0.0f, -1.7f, 0.0f)) *
		glm::rotate(180 / 3.14f, glm::vec3(0, 1, 0)) *
		glm::scale(glm::vec3(0.08f, 3.0f, 0.08f));
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(ModelMat));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
	
}

int main()
{
	sf::ContextSettings context;
	context.depthBits = 24;
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!", sf::Style::Close, context);

	if (Init()) return 1;
	sf::Clock clock;

	DOUBLE time = 0;
	
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{

			case sf::Event::Closed:
				window.close();
				break;
			}
		}
		time = clock.getElapsedTime().asSeconds()/8;
		Update(time);
		Render();
		//cout << sphere_vertices.size() << endl;
		window.display();
	}
	return 0;
}