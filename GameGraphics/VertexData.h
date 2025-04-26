#pragma once

/* 정점에 대한 정의 */
// UV, Texture
struct VertexTextureData
{
	Vec3 position	= { 0, 0, 0 };		// offset 0, size = float count 3 * sizeof(float) = 12Byte
	Vec2 uv			= { 0, 0 };			// offset 12, size = float count 2 * sizeof(float) = 8Byte

	static std::vector<D3D11_INPUT_ELEMENT_DESC> descs;
};
// Color
struct VertexColorData
{
	Vec3 position	= {0, 0, 0};			// offset 0, size = float count 3 * sizeof(float) = 12Byte
	Color color		= { 0, 0, 0, 0 };		// offset 12, size = float count 4 * sizeof(float) = 16Byte

	static std::vector<D3D11_INPUT_ELEMENT_DESC> descs;
};