#pragma once

/* 정점에 대한 정의 */

/* VertexTextureData
 * : 정점의 위치(Position)와 텍스처 좌표(UV)를 포함하는 데이터 구조체
 */
struct VertexTextureData
{
	Vec3 position	= { 0, 0, 0 };		// offset 0, size = float count 3 * sizeof(float) = 12Byte
	Vec2 uv			= { 0, 0 };			// offset 12, size = float count 2 * sizeof(float) = 8Byte

	static std::vector<D3D11_INPUT_ELEMENT_DESC> descs;
};
/* VertexColorData
 * : 정점의 위치(Position)와 색상(Color)를 포함하는 데이터 구조체
 */
struct VertexColorData
{
	Vec3 position	= {0, 0, 0};			// offset 0, size = float count 3 * sizeof(float) = 12Byte
	Color color		= { 0, 0, 0, 0 };		// offset 12, size = float count 4 * sizeof(float) = 16Byte

	static std::vector<D3D11_INPUT_ELEMENT_DESC> descs;
};