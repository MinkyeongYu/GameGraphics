#pragma once

class GameObject;

class Component
{
public:
	Component();
	~Component();

	virtual void Init() abstract;
	virtual void Update() abstract;
	
	std::shared_ptr<GameObject> GetGameObject() { return _owner; }

private:
	std::shared_ptr<GameObject> _owner;
};