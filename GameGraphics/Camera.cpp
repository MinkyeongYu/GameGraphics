#include "pch.h"
#include "Camera.h"

Matrix Camera::S_ViewMatrix = Matrix::Identity;
Matrix Camera::S_ProjectionMatrix = Matrix::Identity;

Camera::Camera()
	: Super(ComponentType::Camera)
{

}

Camera::~Camera()
{

}

void Camera::Update()
{
	UpdateMatrix();
}

void Camera::UpdateMatrix()
{
	Vec3 eyePosition = GetTransform()->GetPosition();
	Vec3 focusPosition = eyePosition + GetTransform()->GetLook();
	Vec3 upDirection = GetTransform()->GetUp();

	// == S_ViewMatrix = GetTransform()->GetWorldMatrix().Invert();
	S_ViewMatrix = ::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);

	if (_type == ProjectionType::Perspective)
	{
		S_ProjectionMatrix = ::XMMatrixPerspectiveFovLH(XM_PI/4.f, 800.f/600.f, 1.f, 100.f);
	}
	else
	{
		S_ProjectionMatrix = ::XMMatrixOrthographicLH(800.f, 600.f, 0.f, 1.f);
	}
}