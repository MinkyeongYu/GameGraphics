#include "pch.h"
#include "Pipeline.h"

Pipeline::Pipeline(ComPtr<ID3D11DeviceContext> deviceContext)
	:_deviceContext(deviceContext)
{

}

Pipeline::~Pipeline()
{

}

void Pipeline::UpdatePipeline(PipelineInfo info)
{
	/* IA - VS - RS - PS - OM */
	{
		// IA (Input Assembler) : 정점의 정보 전달
		/* GPU에게 정점 데이터의 구조 전달 */
		_deviceContext->IASetInputLayout(info._inputLayout->GetComPtr().Get());
		/* 각 정점을 어떻게 이어줄지 전달, 삼각형으로 이어주도록 설정 */
		_deviceContext->IASetPrimitiveTopology(info.topology);

		// VS (Vertex Shader) : 정점의 위치/색상 등 가공
		if (info._vertexShader)
		{
			_deviceContext->VSSetShader(info._vertexShader->GetComPtr().Get(), nullptr, 0);
		}

		// RS (Rasterizer) : 정점 → 픽셀로 삼각형 그리기
		/* Draw()호출 시 내부적으로 GPU가 자동으로 Rasterizer 실행함 */
		if (info._rasterizerState)
		{
			_deviceContext->RSSetState(info._rasterizerState->GetComPtr().Get());
		}

		// PS (Pixel Shader) : 픽셀 단위 색상 처리
		if (info._pixelShader)
		{
			_deviceContext->PSSetShader(info._pixelShader->GetComPtr().Get(), nullptr, 0);
		}

		// OM (Output Merger) : 최종 픽셀을 렌더 타겟에 출력
		if (info._blendState)
		{
			_deviceContext->OMSetBlendState(info._blendState->GetComPtr().Get(), info._blendState->GetBlendFactor(), info._blendState->GetSampleMask());
		}
	}
}

void Pipeline::SetVertexBuffer(std::shared_ptr<VertexBuffer> buffer)
{
	uint32 stride = buffer->GetStride();
	uint32 offset = buffer->GetOffset();
	/* GPU에게 정점 버퍼의 크기와 위치(stride, offset) 전달, vertices 사용함을 GPU에 알려줌 */
	_deviceContext->IASetVertexBuffers(0, 1, buffer->GetComPtr().GetAddressOf(), &stride, &offset);
}

void Pipeline::SetIndexBuffer(std::shared_ptr<IndexBuffer> buffer)
{
	/* 32-bit(4Byte) uint 인덱스 버퍼 GPU에 바인딩 */
	_deviceContext->IASetIndexBuffer(buffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
}

void Pipeline::SetShaderResources(uint32 slot, uint32 scope, std::shared_ptr<Texture> shaderResouceView)
{
	if (scope & SS_VertexShader)
	{
		_deviceContext->VSSetShaderResources(slot, 1, shaderResouceView->GetComPtr().GetAddressOf());
	}
	if (scope & SS_PixelShader)
	{
		_deviceContext->PSSetShaderResources(slot, 1, shaderResouceView->GetComPtr().GetAddressOf());
	}
}

void Pipeline::SetSamplerState(uint32 slot, uint32 scope, std::shared_ptr<SamplerState> samplerState)
{
	if (scope & SS_VertexShader)
	{
		_deviceContext->VSSetSamplers(slot, 1, samplerState->GetComPtr().GetAddressOf());
	}
	if (scope & SS_PixelShader)
	{
		_deviceContext->PSSetSamplers(slot, 1, samplerState->GetComPtr().GetAddressOf());
	}
}

void Pipeline::Draw(uint32 vertexCount, uint32 startVertexLocation)
{
	/* 실제 삼각형 그리기 (정점 개수 3, 시작 offset 0) */
	_deviceContext->Draw(vertexCount, startVertexLocation);
}

void Pipeline::DrawIndexed(uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation)
{
	_deviceContext->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}