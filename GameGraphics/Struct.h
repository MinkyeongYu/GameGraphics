// struct 모음
#pragma once

/* Constant Buffer, 16Byte 정렬 필요 */
struct TransformData
{
	Matrix worldMatrix = Matrix::Identity;			// offset 0, size = 16 * sizeof(float) = 64Byte;
	Matrix viewMatrix = Matrix::Identity;			// offset 64, size = 16 * sizeof(float) = 64Byte;
	Matrix projectrionMatrix = Matrix::Identity;	// offset 128, size = 16 * sizeof(float) = 64Byte
};