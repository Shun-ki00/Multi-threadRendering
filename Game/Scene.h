#pragma once
#include <future>
#include "Framework/DebugCamera.h"
#include "Framework/ConstantBuffer.h"
#include <thread>

class CommonResources;
class DebugCamera;

class Scene
{
private:


	struct PBRLitConstantBuffer
	{
		DirectX::SimpleMath::Vector4 baseColor; // ��{�F
		float matallic;                         // �����x
		float smoothness;                       // �\�ʂ̊��炩��
		float useBaseMap;                       // �x�[�X�J���[�e�N�X�`�����g�p���邩
		float useNormalMap;                     // �@���}�b�v���g�p���邩
	};

	struct AmbientLightParameters
	{
		DirectX::SimpleMath::Vector3 ambientLightColor;
		float ambientLightIntensity;
	};

public:

	// �R���X�g���N�^
	Scene();
	// �f�X�g���N�^
	~Scene() = default;

public:

	// ����������
	void Initialize();
	// �X�V����
	void Update(const float& elapsedTime);
	// �`�揈��
	void Render();
	// �I������
	void Finalize();

	void RenderImGui();

private:

	// �V�F�[�_�[�A�o�b�t�@�̍쐬
	void CreateShaderAndBuffer();
	

	void ModelRender(
		ID3D11DeviceContext* context,
		Microsoft::WRL::ComPtr<ID3D11CommandList>& commandList,
		DirectX::Model* model,
		DirectX::CommonStates* states,
		DirectX::SimpleMath::Matrix world,
		DirectX::SimpleMath::Matrix view,
		DirectX::SimpleMath::Matrix proj,
		ID3D11Buffer* cosntBuffer,
		ID3D11Buffer* lightBuffer,
		std::atomic<bool>& doneFlag);


private:


	// ���L���\�[�X
	CommonResources* m_commonResources;

	// �f�o�b�O�J����
	std::unique_ptr<DebugCamera> m_camera;

	// �f�o�C�X
	ID3D11Device1* m_device;
	// �R���e�L�X�g
	ID3D11DeviceContext1* m_context;

	// �萔�o�b�t�@
	PBRLitConstantBuffer m_pbrConstantBuffer;

	// ���f��
	std::unique_ptr<DirectX::Model> m_modelA;
	std::unique_ptr<DirectX::Model> m_modelB;

	// �f�B�t�@�\�h�R���e�L�X�g
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deferredContextA;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deferredContextB;

	// �R�����X�e�[�g
	std::unique_ptr<DirectX::CommonStates> m_commonStatesA;
	std::unique_ptr<DirectX::CommonStates> m_commonStatesB;

	// �X���b�h
	std::jthread m_threadA;
	std::jthread m_threadB;

	std::atomic<bool> m_recordingDoneA = false;
	std::atomic<bool> m_recordingDoneB = false;

	// �R�}���h���X�g
	Microsoft::WRL::ComPtr<ID3D11CommandList> m_commandListA;
	Microsoft::WRL::ComPtr<ID3D11CommandList> m_commandListB;
	
	// ���_�V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	// �s�N�Z���V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

	// ���̓��C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// �萔�o�b�t�@
	std::unique_ptr<ConstantBuffer<PBRLitConstantBuffer>> m_PBRLitConstantBufferA;
	std::unique_ptr<ConstantBuffer<PBRLitConstantBuffer>> m_PBRLitConstantBufferB;
	// �����C�g
	std::unique_ptr<ConstantBuffer<AmbientLightParameters>> m_ambientLightParametersA;
	std::unique_ptr<ConstantBuffer<AmbientLightParameters>> m_ambientLightParametersB;

	// �x�[�X�e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_baseTexture;
	// �m�[�}���}�b�v
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMap;
	// �L���[�u�}�b�v
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubeMap;
};