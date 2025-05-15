#pragma once
#include "pch.h"
#include "Transform.h"

class MonoBehaviour;
class Camera;

/* GameObject
 * : 3D 게임 오브젝트를 구성하는 기본 클래스
 *   정점/인덱스 데이터, 버퍼, 셰이더, 텍스처, 렌더링 상태 등을 관리하고
 *   Update/Render 함수를 통해 매 프레임 업데이트 및 렌더링을 수행한다.
 */
class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
	GameObject(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext);
	~GameObject();

public:
	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FixedUpdate();

	std::shared_ptr<Component> GetFixedComponent(ComponentType type);
	std::shared_ptr<Transform> GetTransform();
	std::shared_ptr<Camera> GetCamera();

	std::shared_ptr<Transform> GetOrAddTransform();
	void AddComponent(std::shared_ptr<Component> component);

	void Render(std::shared_ptr<Pipeline> pipeline);

private:
	ComPtr<ID3D11Device> _device;

private:
	/* 1. Geometry
	: CPU 메모리 상에서 정점(Vertex)과 인덱스(Index) 데이터를 관리하는 객체 */
	std::shared_ptr<Geometry<VertexTextureData>> _geometry;

	/* 2. Vertex Buffer
	: 정점(Vertex) 데이터를 GPU VRAM에 업로드하고, IA(Input Assembler) 단계에서 사용 */
	std::shared_ptr<VertexBuffer> _vertexBuffer;

	/* 3. Index Buffer
	: 인덱스(Index) 데이터를 GPU VRAM에 업로드하고, IA(Input Assembler) 단계에서 사용 */
	std::shared_ptr<IndexBuffer> _indexBuffer;

	/* 4. Input Layout
	: 정점 버퍼(Vertex Buffer)의 데이터 구조를 GPU(셰이더)에게 알려주기 위한 객체 */
	std::shared_ptr<InputLayout> _inputLayout;

	/* 5. Vertex Shader
	: 정점 변환 단계 (월드/뷰/프로젝션 행렬 적용)에서 실행되는 셰이더 */
	std::shared_ptr<VertexShader> _vertexShader;

	/* 6. Constant Buffer
	: IA(Input Assembler) 이후 VS(Vertex Shader)/PS(Pixel Shader) 단계에서 공통 데이터를 전달하는 용도 */
	std::shared_ptr<ConstantBuffer<TransformData>> _constantBuffer;

	/* 7. Rasterizer State
	 * : 삼각형을 화면에 어떻게 그릴지(채우기, 컬링 등) 설정하는 래스터라이저 상태 객체
	 */
	std::shared_ptr<RasterizerState> _rasterizerState;

	/* 8. Pixel Shader
	: 픽셀 색상 계산 단계 (텍스처, 라이팅 적용)에서 실행되는 셰이더 */
	std::shared_ptr<PixelShader> _pixelShader;

	/* 9. Shader Resource View
	: 셰이더(Shader)가 GPU 리소스(텍스처, 버퍼 등)에 접근할 수 있도록 만들어주는 객체 */
	std::shared_ptr<Texture> _shaderResoureView;

	/* 10. Sampler State
	 * : 텍스처 좌표(UV) 샘플링 방식과 경계 처리 방식을 정의하는 샘플러 상태 객체
	 */
	std::shared_ptr<SamplerState> _samplerState;

	/* 11. Blend State
	 * : 픽셀 색상 혼합(알파 블렌딩 등) 방식을 정의하는 블렌딩 상태 객체
	 */
	std::shared_ptr<BlendState> _blendState;

private:
	/* Constant Buffer */
	TransformData _transformData;
	//ComPtr<ID3D11Buffer> _constantBuffer;

protected:
	// GameObject가 보유할 수 있는 고정 컴포넌트들을 저장하는 배열
	// 각 요소는 Transform, MeshRenderer, Camera 등으로 구성되며,
	// enum class ComponentType의 순서에 맞춰 인덱스로 접근할 수 있음.
	// FIXED_COMPOENENT_COUNT는 End를 제외한 고정 컴포넌트의 총 개수
	// std::shared_ptr<Component>를 사용하여 메모리 자동 관리
	std::array<std::shared_ptr<Component>, FIXED_COMPOENENT_COUNT> _components;
	std::vector<std::shared_ptr<Component>> _scripts;
};