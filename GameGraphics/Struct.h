// struct 모음
#pragma once

/* 정점에 대한 정의 */
struct Vertex
{
	Vec3 position;		// offset 0, size = float count 3 * sizeof(float) = 12Byte
	/*Color color;*/    // offset 12, size = float count 4 * sizeof(float) = 16Byte
	Vec2 uv;			// offset 12, size = float count 2 * sizeof(float) = 8Byte
};

/* Constant Buffer, 16Byte 정렬 필요 */
struct TransformData
{
	Matrix worldMatrix = Matrix::Identity;			// offset 0, size = 16 * sizeof(float) = 64Byte;
	Matrix viewMatrix = Matrix::Identity;			// offset 64, size = 16 * sizeof(float) = 64Byte;
	Matrix projectrionMatrix = Matrix::Identity;	// offset 128, size = 16 * sizeof(float) = 64Byte
};