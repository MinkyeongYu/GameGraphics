#include "pch.h"
#include "GameObject.h"
#include "MonoBehaviour.h"

GameObject::GameObject(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext)
	:_device(device)
{
	// [1] Create Geometry (정점/인덱스 데이터 생성)
	_geometry = std::make_shared<Geometry<VertexTextureData>>();
	GeometryHelper::CreateRectangle(_geometry);

	// [2] Create Vertex Buffer
	_vertexBuffer = std::make_shared<VertexBuffer>(device);
	_vertexBuffer->Create(_geometry->GetVertices());

	// [3] Create Index Buffer
	_indexBuffer = std::make_shared<IndexBuffer>(device);
	_indexBuffer->Create(_geometry->GetIndices());

	// [4] Create Vertex Shader
	_vertexShader = std::make_shared<VertexShader>(device);
	_vertexShader->Create(L"DefaultVertexShader.hlsl", "VS_main", "vs_5_0");

	// [5] Create Input Layout
	_inputLayout = std::make_shared<InputLayout>(device);
	_inputLayout->Create(VertexTextureData::descs, _vertexShader->GetBlob());

	// [6] Create Constant Buffer
	_constantBuffer = std::make_shared<ConstantBuffer<TransformData>>(device, deviceContext);
	_constantBuffer->CreateConstantBuffer();

	// [7] Create Rasterizer State
	_rasterizerState = std::make_shared<RasterizerState>(device);
	_rasterizerState->CreateRasterizerState();

	// [8] Create Pixel Shader
	_pixelShader = std::make_shared<PixelShader>(device);
	_pixelShader->Create(L"DefaultVertexShader.hlsl", "PS", "ps_5_0");

	// [9] Create Shader Resource View (Texture)
	_shaderResoureView = std::make_shared<Texture>(device);
	_shaderResoureView->CreateShaderResourceView(L"hamster_latte.png");

	// [10] Create Sampler State
	_samplerState = std::make_shared<SamplerState>(device);
	_samplerState->CreateSamplerState();

	// [11] Create Blend State
	_blendState = std::make_shared<BlendState>(device);
	_blendState->CreateBlendState();
}

GameObject::~GameObject()
{

}

void GameObject::Awake()
{
	for (std::shared_ptr<Component>& component : _components)
	{
		if(component) component->Awake();
	}

	for (std::shared_ptr<Component>& script : _scripts)
	{
		if (script) script->Awake();
	}
}

void GameObject::Start()
{
	for (std::shared_ptr<Component>& component : _components)
	{
		if (component) component->Start();
	}

	for (std::shared_ptr<Component>& script : _scripts)
	{
		if (script) script->Start();
	}
}

void GameObject::Update()
{
	for (std::shared_ptr<Component>& component : _components)
	{
		if (component) component->Update();
	}

	for (std::shared_ptr<Component>& script : _scripts)
	{
		if (script) script->Update();
	}

	// Create WorldMatrix
	_transformData.worldMatrix = GetOrAddTransform()->GetWorldMatrix();

	// 버퍼에 데이터 복사
	_constantBuffer->CopyData(_transformData);
}

void GameObject::LateUpdate()
{
	for (std::shared_ptr<Component>& component : _components)
	{
		if (component) component->LateUpdate();
	}

	for (std::shared_ptr<Component>& script : _scripts)
	{
		if (script) script->LateUpdate();
	}
}

void GameObject::FixedUpdate()
{
	for (std::shared_ptr<Component>& component : _components)
	{
		if (component) component->FixedUpdate();
	}

	for (std::shared_ptr<Component>& script : _scripts)
	{
		if (script) script->FixedUpdate();
	}
}

std::shared_ptr<Component> GameObject::GetFixedComponent(ComponentType type)
{
	uint8 index = static_cast<uint8>(type);
	assert(index < FIXED_COMPOENENT_COUNT);

	return _components[index];
}

std::shared_ptr<Transform> GameObject::GetTransform()
{
	std::shared_ptr<Component> component = GetFixedComponent(ComponentType::Transform);

	return std::static_pointer_cast<Transform>(component);
}

std::shared_ptr<Camera> GameObject::GetCamera()
{
	return std::shared_ptr<Camera>();
}

std::shared_ptr<Transform> GameObject::GetOrAddTransform()
{
	if (GetTransform() == nullptr)
	{
		std::shared_ptr<Transform> transform = std::make_shared<Transform>();
		AddComponent(transform);
	}

	return GetTransform();
}

void GameObject::AddComponent(std::shared_ptr<Component> component)
{
	component->SetGameObject(shared_from_this());

	uint8 index = static_cast<uint8>(component->GetType());
	if (index < FIXED_COMPOENENT_COUNT)
	{
		_components[index] = component;
	}
	else
	{
		_scripts.push_back(std::dynamic_pointer_cast<MonoBehaviour>(component));
	}
}

void GameObject::Render(std::shared_ptr<Pipeline> pipeline)
{
	/* IA - VS - RS - PS - OM */
	{
		PipelineInfo info;
		info._inputLayout = _inputLayout;
		info._vertexShader = _vertexShader;
		info._pixelShader = _pixelShader;
		info._rasterizerState = _rasterizerState;
		info._blendState = _blendState;

		/*
		- IA : GPU에게 정점 데이터의 구조 전달
			 : _graphics->GetDeviceContext()->IASetInputLayout(_inputLayout->GetComPtr().Get());
		- IA : 각 정점을 어떻게 이어줄지 전달, 삼각형으로 이어주도록 설정
			 : _graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		- VS, PS : Vertex, Pixel Shader 설정
			 : _graphics->GetDeviceContext()->VSSetShader(_vertexShader->GetComPtr().Get(), nullptr, 0);
			  _graphics->GetDeviceContext()->PSSetShader(_pixelShader->GetComPtr().Get(), nullptr, 0);
		- RS : 정점 → 픽셀로 삼각형 그리기
			 : _graphics->GetDeviceContext()->RSSetState(_rasterizerState->GetComPtr().Get());
		- OM : Blend State
			 : _graphics->GetDeviceContext()->OMSetBlendState(_blendState->GetComPtr().Get(), _blendState->GetBlendFactor(), _blendState->GetSampleMask());
		*/
		pipeline->UpdatePipeline(info);

		// IA (Input Assembler) : 정점의 정보 전달
		/* GPU에게 정점 버퍼의 크기와 위치(stride, offset) 전달, vertices 사용함을 GPU에 알려줌 */
		pipeline->SetVertexBuffer(_vertexBuffer);
		/* 32-bit(4Byte) uint 인덱스 버퍼 GPU에 바인딩 */
		pipeline->SetIndexBuffer(_indexBuffer);

		// VS (Vertex Shader) : 정점의 위치/색상 등 가공
		pipeline->SetConstantBuffer(0, SS_VertexShader, _constantBuffer);

		// PS (Pixel Shader) : 픽셀 단위 색상 처리
		pipeline->SetShaderResources(0, SS_PixelShader, _shaderResoureView);
		pipeline->SetSamplerState(0, SS_PixelShader, _samplerState);

		// OM (Output Merger) : 최종 픽셀을 렌더 타겟에 출력
		pipeline->DrawIndexed(_geometry->GetIndexCount(), 0, 0);
	}
}