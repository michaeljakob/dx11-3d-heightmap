
struct VS_INPUT
{
	float3 pos : POSITION;
	float2 texcoords : TEXCOORDS0;
	float3 normal : TEXCOORDS1;
};

struct PS_INPUT
{
	float4 hpos : SV_Position;
	float2 texcoords : TEXCOORDS0;
	float3 normal : TEXCOORDS1;
	float3 pos : POSITION;
	//float fog : FOG;
};

struct SpotLight
{
	float4 pos;
	float4 dir;
	float4 col;
};

struct DirectionalLight
{
	float4 dir;
	float4 col;
};

cbuffer ConstantBuffer : register(b0)
{
	float4x4 worldViewProj;
	float4 special;
	float4 camPos;
	DirectionalLight dirLight;
	SpotLight spotLight;
}


Texture2D gTexture2D_1 : register(t0);
Texture2D gTexture2D_2 : register(t1);
SamplerState gSamplerState : register(s0);

// VertexShader
PS_INPUT VxShader(VS_INPUT input)
{
	PS_INPUT outt;
	outt.hpos = mul( float4(input.pos, 1.0f), worldViewProj);
	outt.texcoords = input.texcoords;
	outt.normal = input.normal;
	outt.pos = input.pos;
	
	return outt;
}

// PixelShader
float4 PxShader(PS_INPUT input) : SV_Target
{
	
	if(special[2] == 1)
	{
		return float4(0, 0, 0, 1);
	}

	float4 tex1 = gTexture2D_1.Sample(gSamplerState, input.texcoords);
	float4 tex2 = gTexture2D_2.Sample(gSamplerState, input.texcoords);
	

	// directional light
	float4 color = tex1;
	color += dot(input.normal, dirLight.dir) * dirLight.col * 0.5;

	// spot light
	float3 l = normalize(spotLight.pos.xyz - input.pos);	// normalerweise andersrum
	float phi = max(dot(l, spotLight.dir.xyz) - 0.3f, 0.0f);	//(1.0f / 0.3f);
	color += float4(dot(input.normal, l).xxx * phi, 1) * spotLight.col;
	
	color += tex2 * ((sin(special[0])+1)/4);
	

	//color *= (sin(special[0]) + 1) * 2;

	// point light
	//float3 p = (float3) myPoint.pos - input.pos;
	//foo += 1 / (dot(p,p) / myPoint.pos - 0.03 ) * myPoint.col * 10;	//* (cos(special[0])*0.5+0.5)*0.5;
	//foo = dot(input.normal, normalize(p)) * myPoint.col;


	//float4 fogIntens = distance(camPos.xz, input.pos.xyz);
	//foo += sin(fogIntens)*0.2 + foo;
	//fogIntens = saturate(fogIntens);
	//foo = lerp( foo, float4(0, 0, 0, 1), fogIntens);
	//foo = fogIntens * float4(0, 0, 0, 1) + (1 - fogIntens) * foo;

	// specular
	// PixelColor = SColor · SIntensity pow(( V · R ), n)
	// R = 2 · ( N · L ) · N – L
	// V = EyePosition – VertexPosition
	//float3 r = 2 * ( normalize(input.normal) * normalize(dirLight.dir.xyz) ) * normalize(input.normal) - normalize(dirLight.dir.xyz);
	//float3 v = normalize(camPos - input.pos);
	//float3 spec = float4(1, 0, 1, 1) * 1 * pow((v * r), 50);
	//foo += float4(spec, 1);
	//foo += float4(0.0f, 1.0f, 0.0f, 1.0f) * 0.05;		// ambient light

	return color;
}
