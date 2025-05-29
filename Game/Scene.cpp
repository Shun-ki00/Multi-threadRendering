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
/// コンストラクタ
/// </summary>
Scene::Scene()
{
	// インスタンスを取得する
	m_commonResources = CommonResources::GetInstance();
	m_device          = CommonResources::GetInstance()->GetDeviceResources()->GetD3DDevice();
	m_context         = CommonResources::GetInstance()->GetDeviceResources()->GetD3DDeviceContext();
}

/// <summary>
/// 初期化処理
/// </summary>
void Scene::Initialize()
{
	// カメラの作成
	m_camera = std::make_unique<DebugCamera>();
	m_camera->Initialize(1280, 720);

	//DefferdContextの作成
	m_device->CreateDeferredContext(0, m_deferredContextA.GetAddressOf());
	m_device->CreateDeferredContext(0, m_deferredContextB.GetAddressOf());

	//	コモンステートの作成
	m_commonStatesA = std::make_unique<DirectX::CommonStates>(m_device);
	m_commonStatesB = std::make_unique<DirectX::CommonStates>(m_device);

	// 初期化処理
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

	// 定数バッファの作成
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


	// モデルをロードする　============================================

	// エフェクトファクトリを生成する
	std::unique_ptr<DirectX::EffectFactory> effectFactory = std::make_unique<DirectX::EffectFactory>(m_device);
	// ディレクトリを設定する
	effectFactory->SetDirectory(L"Resources/Model");
	// モデルのロード
	m_modelA  = DirectX::Model::CreateFromCMO(m_device, L"Resources/Model/torus.cmo", *effectFactory);
	m_modelB  = DirectX::Model::CreateFromCMO(m_device, L"Resources/Model/torus.cmo", *effectFactory);

	// モデルのエフェクト情報を更新する
	m_modelA->UpdateEffects([](DirectX::IEffect* effect) {
		// ベーシックエフェクトを設定する
		DirectX::BasicEffect* basicEffect = dynamic_cast<DirectX::BasicEffect*>(effect);
		if (basicEffect)
		{
			// 拡散反射光
			DirectX::SimpleMath::Color diffuseColor = DirectX::SimpleMath::Color(1.0f, 0.95f, 0.9f);
			// ライトが照らす方向
			DirectX::SimpleMath::Vector3 lightDirection(0.0f, 1.0f, 0.0f);

			basicEffect->SetLightEnabled(1, false);
			basicEffect->SetLightEnabled(2, false);

			// ゼロ番のライトに拡散反射光を設定する
			basicEffect->SetLightDiffuseColor(0, diffuseColor);
			// ゼロ番のライトが照らす方向を設定する
			basicEffect->SetLightDirection(0, lightDirection);
		}
		});


	// モデルのエフェクト情報を更新する
	m_modelB->UpdateEffects([](DirectX::IEffect* effect) {
		// ベーシックエフェクトを設定する
		DirectX::BasicEffect* basicEffect = dynamic_cast<DirectX::BasicEffect*>(effect);
		if (basicEffect)
		{
			// 拡散反射光
			DirectX::SimpleMath::Color diffuseColor = DirectX::SimpleMath::Color(1.0f, 0.95f, 0.9f);
			// ライトが照らす方向
			DirectX::SimpleMath::Vector3 lightDirection(0.0f, 1.0f, 0.0f);

			basicEffect->SetLightEnabled(1, false);
			basicEffect->SetLightEnabled(2, false);

			// ゼロ番のライトに拡散反射光を設定する
			basicEffect->SetLightDiffuseColor(0, diffuseColor);
			// ゼロ番のライトが照らす方向を設定する
			basicEffect->SetLightDirection(0, lightDirection);
		}
		});

	
	// テクスチャをロードする ===========================================

	// ベーステクスチャ
	DirectX::CreateWICTextureFromFile(
	m_device, L"Resources/Textures/wood.png", nullptr, m_baseTexture.ReleaseAndGetAddressOf());
	// ノーマルマップ
	DirectX::CreateWICTextureFromFile(
		m_device, L"Resources/Textures/woodNormal.png", nullptr, m_normalMap.ReleaseAndGetAddressOf());
	// キューブマップ
	DirectX::CreateDDSTextureFromFile(
		m_device, L"Resources/Textures/DirectXTKCubeMap.dds", nullptr, m_cubeMap.ReleaseAndGetAddressOf());


	// シェーダー、バッファの作成
	this->CreateShaderAndBuffer();
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param>
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

	// ImGui ウィンドウ
	if (ImGui::Begin("PBR Material Editor")) {
		// Base Color（Vector4: RGBA）
		ImGui::ColorEdit4("Base Color", reinterpret_cast<float*>(&m_pbrConstantBuffer.baseColor));

		// Metallic（0.0 ～ 1.0）
		ImGui::SliderFloat("Metallic", &m_pbrConstantBuffer.matallic, 0.0f, 1.0f);

		// Smoothness（0.0 ～ 1.0）
		ImGui::SliderFloat("Smoothness", &m_pbrConstantBuffer.smoothness, 0.0f, 1.0f);

		// Use Base Map（0 or 1）
		ImGui::Checkbox("Use Base Map", &chackBoxB);

		// Use Normal Map（0 or 1）
		ImGui::Checkbox("Use Normal Map", &chackBoxN);
	}
	ImGui::End();

	m_pbrConstantBuffer.useBaseMap   = static_cast<int>(chackBoxB);
	m_pbrConstantBuffer.useNormalMap = static_cast<int>(chackBoxN);

	// 定数バッファを更新する
	m_PBRLitConstantBufferA->UpdateIfNeeded(m_context, m_pbrConstantBuffer);
	m_PBRLitConstantBufferB->UpdateIfNeeded(m_context, m_pbrConstantBuffer);
}

/// <summary>
/// 描画処理
/// </summary>
void Scene::Render()
{

	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(2.0f);
	DirectX::SimpleMath::Matrix view = m_camera->GetViewMatrix();
	DirectX::SimpleMath::Matrix proj = m_commonResources->GetProjectionMatrix();

	// スレッド開始（描画コマンド記録のみ）
	m_recordingDoneA = false;
	m_recordingDoneB = false;


	// ディファ―ドコンテキストの初期化処理
	
	// 最終出力ターゲットに戻す
	auto defaultRTV = m_commonResources->GetDeviceResources()->GetRenderTargetView();
	auto defaultDSV = m_commonResources->GetDeviceResources()->GetDepthStencilView();
	m_deferredContextA->OMSetRenderTargets(1, &defaultRTV, defaultDSV);
	m_deferredContextB->OMSetRenderTargets(1, &defaultRTV, defaultDSV);

	// スクリーンビューポートを取得する
	auto const viewport = m_commonResources->GetDeviceResources()->GetScreenViewport();
	// ビューポートを設定する
	m_deferredContextA->RSSetViewports(1, &viewport);
	m_deferredContextB->RSSetViewports(1, &viewport);

	// スレッドA
	m_threadA = std::jthread(&Scene::ModelRender, this,
		m_deferredContextA.Get(),
		std::ref(m_commandListA),
		m_modelA.get(),
		m_commonStatesA.get(),
		DirectX::SimpleMath::Matrix::CreateTranslation(-2, 0, 0), view, proj,
		m_PBRLitConstantBufferA->GetBuffer(), m_ambientLightParametersA->GetBuffer(),
		std::ref(m_recordingDoneA)
	);

	// スレッドB
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
	m_commandListA.Reset(); // 次のフレームで再利用する場合
	m_commandListB.Reset(); // 次のフレームで再利用する場合

	//// 両スレッドの終了を待機
	//while (!m_recordingDoneA.load() || !m_recordingDoneB.load()) {
	//	std::this_thread::yield(); // 無駄なCPU使用を抑える
	//}

	// CommandList 実行（DeferredContext で記録された描画コマンド）
	//if (m_commandListA)
	//{
	//	m_context->ExecuteCommandList(m_commandListA.Get(), FALSE);
	//	m_commandListA.Reset(); // 次のフレームで再利用する場合
	//}
	//if (m_commandListB)
	//{
	//	m_context->ExecuteCommandList(m_commandListB.Get(), FALSE);
	//	m_commandListB.Reset(); // 次のフレームで再利用する場合
	//}
}

/// <summary>
/// 終了処理
/// </summary>
void Scene::Finalize() {}


/// <summary>
/// シェーダー、バッファの作成
/// </summary>
void Scene::CreateShaderAndBuffer()
{
	// シェーダを読み込むためのblob
	std::vector<uint8_t> blob;

	// 頂点シェーダをロードする
	blob = DX::ReadData(L"Resources/Shaders/cso/PBRLit_VS.cso");
	DX::ThrowIfFailed(
		m_device->CreateVertexShader(blob.data(), blob.size(), nullptr, m_vertexShader.ReleaseAndGetAddressOf())
	);

	//	インプットレイアウトの作成
	m_device->CreateInputLayout(
		DirectX::VertexPositionNormalTangentColorTexture::InputElements,
		DirectX::VertexPositionNormalTangentColorTexture::InputElementCount,
		blob.data(), blob.size(),
		m_inputLayout.GetAddressOf());


	// ピクセルシェーダをロードする
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
	
	// モデル描画
	model->Draw(context, *states, world, view, proj, false, [&]
		{
			// 定数バッファを指定する
			ID3D11Buffer* cbuf[] = { lightBuffer };
			context->VSSetConstantBuffers(1, 1, cbuf);
			context->PSSetConstantBuffers(1, 1, cbuf);

			// ブレンドステートを設定 (半透明描画用)
			context->OMSetBlendState(states->AlphaBlend(), nullptr, 0xFFFFFFFF);

			context->IASetInputLayout(m_inputLayout.Get());

			// 定数バッファを指定する
			cbuf[0] = { cosntBuffer };
			context->VSSetConstantBuffers(2, 1, cbuf);
			context->PSSetConstantBuffers(2, 1, cbuf);

			// テクスチャの設定
			std::vector<ID3D11ShaderResourceView*> tex = {
				m_baseTexture.Get(),
				m_normalMap.Get(),
				m_cubeMap.Get(),
			};

			context->VSSetShaderResources(0, (UINT)tex.size(), tex.data());
			context->PSSetShaderResources(0, (UINT)tex.size(), tex.data());

			// シェーダーを設定
			context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
			context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

			// サンプラーステートを指定する
			ID3D11SamplerState* sampler[] = { states->LinearWrap() };
			context->VSSetSamplers(0, 1, sampler);
			context->PSSetSamplers(0, 1, sampler);
		});

	// シェーダの解放
	context->VSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
	// テクスチャリソースの解放
	ID3D11ShaderResourceView* nullsrv[] = { nullptr };
	context->PSSetShaderResources(0, 1, nullsrv);
	context->PSSetShaderResources(1, 1, nullsrv);
	context->PSSetShaderResources(2, 1, nullsrv);

	context->FinishCommandList(FALSE,commandList.ReleaseAndGetAddressOf());

	doneFlag.store(true);
}