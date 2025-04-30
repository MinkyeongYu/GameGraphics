#pragma once
#include "Component.h"

class Transform : public Component
{
public:
	Transform();
	~Transform();

public:
	virtual void Init() override;
	virtual void Update() override;

	void UpdateTransform();

	// Local
	Vec3 GetLocalScale() { return _localScale; }
	void SetLocalScale(const Vec3& localScale) { _localScale = localScale; UpdateTransform(); }
	
	Vec3 GetLocalRotation() { return _localRotation; }
	void SetLocalRotation(const Vec3& localRotation) { _localRotation = localRotation; UpdateTransform();}

	Vec3 GetLocalPosition() { return _localPosition; }
	void SetLocalPosition(const Vec3& localPosition) { _localPosition = localPosition; UpdateTransform(); }

	// World
	Vec3 GetScale() { return _scale; }
	void SetScale(const Vec3& worldScale);

	Vec3 GetRotation() { return _rotation; }
	void SetRotation(const Vec3& worldRotation);

	Vec3 GetPosition() { return _position; }
	void SetPosition(const Vec3& worldPosition);

	// object의 world 변환 행렬
	Matrix GetWorldMatrix() { return _worldMatrix; }

	Vec3 QuaternionToEulerAngles(Quaternion q);

public:
	bool HasParent() { return _parent != nullptr; }
	std::shared_ptr<Transform> GetParent() { return _parent; }
	void SetParent(std::shared_ptr<Transform> parent) { _parent = parent; }

	const std::vector<std::shared_ptr<Transform>> GetChildren() { return _children; }
	void AddChild(std::shared_ptr<Transform> child) { _children.push_back(child); }

private:
	/* SRT */
	Vec3 _localPosition = { 0.f, 0.f, 0.f };
	Vec3 _localRotation = { 0.f, 0.f, 0.f };
	Vec3 _localScale = { 1.f, 1.f, 1.f };
	Matrix _localMatrix = Matrix::Identity;
	Matrix _worldMatrix = Matrix::Identity;

	// Cache
	Vec3 _scale;
	Vec3 _rotation;
	Vec3 _position;

	Vec3 _right;
	Vec3 _up;
	Vec3 _look;

private:
	std::shared_ptr<Transform> _parent;
	std::vector<std::shared_ptr<Transform>> _children;
};