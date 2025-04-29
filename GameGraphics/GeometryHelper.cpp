#include "pch.h"
#include "GeometryHelper.h"

void GeometryHelper::CreateRectangle(std::shared_ptr<Geometry<VertexColorData>> geometry, Color color)
{
	/* Vertex Data */
	std::vector<VertexColorData> vertices;

	/*	사각형이니까 정점 4개, 사각형의 정점 구성도
	*	1  3
	*	0  2
	*/
	vertices.resize(4);

	vertices[0].position = Vec3(-0.5f, -0.5f, 0);
	vertices[0].color = color;

	vertices[1].position = Vec3(-0.5f, 0.5f, 0);
	vertices[1].color = color;

	vertices[2].position = Vec3(0.5f, -0.5f, 0);
	vertices[2].color = color;

	vertices[3].position = Vec3(0.5f, 0.5f, 0);
	vertices[3].color = color;

	geometry->SetVertices(vertices);

	/* Index Data */
	// 삼각형 1 : _vertices[0], _vertices[1], _vertices[2]
	// 삼각형 2 : _vertices[2], _vertices[1], _vertices[3]
	std::vector<uint32> indices = {
		0, 1, 2,
		2, 1, 3
	};

	geometry->SetIndices(indices);
}

void GeometryHelper::CreateRectangle(std::shared_ptr<Geometry<VertexTextureData>> geometry)
{
	/* Vertex Data */
	std::vector<VertexTextureData> vertices;

	/*	사각형이니까 정점 4개, 사각형의 정점 구성도
	*	1  3
	*	0  2
	*/
	vertices.resize(4);

	vertices[0].position = Vec3(-0.5f, -0.5f, 0);
	vertices[0].uv = Vec2(0.f, 1.f);

	vertices[1].position = Vec3(-0.5f, 0.5f, 0);
	vertices[1].uv = Vec2(0.f, 0.f);

	vertices[2].position = Vec3(0.5f, -0.5f, 0);
	vertices[2].uv = Vec2(1.f, 1.f);

	vertices[3].position = Vec3(0.5f, 0.5f, 0);
	vertices[3].uv = Vec2(1.f, 0.f);

	geometry->SetVertices(vertices);

	/* Index Data */
	// 삼각형 1 : _vertices[0], _vertices[1], _vertices[2]
	// 삼각형 2 : _vertices[2], _vertices[1], _vertices[3]
	std::vector<uint32> indices = {
		0, 1, 2,
		2, 1, 3
	};

	geometry->SetIndices(indices);
}