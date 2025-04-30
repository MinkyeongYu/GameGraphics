#include "pch.h"
#include "Transform.h"

Transform::Transform()
{

}

Transform::~Transform()
{

}

void Transform::Init()
{

}

void Transform::Update()
{

}

void Transform::UpdateTransform()
{
	// Create SRT
	Matrix scaleMatrix = Matrix::CreateScale(_localScale);
	Matrix rotationMatrix = Matrix::CreateRotationX(_localRotation.x);
	rotationMatrix *= Matrix::CreateRotationY(_localRotation.y);
	rotationMatrix *= Matrix::CreateRotationZ(_localRotation.z);
	Matrix translationMatrix = Matrix::CreateTranslation(_localPosition);

	_localMatrix = scaleMatrix * rotationMatrix * translationMatrix;
	
	if (HasParent())
	{
		// 부모가 있는 경우 -> 부모의 world 변환 행렬 필요
		_worldMatrix = _localMatrix * _parent->GetWorldMatrix();
	}
	else
	{
		// 부모가 없는 경우
		_worldMatrix = _localMatrix;
	}

	// 짐벌락 해결 위해 Quaternion 사용
	Quaternion quaternion;

	// world행렬 srt로 분해
	_worldMatrix.Decompose(_scale, quaternion, _position);

	_rotation = QuaternionToEulerAngles(quaternion);

	// vector 방향만, 위치 필요하면 transformcoord
	_right = Vec3::TransformNormal(Vec3::Right, _worldMatrix);
	_up = Vec3::TransformNormal(Vec3::Up, _worldMatrix);
	// library에 forward, backward 반대로 되어있음
	_look = Vec3::TransformNormal(Vec3::Backward, _worldMatrix);

	// children에서도 함수 재귀적 호출
	for (const std::shared_ptr<Transform>& child : _children)
	{
		child->UpdateTransform();
	}

}

void Transform::SetScale(const Vec3& worldScale)
{
	if (HasParent())
	{
		// 크기가 4x4x4인 오브젝트A가 크기 2x2x2인 오브젝트B의 자식으로 설정되면, A의 크기는 2x2x2로 설정됨
		Vec3 parentScale = _parent->GetScale();
		Vec3 scale = worldScale;
		scale.x /= parentScale.x;
		scale.y /= parentScale.y;
		scale.z /= parentScale.z;

		SetLocalScale(scale);
	}
	else
	{
		SetLocalScale(worldScale);
	}
}

void Transform::SetRotation(const Vec3& worldRotation)
{
	if (HasParent())
	{
		Matrix inverseMatrix = _parent->GetWorldMatrix().Invert();

		Vec3 rotation;
		rotation.TransformNormal(worldRotation, inverseMatrix);

		SetLocalRotation(rotation);
	}
	else
	{
		SetLocalRotation(worldRotation);
	}
}

void Transform::SetPosition(const Vec3& worldPosition)
{
	if (HasParent())
	{
		// 부모의 local->world 변환 행렬의 역행렬 = world 좌표에 있던 오브젝트를 부모 기준 좌표로 변환
		Matrix worldToParentLocalMatrix = _parent->GetWorldMatrix().Invert();
		
		Vec3 position;
		position.Transform(worldPosition, worldToParentLocalMatrix);

		SetLocalPosition(position);
	}
	else
	{
		SetLocalPosition(worldPosition);
	}
}

Vec3 Transform::QuaternionToEulerAngles(Quaternion q)
{
	Vec3 angles;

	// roll (x-axis rotation)
	double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
	double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
	angles.x = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = std::sqrt(1 + 2 * (q.w * q.y - q.x * q.z));
	double cosp = std::sqrt(1 - 2 * (q.w * q.y - q.x * q.z));
	angles.y = 2 * std::atan2(sinp, cosp) - 3.14159f / 2;

	// yaw (z-axis rotation)
	double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
	double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	angles.z = std::atan2(siny_cosp, cosy_cosp);

	return angles;
} 