#include "../Common/util.fx"

float4x4 mvp : WORLDVIEWPROJECTION;
float3 lightPos;
float3 eyePos;

void ParallaxVS(float4 pos				: POSITION,
					float2 texcoord0	: TEXCOORD0,
					float3 T			: TANGENT,
					float3 B			: BINORMAL,
					out float4 oPos			: POSITION,
					out float2 oTexcoord0	: TEXCOORD0,
					out float3 oL			: TEXCOORD1,	// in tangent space
					out float3 oV			: TEXCOORD2,	// in tangent space
					out float3 oH			: TEXCOORD3)	// in tangent space
{
	oPos = mul(pos, mvp);
	oTexcoord0 = texcoord0;

	float3x3 objToTangentSpace;
	objToTangentSpace[0] = T;
	objToTangentSpace[1] = B;
	objToTangentSpace[2] = cross(T, B);

	float3 lightVec = lightPos - pos.xyz;
	float3 viewVec = eyePos - pos.xyz;
	float3 halfVec = lightVec + viewVec;

	oL = mul(objToTangentSpace, lightVec);
	oV = mul(objToTangentSpace, viewVec);
	oH = mul(objToTangentSpace, halfVec);
}


sampler2D diffuseMapSampler;
sampler2D normalMapSampler;
sampler2D heightMapSampler;

half4 ParallaxPS(float2 texCoord0	: TEXCOORD0,
					float3 L		: TEXCOORD1,
					float3 V		: TEXCOORD2,
					float3 H		: TEXCOORD3) : COLOR
{
	half3 view = normalize(V);

	half height = tex2D(heightMapSampler, texCoord0).r * 0.06 - 0.02;
	half2 texUV = texCoord0 + (view.xy * height);

	half3 diffuse = tex2D(diffuseMapSampler, texUV).rgb;

	half3 bump_normal = decompress_normal(tex2D(normalMapSampler, texUV));
	half3 light_vec = normalize(L);
	half diffuse_factor = dot(light_vec, bump_normal);

	half4 clr;
	if (diffuse_factor > 0)
	{
		half3 half_way = normalize(H);
		half specular_factor = pow(dot(half_way, bump_normal), 4);
		clr = half4(diffuse * diffuse_factor + 0.3f * specular_factor, 1);
	}
	else
	{
		clr = half4(0, 0, 0, 1);
	}

	return clr;
}

technique Parallax
{
	pass p0
	{
		CullMode = CCW;
		
		VertexShader = compile vs_1_1 ParallaxVS();
		PixelShader = compile ps_2_0 ParallaxPS();
	}
}
