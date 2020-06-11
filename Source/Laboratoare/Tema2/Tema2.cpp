#include "Tema2.h"

#include <vector>
#include <string>
#include <iostream>
#include <time.h>

#include <Core/Engine.h>
#include <Laboratoare/Laborator4/Transform3D.h>

using namespace std;

Tema2::Tema2()
{
}

Tema2::~Tema2()
{
}

void Tema2::Init()
{
	camera = new Laborator::Camera();
	camera->Set(glm::vec3(1, 3.6, 5.6f), glm::vec3(1.5, 3.6, 0), glm::vec3(0, 1, 0));
	fov = 60;
	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
	srand(time(NULL));
	{
		Mesh* mesh = new Mesh("airPlane");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "myPlane.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("cylinder");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "cylinder.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("plane");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "plane50.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
	{
		Mesh* mesh = new Mesh("obstacle");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "obstacle.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
	{
		Mesh* mesh = new Mesh("fuel");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "fuel.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
	{
		Mesh* mesh = new Mesh("heart");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "heart.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Create a shader program for drawing face polygon with the color of the normal
	{
		Shader* shader = new Shader("ShaderLab7");
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	{
		Shader* shader = new Shader("ShaderSea");
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/VertexShaderSea.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/FragmentShaderSea.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	{
		Shader* shader = new Shader("ShaderNormal");
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/VertexShaderNormal.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/FragmentShaderNormal.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	//Light & material properties
	{
		lightPosition = glm::vec3(0, 1, 1);
		materialShininess = 30;
		materialKd = 0.5;
		materialKs = 0.5;
	}
	{
		anglePropeller = 0;
		planeOX = 0;
		planeOY = 3;
		timeRunning = 0;
		translateClouds[0] = 0;
		for (int i = 1; i < 6; i++) {
			translateClouds[i] = translateClouds[i - 1] + 5;
		}
		firstPerson = 0;
		fuelNr = 5;
		fuelX[0] = 10;

		fuelY[0] = 5.5;
		for (int i = 1; i < fuelNr; i++) {
			fuelX[i] = fuelX[i - 1] + 0.2;
			if (i > fuelNr/2) {
				fuelY[i] = fuelY[i - 1] + 0.05;
			}
			else {
				fuelY[i] = fuelY[i - 1]-0.05;
			}
		}
		lives = 3;
		nrObst = 8;
		obstacleX[0] = 7;
		obstacleY[0] = 4;
		for (int i = 1; i < nrObst; i++) {
			if (i == nrObst / 2) {
				obstacleX[i] = 12;
			}
			else {
				obstacleX[i] = obstacleX[i - 1] + 1;
			}
			obstacleY[i] = GenerateRandomFloat(3.2,7);

			
		}

		cameraUI = new Laborator::Camera();
		cameraUI->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 2, 0), glm::vec3(0, 1, 0));
		uiProjectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
		livesX[0] = -2.95;
		for (int i = 1; i < lives; i++) {
			livesX[i] = livesX[i - 1] + 0.3;
		}
		scaleFuelOX = 2;
		speedFuelConsumption = 0.05;
		planeAngle = 0;
	}
}

void Tema2::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0.98, 0.84, 0.65, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::Update(float deltaTimeSeconds)
{
	if (scaleFuelOX > 0 && lives > 0) {
		timeRunning += deltaTimeSeconds;
		if (timeRunning > 2) {
			if (planeAngle != 0) {
				if (planeAngle > 0) {
					planeAngle -= deltaTimeSeconds * 0.5;
				}
				else {
					planeAngle += deltaTimeSeconds * 0.5;
				}
			}
			if (planeAngle > -0.01 && planeAngle < 0.021) {
				planeAngle = 0;
			}
			
		}
		
		{//render airplane
			//render airPlane body
			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0, planeOY, 0));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1, 0));
				modelMatrix = glm::rotate(modelMatrix, planeAngle, glm::vec3(0, 0, 1));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2));
				RenderSimpleMesh(meshes["airPlane"], shaders["ShaderLab7"], camera, modelMatrix, glm::vec3(1.00, 0.0, 0.05));
			}
			//the proppeler
			{
				anglePropeller += deltaTimeSeconds * 5;
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0, planeOY, 0));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.32, 1, 0));

				modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.3, 0, 0));
				modelMatrix = glm::rotate(modelMatrix, planeAngle, glm::vec3(0, 0, 1));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.3, 0, 0));

				modelMatrix *= Transform3D::RotateOX(anglePropeller);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 1.5, 0.2));
				//modelMatrix = glm::rotate(modelMatrix, anglePropeller, glm::vec3(1, 0, 0));

				RenderSimpleMesh(meshes["box"], shaders["ShaderLab7"], camera, modelMatrix, glm::vec3(0.00, 0.0, 0.5));

			}
			{

				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0, planeOY, 0));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.32, 1, 0));

				modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.3, 0, 0));
				modelMatrix = glm::rotate(modelMatrix, planeAngle, glm::vec3(0, 0, 1));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.3, 0, 0));

				modelMatrix *= Transform3D::RotateOX(anglePropeller);
				modelMatrix *= Transform3D::RotateOX(pi / 2);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 1.5, 0.2));
				//modelMatrix = glm::rotate(modelMatrix, anglePropeller, glm::vec3(1, 0, 0));

				RenderSimpleMesh(meshes["box"], shaders["ShaderLab7"], camera, modelMatrix, glm::vec3(0.00, 0.0, 0.5));

			}

			//the pilot
		}
		//render sea
		{
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(1, 0.5f, 0));
			modelMatrix *= Transform3D::RotateOZ(timeRunning * 2);
			modelMatrix *= Transform3D::RotateOY(pi / 2);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(2.2f));


			RenderSimpleMesh(meshes["cylinder"], shaders["ShaderSea"], camera, modelMatrix, glm::vec3(0, 0.5, 1));
		}
		{//clouds
			for (int i = 0; i < 6; i++) {
				translateClouds[i] -= deltaTimeSeconds;
				if (translateClouds[i] < -10) {
					translateClouds[i] = 20;
				}
			}
			for (int i = 0; i < 6; i++) {
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.32 + translateClouds[i], 5, -2));
				//modelMatrix *= Transform3D::RotateOX(anglePropeller/3);
				modelMatrix *= Transform3D::RotateOZ(pi / 8);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.31));
				RenderSimpleMesh(meshes["box"], shaders["ShaderLab7"], camera, modelMatrix, glm::vec3(1, 1, 1));
				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.52 + translateClouds[i], 5.1, -2));
				//modelMatrix *= Transform3D::RotateOX(-anglePropeller/3);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2));
				RenderSimpleMesh(meshes["box"], shaders["ShaderLab7"], camera, modelMatrix, glm::vec3(1, 1, 1));
				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.72 + translateClouds[i], 5, -2));
				modelMatrix *= Transform3D::RotateOX(anglePropeller / 4);
				modelMatrix *= Transform3D::RotateOZ(pi / 5);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3));
				RenderSimpleMesh(meshes["box"], shaders["ShaderLab7"], camera, modelMatrix, glm::vec3(1, 1, 1));
				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.92 + translateClouds[i], 4.9, -2));
				//modelMatrix *= Transform3D::RotateOX(anglePropeller);
				modelMatrix *= Transform3D::RotateOZ(-pi / 8);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2));
				RenderSimpleMesh(meshes["box"], shaders["ShaderLab7"], camera, modelMatrix, glm::vec3(1, 1, 1));
			}

		}
		//TODO Gas Canisters, Obstacles, Collisions
		//Maybe lives and gas display


		//obstacle
		{
			for (int i = 0; i < nrObst; i++) {
				obstacleX[i] -= deltaTimeSeconds * 2;
				if (obstacleX[i] < -6) {
					obstacleX[i] = 10;
					obstacleY[i] = GenerateRandomFloat(3.2, 7);
					colidedObst[i] = false;
				}
			}
			for (int i = 0; i < nrObst; i++) {
				if (!colidedObst[i]) {
					glm::mat4 modelMatrix = glm::mat4(1);
					modelMatrix = glm::translate(modelMatrix, glm::vec3(obstacleX[i], obstacleY[i], 0));
					modelMatrix = glm::scale(modelMatrix, glm::vec3(1));
					RenderSimpleMesh(meshes["obstacle"], shaders["ShaderLab7"], camera, modelMatrix, glm::vec3(1.00, 0.0, 0.05));
				}
			}
		}

		//fuel
		{
			for (int i = 0; i < fuelNr; i++) {
				fuelY[i] += 0.02 * sin(timeRunning);
				fuelX[i] -= deltaTimeSeconds;
				if (fuelX[i] < -5) {
					fuelX[i] = 10;
					colidedFuel[i] = false;
				}
			}
			for (int i = 0; i < fuelNr; i++) {
				if (!colidedFuel[i]) {
					glm::mat4 modelMatrix = glm::mat4(1);
					modelMatrix = glm::translate(modelMatrix, glm::vec3(fuelX[i], fuelY[i], 0));
					modelMatrix = glm::rotate(modelMatrix, anglePropeller / 2 + i, glm::vec3(1, 1, 1));
					modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5));
					RenderSimpleMesh(meshes["fuel"], shaders["ShaderLab7"], camera, modelMatrix, glm::vec3(0.68, 0.85, 0.90));
				}
			}
		}

		//colissions check
		{//obstacles
			for (int i = 0; i < nrObst; i++) {
				if (!colidedObst[i] && CheckCollisions(0, planeOY + 1, obstacleX[i], obstacleY[i], 0.2)) {
					//printf("Collision detected obstacle %d\n", i);
					colidedObst[i] = true;
					lives--;
				}
			}
		}
		{//fuel
			for (int i = 0; i < fuelNr; i++) {
				if (!colidedFuel[i] && CheckCollisions(0, planeOY + 1, fuelX[i], fuelY[i], 0.15)) {
					//printf("Collision detected fuel %d\n", i);
					colidedFuel[i] = true;
					fuelCapacity += 0.25;
				}
			}
		}

		//user interface
		{//hearts
			for (int i = 0; i < lives; i++) {
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(livesX[i], 3.43, 0));
				modelMatrix *= Transform3D::RotateOY(-pi / 2);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05));
				RenderSimpleMesh(meshes["heart"], shaders["ShaderNormal"], cameraUI, modelMatrix, glm::vec3(0.76, 0.13, 0.28));
			}
			//fuel
				//black bar
			scaleFuelOX -= deltaTimeSeconds * speedFuelConsumption;
			scaleFuelOX += fuelCapacity;
			fuelCapacity = 0;
			if (scaleFuelOX > 2) {
				scaleFuelOX = 2;

			}
			if (scaleFuelOX < 0) {
				scaleFuelOX = 0;
			}
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(3, 3.85, 0));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.5, 0, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(scaleFuelOX, 0.2, 0.01));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5, 0, 0));

			RenderSimpleMesh(meshes["box"], shaders["ShaderNormal"], cameraUI, modelMatrix, glm::vec3(1));
			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(2.5, 3.85, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(2, 0.2, 0.01));
			RenderSimpleMesh(meshes["box"], shaders["ShaderNormal"], cameraUI, modelMatrix, glm::vec3(0));
			//fuel bar


		}
		planeOYDeath = planeOY;
	} 
	//death animation
	else {
	if (planeOYDeath < 0) {
		if (scaleFuelOX < 0) {
			printf("Out of Fuel!\n");
		}
		else {
			printf("You've run out of lives!\n");
		}
		printf("Game over!\n");
		this->Exit();
	}
	planeAngleDeath -= deltaTimeSeconds;
	planeOYDeath -= deltaTimeSeconds;
	if (planeAngleDeath < -pi / 2) {
		planeAngleDeath = -pi / 2;
	}

		{//render airplane
				//render airPlane body
			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0, planeOYDeath, 0));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1, 0));
				modelMatrix = glm::rotate(modelMatrix, planeAngleDeath, glm::vec3(0, 0, 1));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2));
				RenderSimpleMesh(meshes["airPlane"], shaders["ShaderLab7"], camera, modelMatrix, glm::vec3(1.00, 0.0, 0.05));
			}
			//the proppeler
			{
				//anglePropeller += deltaTimeSeconds * 5;
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0, planeOYDeath, 0));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.32, 1, 0));

				modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.3, 0, 0));
				modelMatrix = glm::rotate(modelMatrix, planeAngleDeath, glm::vec3(0, 0, 1));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.3, 0, 0));

				modelMatrix *= Transform3D::RotateOX(anglePropeller);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 1.5, 0.2));
				//modelMatrix = glm::rotate(modelMatrix, anglePropeller, glm::vec3(1, 0, 0));

				RenderSimpleMesh(meshes["box"], shaders["ShaderLab7"], camera, modelMatrix, glm::vec3(0.00, 0.0, 0.5));

			}
			{

				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0, planeOYDeath, 0));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.32, 1, 0));

				modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.3, 0, 0));
				modelMatrix = glm::rotate(modelMatrix, planeAngleDeath, glm::vec3(0, 0, 1));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.3, 0, 0));

				modelMatrix *= Transform3D::RotateOX(anglePropeller);
				modelMatrix *= Transform3D::RotateOX(pi / 2);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 1.5, 0.2));
				//modelMatrix = glm::rotate(modelMatrix, anglePropeller, glm::vec3(1, 0, 0));

				RenderSimpleMesh(meshes["box"], shaders["ShaderLab7"], camera, modelMatrix, glm::vec3(0.00, 0.0, 0.5));

			}

			//the pilot
		}
	//render sea
		{
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(1, 0.5f, 0));
			modelMatrix *= Transform3D::RotateOZ(timeRunning * 2);
			modelMatrix *= Transform3D::RotateOY(pi / 2);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(2.2f));


			RenderSimpleMesh(meshes["cylinder"], shaders["ShaderSea"], camera, modelMatrix, glm::vec3(0, 0.5, 1));
		}
		{//clouds
			for (int i = 0; i < 6; i++) {
				translateClouds[i] -= deltaTimeSeconds;
				if (translateClouds[i] < -10) {
					translateClouds[i] = 20;
				}
			}
			for (int i = 0; i < 6; i++) {
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.32 + translateClouds[i], 5, -2));
				//modelMatrix *= Transform3D::RotateOX(anglePropeller/3);
				modelMatrix *= Transform3D::RotateOZ(pi / 8);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.31));
				RenderSimpleMesh(meshes["box"], shaders["ShaderLab7"], camera, modelMatrix, glm::vec3(1, 1, 1));
				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.52 + translateClouds[i], 5.1, -2));
				//modelMatrix *= Transform3D::RotateOX(-anglePropeller/3);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2));
				RenderSimpleMesh(meshes["box"], shaders["ShaderLab7"], camera, modelMatrix, glm::vec3(1, 1, 1));
				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.72 + translateClouds[i], 5, -2));
				modelMatrix *= Transform3D::RotateOX(anglePropeller / 4);
				modelMatrix *= Transform3D::RotateOZ(pi / 5);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3));
				RenderSimpleMesh(meshes["box"], shaders["ShaderLab7"], camera, modelMatrix, glm::vec3(1, 1, 1));
				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.92 + translateClouds[i], 4.9, -2));
				//modelMatrix *= Transform3D::RotateOX(anglePropeller);
				modelMatrix *= Transform3D::RotateOZ(-pi / 8);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2));
				RenderSimpleMesh(meshes["box"], shaders["ShaderLab7"], camera, modelMatrix, glm::vec3(1, 1, 1));
			}

		}

	//user interface
		{//hearts
			for (int i = 0; i < lives; i++) {
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(livesX[i], 3.43, 0));
				modelMatrix *= Transform3D::RotateOY(-pi / 2);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05));
				RenderSimpleMesh(meshes["heart"], shaders["ShaderNormal"], cameraUI, modelMatrix, glm::vec3(0.76, 0.13, 0.28));
			}
			//fuel
				//black bar
			//scaleFuelOX -= deltaTimeSeconds * speedFuelConsumption;
			//scaleFuelOX += fuelCapacity;
			fuelCapacity = 0;
			if (scaleFuelOX > 2) {
				scaleFuelOX = 2;

			}
			if (scaleFuelOX < 0) {
				scaleFuelOX = 0;
			}
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(3, 3.85, 0));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.5, 0, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(scaleFuelOX, 0.2, 0.01));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5, 0, 0));

			RenderSimpleMesh(meshes["box"], shaders["ShaderNormal"], cameraUI, modelMatrix, glm::vec3(1));
			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(2.5, 3.85, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(2, 0.2, 0.01));
			RenderSimpleMesh(meshes["box"], shaders["ShaderNormal"], cameraUI, modelMatrix, glm::vec3(0));
			//fuel bar


		}
	}//end of death animation

	
}

void Tema2::FrameEnd()
{
	//DrawCoordinatSystem();
	//DrawCoordinatSystem(camera->GetViewMatrix(),projectionMatrix);
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, Laborator::Camera* usedCamera, const glm::mat4& modelMatrix, const glm::vec3& color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Set shader uniforms for light & material properties
	// TODO: Set light position uniform
	int m = glGetUniformLocation(shader->program, "light_position");
	glUniform3fv(m, 1, glm::value_ptr(lightPosition));
	// TODO: Set eye position (camera position) uniform
	
	//glm::vec3 eyePosition = camera->GetViewMatrix();
		

	m = glGetUniformLocation(shader->program, "eye_position");
	//glUniform3fv(m, 1, glm::value_ptr(eyePosition));

	// TODO: Set material property uniforms (shininess, kd, ks, object color) 
	m = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(m, materialShininess);
	m = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(m, materialKd);
	m = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(m, materialKs);
	m = glGetUniformLocation(shader->program, "object_color");
	glUniform3fv(m, 1, glm::value_ptr(color));

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = usedCamera->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	//glm::mat4 projectionMatrix = projectionMatrix;
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	
	glm::mat4 actualProjectionMatrix = projectionMatrix;
	if (usedCamera == cameraUI) {
		actualProjectionMatrix = uiProjectionMatrix;
	}
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(actualProjectionMatrix));

	m = glGetUniformLocation(shader->program, "time");
	glUniform1f(m, timeRunning);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
	float speed = 2;

	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float cameraSpeed = 2.0f;

		if (window->KeyHold(GLFW_KEY_W)) {
			// TODO : translate the camera forward
			camera->TranslateForward(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_A)) {
			// TODO : translate the camera to the left
			camera->TranslateRight(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_S)) {
			// TODO : translate the camera backwards
			camera->TranslateForward(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_D)) {
			// TODO : translate the camera to the right
			camera->TranslateRight(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_Q)) {
			// TODO : translate the camera down
			camera->TranslateUpword(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_E)) {
			// TODO : translate the camera up
			camera->TranslateUpword(cameraSpeed * deltaTime);
		}
	}
	if (window->KeyHold(GLFW_KEY_O)) {
		projectionMatrix = glm::ortho(-10, 10, -10, 10, 2, 200);

	}

	if (window->KeyHold(GLFW_KEY_P)) {
		projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 200.0f);
	}

	if (window->KeyHold(GLFW_KEY_L)) {
		fov += 1;
		projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 200.0f);

	}

	if (window->KeyHold(GLFW_KEY_M)) {
		fov -= 1;
		projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 200.0f);
	}

	if (window->KeyHold(GLFW_KEY_K)) {
		length += 1;
		projectionMatrix = glm::ortho(-10, 10, -10, 10, 2, 200);

	}

	if (window->KeyHold(GLFW_KEY_N)) {
		length -= 1;
		projectionMatrix = glm::ortho(-10, 10, -10, 10, 2, 200);
	}
	
}

void Tema2::OnKeyPress(int key, int mods)
{
	// add key press event
	if (window->KeyHold(GLFW_KEY_C)) {
		if (firstPerson == 0) {
			firstPerson = 1;
			camera->Set(glm::vec3(0.05, planeOY + 1.3, 0), glm::vec3(1, planeOY+1, 0), glm::vec3(0, 1, 0));
		}
		else {
			firstPerson = 0;
			camera->Set(glm::vec3(1, 3.6, 5.6f), glm::vec3(1.5, 3.6, 0), glm::vec3(0, 1, 0));
		}
	}
}

void Tema2::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT))
	{
		//if first person, translate also the camera
		planeAngle -= deltaY * 0.004;
		
		if (planeAngle > maxAngleValue) {
			planeAngle = maxAngleValue;
		}
		if (planeAngle < -maxAngleValue) {
			planeAngle = -maxAngleValue;
		}
		//printf("Plane Angle is %f and deltaY is %d\n", planeAngle, deltaY);
		planeOY -= deltaY * 0.007;
		if (planeOY > maxPlaneOY) {
			planeOY = maxPlaneOY;
		}
		if (planeOY < minPlaneOY) {
			planeOY = minPlaneOY;
		}
		if (firstPerson) {
			camera->TranslateUpword(-deltaY * 0.007);
		}

	}
	// add mouse move event
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;

		if (window->GetSpecialKeyState() == 0) {
			
			camera->RotateFirstPerson_OX(-deltaY * sensivityOX);
			camera->RotateFirstPerson_OY(-deltaX * sensivityOY);
			//camera->RotateFirstPerson_OZ(deltaY * sensivityOY);
			// TODO : rotate the camera in First-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
		}

		if (window->GetSpecialKeyState() && GLFW_MOD_CONTROL) {
			
			camera->RotateThirdPerson_OX(-deltaY * sensivityOX);
			camera->RotateThirdPerson_OY(-deltaX * sensivityOY);
			// TODO : rotate the camera in Third-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
		}

	}
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema2::OnWindowResize(int width, int height)
{
}

float Tema2::GenerateRandomFloat(float min, float max) {
	float scale = rand() / (float)RAND_MAX;
	return min + scale * (max - min);
}

bool Tema2::CheckCollisions(float planeX, float planeY, float objectX, float objectY, float objectRadius) {

	float actualCircleY = objectY;
	float actualCircleX = objectX;

	float heightValue = 0.08;
	float valueLeft = 0.15;
	float rLeft = planeX - valueLeft;
	float rRight = planeX + valueLeft;
	float rTop = planeY + heightValue;
	float rBottom = planeY - heightValue;

	float closestX = clamp(actualCircleX, rRight, rLeft);
	float closestY = clamp(actualCircleY, rTop, rBottom);

	float distanceX = actualCircleX - closestX;
	float distanceY = actualCircleY - closestY;

	float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
	
	return distanceSquared < (objectRadius * objectRadius);

}
float Tema2::clamp(float value, float max, float min) {
	if (value > max) {
		return max;
	}
	else if (value < min) {
		return min;
	}
	return value;
}