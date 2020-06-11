#pragma once
#include <Component/SimpleScene.h>
#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>
#include <Laboratoare/Laborator5/LabCamera.h>

class Tema2 : public SimpleScene
{
public:
	Tema2();
	~Tema2();

	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void RenderSimpleMesh(Mesh* mesh, Shader* shader, Laborator::Camera* usedCamera, const glm::mat4& modelMatrix, const glm::vec3& color = glm::vec3(1));

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;
	float GenerateRandomFloat(float min, float max);
	bool CheckCollisions(float planeX, float planeY, float objectX, float objectY, float objectRadius);
	float clamp(float value, float max, float min);

	glm::vec3 lightPosition;
	unsigned int materialShininess;
	float materialKd;
	float materialKs;
	float anglePropeller;
	float pi = 3.14159265;
	float planeOX;
	float planeOY;

	Laborator::Camera* camera;
	glm::mat4 projectionMatrix;
	glm::mat4 uiProjectionMatrix;
	float fov;
	float length;
	float timeRunning;
	float translateClouds[6];
	int firstPerson;
	int fuelNr;
	float fuelX[100];
	float fuelY[100];
	bool colidedFuel[100];
	int lives;
	float obstacleX[50];
	float obstacleY[50];
	float nrObst;
	float fuelCapacity;
	bool colidedObst[50];
	Laborator::Camera* cameraUI;
	float livesX[3];
	float scaleFuelOX;
	float speedFuelConsumption;
	float planeAngle;
	float planeAngleDeath;
	float planeOYDeath;
	float maxAngleValue = 0.5;
	float maxPlaneOY = 6;
	float minPlaneOY = 2.3;
};
