#pragma once
#include "VertexData.h" 
#include "Geometry.h"

/* 도형 그리기(삼각형 정점 정보) 같은 간단한 작업 */
class GeometryHelper
{
public:
	static void CreateRectangle(std::shared_ptr<Geometry<VertexColorData>> geometry, Color color);
	static void CreateRectangle(std::shared_ptr<Geometry<VertexTextureData>> geometry);
};