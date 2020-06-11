#include "Tema1.h"

#include <vector>
#include <iostream>

#include <Core/Engine.h>
#include <Laboratoare/Laborator3/Transform2D.h>
#include <Laboratoare/Laborator3/Object2D.h>

using namespace std;

Tema1::Tema1()
{
}

Tema1::~Tema1()
{
}

void Tema1::Init()
{
	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	glm::vec3 corner = glm::vec3(0, 0, 0);
	float squareSide = 100;

	// compute coordinates of square center
	float cx = corner.x + squareSide / 2;
	float cy = corner.y + squareSide / 2;

	// initialize tx and ty (the translation steps)
	translateX = 0;
	translateY = 0;



	// initialize angularStep
	angularStep = 0;
	tx1 = 250;
	tx2 = 500;
	tx3 = 750;
	tx4 = 1000;
	tx5 = 1250;
	t[0] = 150;
	s[0] = 0.2;
	s[1] = 0.2;
	for (int i = 1; i < 5; i++) {
		t[i] = t[i - 1] + 260;
		s[i * 2] = rand() % 4 + 1;
		s[i * 2 + 1] = rand() % 4 + 1;
		int s1 = s[i * 2];
		int s2 = s[i * 2 + 1];
		if (s1 + s2 > 5.7) {
			while (s1 + s2 > 5.7 && s1 > 0) {
				s1 -= 0.5;
			}
			if (s1 < 0) {
				s1 += 0.5;
				while (s1 + s2 > 5.7 && s2 > 0) {
					s2 -= 0.5;
				}
			}
		}
		s[i * 2] = s1;
		s[i * 2 + 1] = s2;
	}


	up = 0;
	screenEdge = 1300;
	debug = 0;
	CMx = 64.13;
	CMy = 40;
	maxAngularStep = 0.5;
	points = 0;
	showPoints = 5;
	circleX = 179 / 2;
	circleY = 162.5 - 5;
	circleRadius = 40;
	gracePeriod = 0;

	birdy = 400;
	srand(NULL);

	Mesh* square1 = Object2D::CreateSquare("square1", corner, squareSide * 0.8, glm::vec3(0, 0, 1), true);
	AddMeshToList(square1);
	Mesh* triangle1 = Object2D::CreateTriangle("triangle1", corner, 30, glm::vec3(0, 0, 1), true);
	AddMeshToList(triangle1);
	Mesh* circle1 = Object2D::CreateCircle("circle1", 30, squareSide, glm::vec3(0, 0, 1), true);
	AddMeshToList(circle1);
	Mesh* square2 = Object2D::CreateSquare("square2", corner, squareSide, glm::vec3(0, 0, 1), true);
	AddMeshToList(square2);


	//debug TODO delte
	Mesh* circle2 = Object2D::CreateCircle("circle2", 5, squareSide, glm::vec3(1, 0, 1), true);
	AddMeshToList(circle2);
}

// 2D visualization matrix
glm::mat3 Tema1::VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
	float sx, sy, tx, ty;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	tx = viewSpace.x - sx * logicSpace.x;
	ty = viewSpace.y - sy * logicSpace.y;

	return glm::transpose(glm::mat3(
		sx, 0.0f, tx,
		0.0f, sy, ty,
		0.0f, 0.0f, 1.0f));
}

// uniform 2D visualization matrix (same scale factor on x and y axes)
glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
	float sx, sy, tx, ty, smin;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	if (sx < sy)
		smin = sx;
	else
		smin = sy;
	tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
	ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

	return glm::transpose(glm::mat3(
		smin, 0.0f, tx,
		0.0f, smin, ty,
		0.0f, 0.0f, 1.0f));
}

void Tema1::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
{
	glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	glEnable(GL_SCISSOR_TEST);
	glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
	GetSceneCamera()->Update();
}

void Tema1::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);

}

void Tema1::Update(float deltaTimeSeconds)
{
	// bird init
	//Checking smth
	gracePeriod += deltaTimeSeconds;
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(circleX, circleY + birdy / 2);

	//RenderMesh2D(meshes["circle2"], shaders["VertexColor"], modelMatrix);

	//done smth
	points += 11.9 * deltaTimeSeconds;
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Scale(0.5, 0.5);
	modelMatrix *= Transform2D::Translate(255, 310 + birdy);
	int c = -75;
	int d = 15;
	modelMatrix *= Transform2D::Translate(c, d);
	modelMatrix *= Transform2D::Rotate(angularStep);
	modelMatrix *= Transform2D::Translate(-c, -d);

	RenderMesh2D(meshes["triangle1"], shaders["VertexColor"], modelMatrix);
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Scale(0.5, 0.5);
	modelMatrix *= Transform2D::Translate(225, 325 + birdy);
	int a = -45.87;
	int b = 0;
	modelMatrix *= Transform2D::Translate(a, b);
	modelMatrix *= Transform2D::Rotate(angularStep);
	modelMatrix *= Transform2D::Translate(-a, -b);

	RenderMesh2D(meshes["circle1"], shaders["VertexColor"], modelMatrix);
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Scale(0.5, 0.5);
	modelMatrix *= Transform2D::Translate(115, 285 + birdy);
	modelMatrix *= Transform2D::Translate(CMx, CMy);
	modelMatrix *= Transform2D::Rotate(angularStep);
	modelMatrix *= Transform2D::Translate(-CMx, -CMy);

	RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);




	//wall stuff
	float value = 200 * deltaTimeSeconds;
	float birdValueY = 520 * deltaTimeSeconds;
	float minDistance[2];
	minDistance[0] = 1.5;
	minDistance[1] = 2.1;
	float minValue = 5.7;
	int j;
	if (gracePeriod > 2) {
		for (int i = 0; i < 5; i++) {
			t[i] -= value;
			if (t[i] + 100 < 0) {
				j = rand() % 2;
				minValue = 7.2 - minDistance[j];
				t[i] = screenEdge;
				s[i * 2] = rand() % 4 + 1;
				s[i * 2 + 1] = rand() % 4 + 1;
				int s1 = s[i * 2];
				int s2 = s[i * 2 + 1];
				if (s1 + s2 > minValue) {
					while (s1 + s2 > minValue&& s1 > 0) {
						s1 -= 0.5;
					}
					if (s1 < 0) {
						s1 += 0.5;
						while (s1 + s2 > minValue&& s2 > 0) {
							s2 -= 0.5;
						}
					}
				}
				s[i * 2] = s1;
				s[i * 2 + 1] = s2;
			}
		}

		if (up) {
			if (angularStep < maxAngularStep) {
				angularStep += 3 * deltaTimeSeconds;
			}
			birdy += 1.2 * birdValueY;
			if (timeUp > 0.35) {
				up = 0;
				timeUp = 0;
			}
			timeUp += deltaTimeSeconds;
		}
		else {
			if (angularStep > -maxAngularStep) {
				angularStep -= 2 * deltaTimeSeconds;
			}
			birdy -= birdValueY;
		}
	}





	for (int i = 0; i < 5; i++) {
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(t[i], 0);
		modelMatrix *= Transform2D::Scale(1, s[i * 2]);
		RenderMesh2D(meshes["square2"], shaders["VertexColor"], modelMatrix);

		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(t[i], 720);
		modelMatrix *= Transform2D::Scale(1, -s[i * 2 + 1]);
		RenderMesh2D(meshes["square2"], shaders["VertexColor"], modelMatrix);
	}

	if (points > showPoints) {
		printf("Score = %f\n", points);
		showPoints += 20;
	}

	//check colissions
	for (int i = 0; i < 5; i++) {
		if (CheckCollisions(i)) {
			if (gracePeriod > 2) {
				printf("Collision Detected! Game Over!\n");
				printf("Score = %f\n", points);

				this->Exit();
				//exit(0); //maybe show game over screen, maybe
			}
		}
	}



}

void Tema1::FrameEnd()
{

}

void Tema1::DrawScene(glm::mat3 visMatrix)
{
	modelMatrix = visMatrix * Transform2D::Translate(0, 0);
	RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

	modelMatrix = visMatrix * Transform2D::Translate(3, 0);
	RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

	modelMatrix = visMatrix * Transform2D::Translate(1.5, 1.5);
	RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

	modelMatrix = visMatrix * Transform2D::Translate(0, 3);
	RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

	modelMatrix = visMatrix * Transform2D::Translate(3, 3);
	RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);
}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{

}

void Tema1::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_SPACE) {
		up = 1;
		timeUp = 0;
	}
	if (key == GLFW_KEY_P) {
		if (debug == 0) {
			debug = 1;
			angularStep = 0;
		}
		else
			debug = 0;
	}
}

void Tema1::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

BOOLEAN Tema1::CheckCollisions(int colID) {
	int actualCircleY = circleY + birdy / 2;
	int actualCircleX = circleX;
	int height = s[2 * colID] * 100;
	int rLeft = t[colID];
	int rRight = t[colID] + 100;
	int rTop = height;
	int rBottom = 0;

	if (actualCircleY < 0) {
		return true;
	}

	float closestX = clamp(actualCircleX, rLeft, rRight);
	float closestY = clamp(actualCircleY, rTop, rBottom);

	float distanceX = actualCircleX - closestX;
	float distanceY = actualCircleY - closestY;

	float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
	if (distanceSquared < (circleRadius * circleRadius)) {
		return true;
	}


	height = s[2 * colID + 1] * 100;
	rLeft = t[colID];
	rRight = t[colID] + 100;
	rTop = 1000;
	rBottom = 720 - height;
	closestX = clamp(actualCircleX, rLeft, rRight);
	closestY = clamp(actualCircleY, rTop, rBottom);


	distanceX = actualCircleX - closestX;
	distanceY = actualCircleY - closestY;


	distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
	return distanceSquared < (circleRadius * circleRadius);
}

float Tema1::clamp(float value, float max, float min) {
	if (value > max) {
		return max;
	}
	else if (value < min) {
		return min;
	}
	return value;
}