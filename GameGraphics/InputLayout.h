#pragma once

/* InputLayout
 * : 정점(Vertex) 데이터의 구조를 GPU 셰이더에게 알려주기 위한 레이아웃 클래스
 */
class InputLayout
{
public:
	InputLayout(ComPtr<ID3D11Device> device);
	~InputLayout();

public:
	ComPtr<ID3D11InputLayout> GetComPtr() { return _inputLayout; }

	void Create(const std::vector<D3D11_INPUT_ELEMENT_DESC>& descs, ComPtr<ID3DBlob> blob);

private:
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11InputLayout> _inputLayout;
};