// Vertex Shader

struct VS_INPUT
{
	// x, y, z, w
	float4 position : Position;
	// r, g, b, a
	//float4 color	: COLOR;
	float2 uv : TEXCOORD;
};

struct	VS_OUTPUT
{
	float4 position : SV_Position;
	//float4 color	: COLOR;
	float2 uv : TEXCOORD;
};

cbuffer TransformData : register(b0)
{
	float4 offset;
}

// IA - VS - RS(VS_main���� �ڵ� ó��) - PS - OM
// VS_main�� ���� ������ ����, ��ġ ���� ó��
VS_OUTPUT VS_main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = input.position + offset;
	output.uv = input.uv;

	return output;
}

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

// Pixel Shader, ��� �ȼ� ������� ����, ���� ���� ó��
// �ȼ� ���̴� PS�� ������� ���� Ÿ�� 0���� ����
float4 PS(VS_OUTPUT input) : SV_Target
{
	// texture0�� input.uv��ǥ�� �ش��ϴ� ���� color�� ���� 
	float4 color = texture0.Sample(sampler0, input.uv);	
	
	// texture0�� resource�� ���İ��� 0.1���� �۴ٸ� ȭ�鿡 ���X
	if(color.a <= 0.1f)
		discard;

	return color;
}