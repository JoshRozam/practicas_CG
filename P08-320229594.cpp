/*
Práctica 7: Iluminación 1
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture Dado;

Model Blackhawk_M;
Model Lampara;
Model Carro;			//Cuerpo del carro
Model Carro_Cofre;		//Cofre del carro
Model Carro_Parabrisas; //Parabrisas de carro
Model Llanta_TD;
Model Llanta_TI;
Model Llanta_FD;
Model Llanta_FI;
Model microondas;		//Modelo para Practica08: ejercicio03 

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;

// Para activar o desactivar lampara
unsigned int lucesPuntuales;

// Para activar o desactivar luces 
unsigned int lucesPuntuales2;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}

// Función para crear pirámide cuadrangular unitaria
void CrearOctaedro()
{
	unsigned int octaedro_indices[] = {
		0, 1, 2,

		//Izquierda
		3, 4, 5,

		6, 7, 8,

		9, 10, 11,

		12, 13, 14,

		15, 16, 17,

		18, 19, 20,

		21, 22, 23

	};

	GLfloat octaedro_vertices[] = {
		//Frente 
		//X		Y	Z		S			T			NX		NY		NZ
		0.0, 0.75, 0.0,		0.3281,		0.19921,	0.0,	0.0,	-1.0,	//0 Pico SUPERIOR
		0.5, 0.0, 0.5,		0.009,		0.00,		0.0,	0.0,	-1.0,	//1 Izquierda
		-0.5, 0.0, 0.5,		0.009,		0.4023,		0.0,	0.0,	-1.0,	//2 Derecha

		//Izquierda 
		0.0, 0.75, 0.0,		0.33203,	0.19921,	1.0,	0.0,	0.0,	//3 Pico SUPERIOR	Normal desde x positivo, así llega la luz a la cara
		-0.5, 0.0, -0.5,	0.33201,	0.6015,		1.0,	0.0,	0.0,	//4 Izquierda
		-0.5, 0.0, 0.5,		0.0,		0.4023,		1.0,	0.0,	0.0,	//5 Derecha

		//Derecha  
		0.0, 0.75, 0.0,		0.33203,	0.19921,	-1.0,	0.0,	0.0,	//6 Pico SUPERIOR
		0.5, 0.0, 0.5,		0.66406,	0.0,		-1.0,	0.0,	0.0,	//7 Izquierda
		0.5, 0.0, -0.5,		0.66406,	0.3984,		-1.0,	0.0,	0.0,	//8 Derecha 

		//Atrás 
		0.0, 0.75, 0.0,		0.33203,	0.19921,	0.0,	0.0,	1.0,	//9	Pico SUPERIOR 
		-0.5, 0.0, -0.5,	0.33201,	0.6015,		0.0,	0.0,	1.0,	//10 Izquierda 
		0.5, 0.0, -0.5,		0.66406,	0.3984,		0.0,	0.0,	1.0,	//11 Derecha 

		//Abajo Frente
		0.0, -0.75, 0.0,	0.6640,		0.8007,		0.0,	0.0,	-1.0,	//12 Pico BAJO
		0.5, 0.0, 0.5,		1.0,		1.0,		0.0,	0.0,	-1.0,	//13 Derecha
		-0.5, 0.0, 0.5,		1.0,		0.6015,		0.0,	0.0,	-1.0,	//14 Izquierda


		//Abajo Derecha  
		0.0, -0.75, 0.0,	0.6640,		0.8007,		-1.0,	0.0,	0.0,	//18 Pico BAJO
		0.5, 0.0, 0.5,		1.0,		0.6093,		-1.0,	0.0,	0.0,	//19 Derecha
		0.5, 0.0, -0.5,		0.6640,		0.3984,		-1.0,	0.0,	0.0,	//20 Izquierda
		//Abajo Izquierda
		0.0, -0.75, 0.0,	0.6640,		0.8007,		1.0,	0.0,	0.0,	//15 Pico BAJO
		-0.5, 0.0, -0.5,	0.3320,		0.6093,		1.0,	0.0,	0.0,	//16 Derecha 
		-0.5, 0.0, 0.5,		0.3320,		1.0,		1.0,	0.0,	0.0,	//17 Izquierda

		//Abajo Atrás 
		0.0, -0.75, 0.0,	0.6640,		0.8007,		0.0,	0.0,	1.0,	//21 Pico BAJO
		-0.5, 0.0, -0.5,	0.3320,		0.6093,		0.0,	0.0,	1.0,	//22 Derecha
		0.5,  0.0, -0.5,	0.6640,		0.4062,		0.0,	0.0,	1.0		//23 Izquierda

	};

	Mesh* octaedro = new Mesh();
	octaedro->CreateMesh(octaedro_vertices, octaedro_indices,
		24 * 8, 24); // 5 vértices * 8 floats = 40, 6 triángulos * 3 = 18
	meshList.push_back(octaedro);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();
	CrearOctaedro();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	Dado = Texture("Textures/dadoOchoOpt.png");
	Dado.LoadTextureA();

	microondas = Model();
	microondas.LoadModel("Models/microondas.dae");
	Lampara = Model();
	Lampara.LoadModel("Models/lampara.dae");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Carro = Model();
	Carro.LoadModel("Models/Cuerpo.dae");						// CUERPO DE COCHE
	Carro_Cofre = Model();
	Carro_Cofre.LoadModel("Models/Cofre.dae");					// COFRE
	Carro_Parabrisas = Model();
	Carro_Parabrisas.LoadModel("Models/Parabrisa.dae");			// PARABRISAS
	Llanta_FI = Model();
	Llanta_FI.LoadModel("Models/LlantaFrontalIzquierda.dae");	// RUEDA FRONTAL IZQUIERDA
	Llanta_FD = Model();
	Llanta_FD.LoadModel("Models/LlantaFrontalDerecha.dae");		// RUEDA FRONTAL DERECHA
	Llanta_TI = Model();
	Llanta_TI.LoadModel("Models/LlantaTraseraIzquierda.dae");	//RUEDA TRASERA IZQUIERDA
	Llanta_TD = Model();
	Llanta_TD.LoadModel("Models/LlantaTraseraDerecha.dae");		//RUEDA TRASERA DERECHA


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración luz auxiliar
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,
		0.0, 0.0,
		0.3f, -1.0f, 10.0f,
		0.0f, 0.0f, 1.0f);
	pointLightCount++;

	// Luz para el microondas
	pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,
		1.0, 1.0,
		-7.5f, -1.0f, 10.0f,
		0.0f, 0.0f, 1.0f);
	pointLightCount++;

	// Luz para la lámpara
	pointLights[2] = PointLight(1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f, 10.0f,
		0.0f, 0.0f, 1.0f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija del cofre
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	//luz fija problema 2 AUXILIAR
	spotLights[2] = SpotLight(1.0f, 1.0f, 0.0f,   // Color azul (R, G, B)
		1.0f, 2.0f,         // Intensidad ambiental y difusa
		0.0f, 0.0f, 0.0f,   // Posición inicial (se actualizará en el loop)
		1.0f, 0.0f, 0.0f,  // Dirección inicial (se actualizará en el loop)
		1.0f, 0.0f, 0.0f,   // Constantes de atenuación
		15.0f);             // Ángulo de corte (spotlight)
	spotLightCount++;

	//luz fija problema 2 REVERSA
	spotLights[3] = SpotLight(1.0f, 0.0f, 1.0f,   // Color azul (R, G, B)
		1.0f, 2.0f,         // Intensidad ambiental y difusa
		0.0f, 0.0f, 0.0f,   // Posición inicial (se actualizará en el loop)
		1.0f, 0.0f, 0.0f,  // Dirección inicial (se actualizará en el loop)
		1.0f, 0.0f, 0.0f,   // Constantes de atenuación
		15.0f);             // Ángulo de corte (spotlight)
	spotLightCount++;

	//luz fija problema 2, ADELANTE
	spotLights[4] = SpotLight(1.0f, 1.0f, 0.0f,   // Color azul (R, G, B)
		1.0f, 2.0f,         // Intensidad ambiental y difusa
		0.0f, 0.0f, 0.0f,   // Posición inicial (se actualizará en el loop)
		1.0f, 0.0f, 0.0f,  // Dirección inicial (se actualizará en el loop)
		1.0f, 0.0f, 0.0f,   // Constantes de atenuación
		15.0f);             // Ángulo de corte (spotlight)
	spotLightCount++;

	//se crean mas luces puntuales y spotlight 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		

		if (mainWindow.getPrendida())
		{
			pointLights[0] = pointLights[2];
			lucesPuntuales = pointLightCount - 2;
		}
		else
		{
			pointLights[0] = pointLights[1];
			lucesPuntuales = pointLightCount - 2;
		}

		if (mainWindow.getPrendidaDireccion())
		{	
			spotLights[2] = spotLights[4];
			lucesPuntuales2 = spotLightCount - 2;
		}
		else
		{
			
			spotLights[2] = spotLights[3];
			lucesPuntuales2 = spotLightCount - 2;
		}


		//2 amarilla
		//3 morada
		//4 auxiliar


		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, lucesPuntuales);
		shaderList[0].SetSpotLights(spotLights, lucesPuntuales2);

		

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::mat4 modelaux2(1.0);
		glm::mat4 modelaux3(1.0);
		glm::mat4 modelaux4(1.0);
		glm::mat4 modelaux5(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//Dado de Opengl
		//Ejercicio 1: Texturizar su cubo con la imagen dado_animales ya optimizada por ustedes //1.35f, 0.35f, -0.05f
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-30.35f, 4.35f, -0.05f));
		//model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Dado.UseTexture();
		meshList[4]->RenderMesh();

		//Microondas EJERCICIO 03
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-7.5f, -1.0f, 10.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		microondas.RenderModel();


		//Instancia del coche 
//------------*INICIA DIBUJO DE NUESTROS DEMÁS OBJETOS-------------------*
//Carro
		color = glm::vec3(0.75f, 0.75f, 1.0f); //modelo de carro de color negro

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), 1.18f, 0.0f));	// ADELANTE CON F. ATRÁS CON G
		glm::vec3 posicionTrasera = glm::vec3(7.5f + mainWindow.getmuevex(), 1.0f, 0.0f);	//Posicionamiento ajustable de x
		glm::vec3 posicionDelantera = glm::vec3(1.0f + mainWindow.getmuevex(), 1.0f, 0.0f);	//Posicionamiento ajustable de x

		glm::vec3 direccionLuzTrasera = glm::vec3(1.0f, 0.0f, 0.0f);						// Donde apunta la luz
		glm::vec3 direccionLuzDelantera = glm::vec3(-1.0f, 0.0f, 0.0f);

		spotLights[2].SetFlash(posicionDelantera, direccionLuzDelantera);					//Luz amarilla del frente
		spotLights[4].SetFlash(posicionDelantera, direccionLuzDelantera);					//Luz auxiliar
		spotLights[3].SetFlash(posicionTrasera, direccionLuzTrasera);						//Luz morada de atrás



		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		//modelaux = model;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro.RenderModel();//modificar por el modelo sin las 4 patas y sin cola
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		//En sesión se separara una parte del modelo del carro y se unirá por jeraquía al cuerpo
		modelaux = model;
		modelaux2 = model;
		modelaux3 = model;
		modelaux4 = model;
		modelaux5 = model;

		// Cofre
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.35f, 0.35f, -0.05f));
		
		//Rotación del cofre para que abra y cierra
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion()), glm::vec3(0.0f, 0.0f, 1.0f)); // ARRIBA CON H. ABAJO CON J
		//Luz del cofre
		glm::vec3 PosicionLuzCofre = glm::vec3(1.35f + mainWindow.getmuevex(), 0.0f + mainWindow.getarticulacion(), -0.05f);
		spotLights[1].SetPos(PosicionLuzCofre);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_Cofre.RenderModel();

		//Parabrisas
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.35f, 0.35f, -0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_Parabrisas.RenderModel();


		//llanta delantera derecha
		//model = modelaux2;
		model = modelaux2;
		model = glm::translate(model, glm::vec3(1.35f, -0.66f, -1.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_FD.RenderModel();


		//llanta delantera izquierda
		model = modelaux3;
		model = glm::translate(model, glm::vec3(1.35, -0.60f, 1.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_FI.RenderModel();


		//llanta trasera derecha
		model = modelaux4;
		model = glm::translate(model, glm::vec3(4.05f, -0.68f, -1.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_TD.RenderModel();

		//llanta trasera izquierda
		model = modelaux5;
		model = glm::translate(model, glm::vec3(4.05f, -0.64f, 1.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_TI.RenderModel();


		//Instancia del helicoptero 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex2(), 5.0f, 6.0)); //getmuevex2 Se mueve adelante con L y atrás con K
		//glm::vec3 posicion = glm::vec3(0.0f + mainWindow.getmuevex2(), 5.0f, 6.0f);	//Posicionamiento ajustable de x

		//glm::vec3 posicionLuz = posicion + glm::vec3(0.0f, 0.5f, 0.0f);		// Posición
		//glm::vec3 direccionLuz = glm::vec3(0.0f, -1.0f, 0.0f);				// Donde apunta la luz
		//spotLights[2].SetFlash(posicionLuz, direccionLuz);					// Reposicionamiento de la luz

		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();

		//Lampara texturizada
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 10.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		//Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
