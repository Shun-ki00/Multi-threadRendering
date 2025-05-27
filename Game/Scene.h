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
		DirectX::SimpleMath::Vector4 baseColor; // 基本色
		float matallic;                         // 金属度
		float smoothness;                       // 表面の滑らかさ
		float useBaseMap;                       // ベースカラーテクスチャを使用するか
		float useNormalMap;                     // 法線マップを使用するか
	};

	struct AmbientLightParameters
	{
		DirectX::SimpleMath::Vector3 ambientLightColor;
		float ambientLightIntensity;
	};

public:

	// コンストラクタ
	Scene();
	// デストラクタ
	~Scene() = default;

public:

	// 初期化処理
	void Initialize();
	// 更新処理
	void Update(const float& elapsedTime);
	// 描画処理
	void Render();
	// 終了処理
	void Finalize();

	void RenderImGui();

private:

	// シェーダー、バッファの作成
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


	// 共有リソース
	CommonResources* m_commonResources;

	// デバッグカメラ
	std::unique_ptr<DebugCamera> m_camera;

	// デバイス
	ID3D11Device1* m_device;
	// コンテキスト
	ID3D11DeviceContext1* m_context;

	// 定数バッファ
	PBRLitConstantBuffer m_pbrConstantBuffer;

	// モデル
	std::unique_ptr<DirectX::Model> m_modelA;
	std::unique_ptr<DirectX::Model> m_modelB;

	// ディファ―ドコンテキスト
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deferredContextA;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deferredContextB;

	// コモンステート
	std::unique_ptr<DirectX::CommonStates> m_commonStatesA;
	std::unique_ptr<DirectX::CommonStates> m_commonStatesB;

	// スレッド
	std::jthread m_threadA;
	std::jthread m_threadB;

	std::atomic<bool> m_recordingDoneA = false;
	std::atomic<bool> m_recordingDoneB = false;

	// コマンドリスト
	Microsoft::WRL::ComPtr<ID3D11CommandList> m_commandListA;
	Microsoft::WRL::ComPtr<ID3D11CommandList> m_commandListB;
	
	// 頂点シェーダー
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	// ピクセルシェーダー
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// 定数バッファ
	std::unique_ptr<ConstantBuffer<PBRLitConstantBuffer>> m_PBRLitConstantBufferA;
	std::unique_ptr<ConstantBuffer<PBRLitConstantBuffer>> m_PBRLitConstantBufferB;
	// 環境ライト
	std::unique_ptr<ConstantBuffer<AmbientLightParameters>> m_ambientLightParametersA;
	std::unique_ptr<ConstantBuffer<AmbientLightParameters>> m_ambientLightParametersB;

	// ベーステクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_baseTexture;
	// ノーマルマップ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMap;
	// キューブマップ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubeMap;
};