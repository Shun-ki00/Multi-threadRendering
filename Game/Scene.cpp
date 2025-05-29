#include "pch.h"
#include "Game/Scene.h"
#include "Framework/CommonResources.h"
#include "Framework/DebugCamera.h"
#include "Framework/ConstantBuffer.h"
#include "Framework/Microsoft/ReadData.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"
#include "imgui/ImGuizmo.h"
#include <Model.h>


/// <summary>
/// �R���X�g���N�^
/// </summary>
Scene::Scene()
{
	// �C���X�^���X���擾����
	m_commonResources = CommonResources::GetInstance();
	m_device          = CommonResources::GetInstance()->GetDeviceResources()->GetD3DDevice();
	m_context         = CommonResources::GetInstance()->GetDeviceResources()->GetD3DDeviceContext();
}

/// <summary>
/// ����������
/// </summary>
void Scene::Initialize()
{
	// �J�����̍쐬
	m_camera = std::make_unique<DebugCamera>();
	m_camera->Initialize(1280, 720);

	//DefferdContext�̍쐬
	m_device->CreateDeferredContext(0, m_deferredContextA.GetAddressOf());
	m_device->CreateDeferredContext(0, m_deferredContextB.GetAddressOf());

	//	�R�����X�e�[�g�̍쐬
	m_commonStatesA = std::make_unique<DirectX::CommonStates>(m_device);
	m_commonStatesB = std::make_unique<DirectX::CommonStates>(m_device);

	// ����������
	PBRLitConstantBuffer pbr{
		DirectX::SimpleMath::Vector4::One,
		1.0f,
		0.0f,
		1.0f,
		1.0f
	};

	m_pbrConstantBuffer = pbr;

	AmbientLightParameters ambient{
		DirectX::SimpleMath::Vector3::One,
		1.0f
	};

	// �萔�o�b�t�@�̍쐬
	m_PBRLitConstantBufferA = std::make_unique<ConstantBuffer<PBRLitConstantBuffer>>();
	m_PBRLitConstantBufferA->Initialize(m_device);
	m_PBRLitConstantBufferA->Update(m_context,pbr);
	m_ambientLightParametersA = std::make_unique<ConstantBuffer<AmbientLightParameters>>();
	m_ambientLightParametersA->Initialize(m_device);
	m_ambientLightParametersA->Update(m_context, ambient);

	m_PBRLitConstantBufferB = std::make_unique<ConstantBuffer<PBRLitConstantBuffer>>();
	m_PBRLitConstantBufferB->Initialize(m_device);
	m_PBRLitConstantBufferB->Update(m_context, pbr);
	m_ambientLightParametersB = std::make_unique<ConstantBuffer<AmbientLightParameters>>();
	m_ambientLightParametersB->Initialize(m_device);
	m_ambientLightParametersB->Update(m_context, ambient);


	// ���f�������[�h����@============================================

	// �G�t�F�N�g�t�@�N�g���𐶐�����
	std::unique_ptr<DirectX::EffectFactory> effectFactory = std::make_unique<DirectX::EffectFactory>(m_device);
	// �f�B���N�g����ݒ肷��
	effectFactory->SetDirectory(L"Resources/Model");
	// ���f���̃��[�h
	m_modelA  = DirectX::Model::CreateFromCMO(m_device, L"Resources/Model/torus.cmo", *effectFactory);
	m_modelB  = DirectX::Model::CreateFromCMO(m_device, L"Resources/Model/torus.cmo", *effectFactory);

	// ���f���̃G�t�F�N�g�����X�V����
	m_modelA->UpdateEffects([](DirectX::IEffect* effect) {
		// �x�[�V�b�N�G�t�F�N�g��ݒ肷��
		DirectX::BasicEffect* basicEffect = dynamic_cast<DirectX::BasicEffect*>(effect);
		if (basicEffect)
		{
			// �g�U���ˌ�
			DirectX::SimpleMath::Color diffuseColor = DirectX::SimpleMath::Color(1.0f, 0.95f, 0.9f);
			// ���C�g���Ƃ炷����
			DirectX::SimpleMath::Vector3 lightDirection(0.0f, 1.0f, 0.0f);

			basicEffect->SetLightEnabled(1, false);
			basicEffect->SetLightEnabled(2, false);

			// �[���Ԃ̃��C�g�Ɋg�U���ˌ���ݒ肷��
			basicEffect->SetLightDiffuseColor(0, diffuseColor);
			// �[���Ԃ̃��C�g���Ƃ炷������ݒ肷��
			basicEffect->SetLightDirection(0, lightDirection);
		}
		});


	// ���f���̃G�t�F�N�g�����X�V����
	m_modelB->UpdateEffects([](DirectX::IEffect* effect) {
		// �x�[�V�b�N�G�t�F�N�g��ݒ肷��
		DirectX::BasicEffect* basicEffect = dynamic_cast<DirectX::BasicEffect*>(effect);
		if (basicEffect)
		{
			// �g�U���ˌ�
			DirectX::SimpleMath::Color diffuseColor = DirectX::SimpleMath::Color(1.0f, 0.95f, 0.9f);
			// ���C�g���Ƃ炷����
			DirectX::SimpleMath::Vector3 lightDirection(0.0f, 1.0f, 0.0f);

			basicEffect->SetLightEnabled(1, false);
			basicEffect->SetLightEnabled(2, false);

			// �[���Ԃ̃��C�g�Ɋg�U���ˌ���ݒ肷��
			basicEffect->SetLightDiffuseColor(0, diffuseColor);
			// �[���Ԃ̃��C�g���Ƃ炷������ݒ肷��
			basicEffect->SetLightDirection(0, lightDirection);
		}
		});

	
	// �e�N�X�`�������[�h���� ===========================================

	// �x�[�X�e�N�X�`��
	DirectX::CreateWICTextureFromFile(
	m_device, L"Resources/Textures/wood.png", nullptr, m_baseTexture.ReleaseAndGetAddressOf());
	// �m�[�}���}�b�v
	DirectX::CreateWICTextureFromFile(
		m_device, L"Resources/Textures/woodNormal.png", nullptr, m_normalMap.ReleaseAndGetAddressOf());
	// �L���[�u�}�b�v
	DirectX::CreateDDSTextureFromFile(
		m_device, L"Resources/Textures/DirectXTKCubeMap.dds", nullptr, m_cubeMap.ReleaseAndGetAddressOf());


	// �V�F�[�_�[�A�o�b�t�@�̍쐬
	this->CreateShaderAndBuffer();
}

/// <summary>
/// �X�V����
/// </summary>
/// <param name="elapsedTime">�o�ߎ���</param>
void Scene::Update(const float& elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	m_camera->Update();
	m_commonResources->SetViewMatrix(m_camera->GetViewMatrix());

}

void Scene::RenderImGui()
{
	static bool chackBoxB = false;
	static bool chackBoxN = false;

	// ImGui �E�B���h�E
	if (ImGui::Begin("PBR Material Editor")) {
		// Base Color�iVector4: RGBA�j
		ImGui::ColorEdit4("Base Color", reinterpret_cast<float*>(&m_pbrConstantBuffer.baseColor));

		// Metallic�i0.0 �` 1.0�j
		ImGui::SliderFloat("Metallic", &m_pbrConstantBuffer.matallic, 0.0f, 1.0f);

		// Smoothness�i0.0 �` 1.0�j
		ImGui::SliderFloat("Smoothness", &m_pbrConstantBuffer.smoothness, 0.0f, 1.0f);

		// Use Base Map�i0 or 1�j
		ImGui::Checkbox("Use Base Map", &chackBoxB);

		// Use Normal Map�i0 or 1�j
		ImGui::Checkbox("Use Normal Map", &chackBoxN);
	}
	ImGui::End();

	m_pbrConstantBuffer.useBaseMap   = static_cast<int>(chackBoxB);
	m_pbrConstantBuffer.useNormalMap = static_cast<int>(chackBoxN);

	// �萔�o�b�t�@���X�V����
	m_PBRLitConstantBufferA->UpdateIfNeeded(m_context, m_pbrConstantBuffer);
	m_PBRLitConstantBufferB->UpdateIfNeeded(m_context, m_pbrConstantBuffer);
}

/// <summary>
/// �`�揈��
/// </summary>
void Scene::Render()
{

	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(2.0f);
	DirectX::SimpleMath::Matrix view = m_camera->GetViewMatrix();
	DirectX::SimpleMath::Matrix proj = m_commonResources->GetProjectionMatrix();

	// �X���b�h�J�n�i�`��R�}���h�L�^�̂݁j
	m_recordingDoneA = false;
	m_recordingDoneB = false;


	// �f�B�t�@�\�h�R���e�L�X�g�̏���������
	
	// �ŏI�o�̓^�[�Q�b�g�ɖ߂�
	auto defaultRTV = m_commonResources->GetDeviceResources()->GetRenderTargetView();
	auto defaultDSV = m_commonResources->GetDeviceResources()->GetDepthStencilView();
	m_deferredContextA->OMSetRenderTargets(1, &defaultRTV, defaultDSV);
	m_deferredContextB->OMSetRenderTargets(1, &defaultRTV, defaultDSV);

	// �X�N���[���r���[�|�[�g���擾����
	auto const viewport = m_commonResources->GetDeviceResources()->GetScreenViewport();
	// �r���[�|�[�g��ݒ肷��
	m_deferredContextA->RSSetViewports(1, &viewport);
	m_deferredContextB->RSSetViewports(1, &viewport);

	// �X���b�hA
	m_threadA = std::jthread(&Scene::ModelRender, this,
		m_deferredContextA.Get(),
		std::ref(m_commandListA),
		m_modelA.get(),
		m_commonStatesA.get(),
		DirectX::SimpleMath::Matrix::CreateTranslation(-2, 0, 0), view, proj,
		m_PBRLitConstantBufferA->GetBuffer(), m_ambientLightParametersA->GetBuffer(),
		std::ref(m_recordingDoneA)
	);

	// �X���b�hB
	m_threadB = std::jthread(&Scene::ModelRender, this,
		m_deferredContextB.Get(),
		std::ref(m_commandListB),
		m_modelB.get(),
		m_commonStatesB.get(),
		DirectX::SimpleMath::Matrix::CreateTranslation(2, 0, 0), view, proj,
		m_PBRLitConstantBufferB->GetBuffer(),m_ambientLightParametersB->GetBuffer(),
		std::ref(m_recordingDoneB)
	);
	
	m_threadA.join();
	m_threadB.join();

	m_context->ExecuteCommandList(m_commandListA.Get(), TRUE);
	m_context->ExecuteCommandList(m_commandListB.Get(), TRUE);
	m_commandListA.Reset(); // ���̃t���[���ōė��p����ꍇ
	m_commandListB.Reset(); // ���̃t���[���ōė��p����ꍇ

	//// ���X���b�h�̏I����ҋ@
	//while (!m_recordingDoneA.load() || !m_recordingDoneB.load()) {
	//	std::this_thread::yield(); // ���ʂ�CPU�g�p��}����
	//}

	// CommandList ���s�iDeferredContext �ŋL�^���ꂽ�`��R�}���h�j
	//if (m_commandListA)
	//{
	//	m_context->ExecuteCommandList(m_commandListA.Get(), FALSE);
	//	m_commandListA.Reset(); // ���̃t���[���ōė��p����ꍇ
	//}
	//if (m_commandListB)
	//{
	//	m_context->ExecuteCommandList(m_commandListB.Get(), FALSE);
	//	m_commandListB.Reset(); // ���̃t���[���ōė��p����ꍇ
	//}
}

/// <summary>
/// �I������
/// </summary>
void Scene::Finalize() {}


/// <summary>
/// �V�F�[�_�[�A�o�b�t�@�̍쐬
/// </summary>
void Scene::CreateShaderAndBuffer()
{
	// �V�F�[�_��ǂݍ��ނ��߂�blob
	std::vector<uint8_t> blob;

	// ���_�V�F�[�_�����[�h����
	blob = DX::ReadData(L"Resources/Shaders/cso/PBRLit_VS.cso");
	DX::ThrowIfFailed(
		m_device->CreateVertexShader(blob.data(), blob.size(), nullptr, m_vertexShader.ReleaseAndGetAddressOf())
	);

	//	�C���v�b�g���C�A�E�g�̍쐬
	m_device->CreateInputLayout(
		DirectX::VertexPositionNormalTangentColorTexture::InputElements,
		DirectX::VertexPositionNormalTangentColorTexture::InputElementCount,
		blob.data(), blob.size(),
		m_inputLayout.GetAddressOf());


	// �s�N�Z���V�F�[�_�����[�h����
	blob = DX::ReadData(L"Resources/Shaders/cso/PBRLit_PS.cso");
	DX::ThrowIfFailed(
		m_device->CreatePixelShader(blob.data(), blob.size(), nullptr, m_pixelShader.ReleaseAndGetAddressOf())
	);

}


void Scene::ModelRender(
	ID3D11DeviceContext* context,
	Microsoft::WRL::ComPtr<ID3D11CommandList>& commandList,
	DirectX::Model* model,
	DirectX::CommonStates* states,
	DirectX::SimpleMath::Matrix world,
	DirectX::SimpleMath::Matrix view,
	DirectX::SimpleMath::Matrix proj,
	ID3D11Buffer* cosntBuffer,
	ID3D11Buffer* lightBuffer,
	std::atomic<bool>& doneFlag)
{
	
	// ���f���`��
	model->Draw(context, *states, world, view, proj, false, [&]
		{
			// �萔�o�b�t�@���w�肷��
			ID3D11Buffer* cbuf[] = { lightBuffer };
			context->VSSetConstantBuffers(1, 1, cbuf);
			context->PSSetConstantBuffers(1, 1, cbuf);

			// �u�����h�X�e�[�g��ݒ� (�������`��p)
			context->OMSetBlendState(states->AlphaBlend(), nullptr, 0xFFFFFFFF);

			context->IASetInputLayout(m_inputLayout.Get());

			// �萔�o�b�t�@���w�肷��
			cbuf[0] = { cosntBuffer };
			context->VSSetConstantBuffers(2, 1, cbuf);
			context->PSSetConstantBuffers(2, 1, cbuf);

			// �e�N�X�`���̐ݒ�
			std::vector<ID3D11ShaderResourceView*> tex = {
				m_baseTexture.Get(),
				m_normalMap.Get(),
				m_cubeMap.Get(),
			};

			context->VSSetShaderResources(0, (UINT)tex.size(), tex.data());
			context->PSSetShaderResources(0, (UINT)tex.size(), tex.data());

			// �V�F�[�_�[��ݒ�
			context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
			context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

			// �T���v���[�X�e�[�g���w�肷��
			ID3D11SamplerState* sampler[] = { states->LinearWrap() };
			context->VSSetSamplers(0, 1, sampler);
			context->PSSetSamplers(0, 1, sampler);
		});

	// �V�F�[�_�̉��
	context->VSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
	// �e�N�X�`�����\�[�X�̉��
	ID3D11ShaderResourceView* nullsrv[] = { nullptr };
	context->PSSetShaderResources(0, 1, nullsrv);
	context->PSSetShaderResources(1, 1, nullsrv);
	context->PSSetShaderResources(2, 1, nullsrv);

	context->FinishCommandList(FALSE,commandList.ReleaseAndGetAddressOf());

	doneFlag.store(true);
}