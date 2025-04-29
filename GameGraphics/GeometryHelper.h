#pragma once
#include "VertexData.h" 
#include "Geometry.h"

/* GeometryHelper
 * : 사각형(Rectangle) 등 간단한 기하 도형을 생성해주는 헬퍼 클래스
 */
class GeometryHelper
{
public:
	static void CreateRectangle(std::shared_ptr<Geometry<VertexColorData>> geometry, Color color);
	static void CreateRectangle(std::shared_ptr<Geometry<VertexTextureData>> geometry);
};