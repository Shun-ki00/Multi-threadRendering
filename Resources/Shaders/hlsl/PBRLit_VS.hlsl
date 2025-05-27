#include "PBRLit.hlsli"

PS_Input main(VS_Input input)
{
    // �o�͍\����0������
    PS_Input output = (PS_Input)0;
    
    // ���[���h��Ԃ֕ϊ�
    output.positionWS = mul(float4(input.positionOS.xyz, 1.0), World).xyz;
    // ���e��Ԃ֕ϊ�
    output.positionCS = mul(float4(input.positionOS.xyz, 1.0), WorldViewProj);
    
    
    // �@���x�N�g�������[���h��Ԃցi�g��k���̉e����ł��������ߋt�]�u�s���������j
    output.normalWS = normalize(mul(input.normalOS, WorldInverseTranspose));
    // �ڐ��x�N�g�������[���h��Ԃ�
    output.tangentWS = normalize(mul(float4(input.tangentOS.xyz, 0), World).xyz);
    // �o�C�m�[�}���v�Z
    output.binormalWS = normalize(cross(output.normalWS, output.tangentWS));

        
    // �e�N�X�`����UV���W
    output.uv = input.uv;
    //output.uv.y *= -1.0f;
    
    // ���_�J���[
    output.color = input.color;
        
    return output;
}