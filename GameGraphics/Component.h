#pragma once

class GameObject;
class Transform;

enum class ComponentType : uint8
{
	Transform,
	MeshRenderer,
	Camera,
	Animator,
	// ...
	Script,
	End,
};

enum
{
	FIXED_COMPOENENT_COUNT = static_cast<uint8>(ComponentType::End) - 1,
};

class Component
{
public:
	Component(ComponentType type);
	~Component();

	virtual void Awake() {}
	virtual void Start() {}
	virtual void Update() {}
	virtual void LateUpdate() {}
	virtual void FixedUpdate() {}
	
public:
	std::shared_ptr<GameObject> GetGameObject();
	std::shared_ptr<Transform> GetTransform();
	ComponentType GetType() { return _type; }

private:
	friend class GameObject;
	void SetGameObject(std::shared_ptr<GameObject> gameObject) { _gameObject = gameObject; }

private:
	ComponentType _type;
	std::weak_ptr<GameObject> _gameObject;
};