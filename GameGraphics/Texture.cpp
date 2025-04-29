#include "pch.h"
#include "Texture.h"

Texture::Texture(ComPtr<ID3D11Device> device)
	:_device(device)
{
}

Texture::~Texture()
{
}

void Texture::CreateShaderResourceView(const std::wstring& path)
{
	// PNG 이미지 파일 로드
	DirectX::TexMetadata md;		// 텍스처 메타데이터 (크기, 포맷 등)
	DirectX::ScratchImage img;		// 이미지 데이터를 임시 저장할 객체

	// WIC(Windows Imaging Component)를 통해 PNG 파일을 읽고 img에 저장
	HRESULT hResult = ::LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, &md, img);
	CHECK(hResult);

	// 읽어온 이미지 데이터를 기반으로 Shader Resource View 생성
	hResult = ::CreateShaderResourceView(_device.Get(), img.GetImages(), img.GetImageCount(), md, _shaderResourceView.GetAddressOf());
	CHECK(hResult);
}