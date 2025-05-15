#pragma once
#include "Component.h"

enum class ProjectionType
{
	// 원근 투영
	Perspective,
	// 직교 투영
	Orthographic,
};

class Camera : public Component
{
	using Super = Component;

public:
	Camera();
	~Camera();

	virtual void Update() override;

	void SetProjectionType(ProjectionType type) { _type = type; }
	ProjectionType GetProjectionType() { return _type; }
	
	void UpdateMatrix();

private:
	ProjectionType _type = ProjectionType::Orthographic;

public:
	static Matrix S_ViewMatrix;
	static Matrix S_ProjectionMatrix;
};