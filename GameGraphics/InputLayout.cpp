#include "pch.h"
#include "InputLayout.h"

InputLayout::InputLayout(ComPtr<ID3D11Device> device)
	: _device(device)
{

}

InputLayout::~InputLayout()
{

}

void InputLayout::Create(const std::vector<D3D11_INPUT_ELEMENT_DESC>& descs, ComPtr<ID3DBlob> vertexShaderBlob)
{
	/* (배열 / 원소)로 원소 개수 구하기 */
	const int32 count = static_cast<int32>(descs.size());

	/* 입력 레이아웃 생성 */
	_device->CreateInputLayout(
		descs.data(), count, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), _inputLayout.GetAddressOf()
	);
}