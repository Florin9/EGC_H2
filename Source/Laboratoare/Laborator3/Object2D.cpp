#include "Object2D.h"

#include <Core/Engine.h>

Mesh* Object2D::CreateSquare(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, length, 0), color),
		VertexFormat(corner + glm::vec3(0, length, 0), color)
	};

	Mesh* square = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2, 3 };
	
	if (!fill) {
		square->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		// draw 2 triangles. Add the remaining 2 indices
		indices.push_back(0);
		indices.push_back(2);
	}

	square->InitFromData(vertices, indices);
	return square;
}

Mesh* Object2D::CreateTriangle(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(0,length, 0), color),
		VertexFormat(corner + glm::vec3(length/1.5, length/2, 0), color),
	};

	Mesh* triangle = new Mesh(name);
	std::vector<unsigned short> indices = { 2,1,0};

	if (!fill) {
		triangle->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		// draw 2 triangles. Add the remaining 2 indices
		indices.push_back(0);
		//indices.push_back(2);
	}

	triangle->InitFromData(vertices, indices);
	return triangle;
}

Mesh* Object2D::CreateCircle(std::string name, int radius, float length, glm::vec3 color, bool fill)
{
	//glm::vec3 corner = center;
	int r = radius;

	std::vector<VertexFormat> vertices =
	{
	};
	std::vector<unsigned short> indices = {};
	int j = 0;
	int lineAmount = 50;
	GLfloat twicePi = 2.0f * 3.14;
	int x = 0;
	int y = 0;

	for (int i = 0; i <= lineAmount; i++) {
		int a = r * cos(i * twicePi / lineAmount);
		int b = r * sin(i * twicePi / lineAmount);
		vertices.push_back(VertexFormat(glm::vec3(x+a, y+b, 0), color));
		indices.push_back(i);
	}

	Mesh* circle = new Mesh(name);
	

	if (!fill) {
		circle->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		circle->SetDrawMode(GL_TRIANGLE_FAN);
		// draw 2 triangles. Add the remaining 2 indices
		//indices.push_back(0);
		//indices.push_back(2);
	}

	circle->InitFromData(vertices, indices);
	return circle;
}