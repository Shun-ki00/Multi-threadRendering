//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "Framework/CommonResources.h"
#include "Game/Scene.h"
#include "Framework/InputManager.h"
#include "Framework/Microsoft/DebugDraw.h"


extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    // ���L���\�[�X�̃C���X�^���X���擾����
    m_commonResources = CommonResources::GetInstance();


    // �f�o�C�X���\�[�X���쐬����
    m_deviceResources = std::make_unique<DX::DeviceResources>(
        DXGI_FORMAT_B8G8R8A8_UNORM,     // �o�b�N�o�b�t�@�̃t�H�[�}�b�g���w�肷��
        DXGI_FORMAT_D24_UNORM_S8_UINT   // �X�e���V���o�b�t�@���g�p����
    );
    // �f�o�C�X�̒ʒm���ݒ肷��
    m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

// Direct3D���\�[�X������������
void Game::Initialize(HWND window, int width, int height)
{

    // �f�o�C�X���\�[�X������================================================

        // �E�B���h�E�n���h����ݒ肷��
    m_hWnd = window;
    // �f�o�C�X���\�[�X�𐶐�����
    m_deviceResources->CreateDeviceResources();
    // �f�o�C�X�Ɉˑ��������\�[�X�𐶐�����
    CreateDeviceDependentResources();
    // �E�B���h�E�T�C�Y�Ɉˑ��������\�[�X�𐶐�����
    m_deviceResources->CreateWindowSizeDependentResources();
    // �E�B���h�E�T�C�Y�Ɉˑ��������\�[�X�𐶐�����
    CreateWindowSizeDependentResources();

    // �f�o�C�X�ƃR���e�L�X�g��ݒ�
    m_device = m_deviceResources->GetD3DDevice();
    m_context = m_deviceResources->GetD3DDeviceContext();

    // �f�o�C�X���\�[�X�����L���\�[�X�ɐݒ�
    m_commonResources->SetDeviceResources(m_deviceResources.get());

    m_commonStates = std::make_unique<CommonStates>(m_device);
    m_commonResources->SetCommonStates(m_commonStates.get());

    // ���̓}�l�[�W���[�̏���������
    m_inputManager = InputManager::GetInstance();
    m_inputManager->GetMouse()->SetWindow(window);

    // �^�C�}�[�ݒ��ύX����   
    m_timer.SetFixedTimeStep(true);
    // �^�C�}�[�Ƀ^�[�Q�b�g�o�ߕb(60FPS)��ݒ肷��
    m_timer.SetTargetElapsedSeconds(1.0 / 60);

    // �V�[���̍쐬
    m_scene = std::make_unique<Scene>();
    m_scene->Initialize();

 


    // ImGui�̏�����========================================================

    //  �o�[�W�����̊m�F
    IMGUI_CHECKVERSION();
    //  �R���e�L�X�g�̍쐬
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // �L�[�{�[�h�ɂ��i�r�Q�[�V�����̗L����
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // �R���g���[���[�ɂ��i�r�Q�[�V�����̗L����
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    // �}���`�r���[�|�[�g�L����
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    // �h�b�L���O�L����
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // ���{��Ή�
	ImFontConfig config;
    config.FontDataOwnedByAtlas = false;
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\meiryo.ttc", 20.0f, &config, io.Fonts->GetGlyphRangesJapanese());
	
    //  Win32�p�̏�����
    ImGui_ImplWin32_Init(window);
    //  DirectX11�p�̏�����
    ID3D11Device* device = m_deviceResources->GetD3DDevice();
    ID3D11DeviceContext* context = m_deviceResources->GetD3DDeviceContext();
    ImGui_ImplDX11_Init(device, context);

   
    // �v���W�F�N�V�����s��̍쐬
    m_commonResources->SetProjectionMatrix(
        DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
            DirectX::XMConvertToRadians(45.0f),
            (float)width / (float)height,
            0.1f,
            100.0f
        ));

    // �x�[�V�b�N�G�t�F�N�g���쐬
    m_basicEffect = std::make_unique<DirectX::BasicEffect>(
        m_deviceResources->GetD3DDevice()
    );
    // ���_�J���[��L���ɂ���
    m_basicEffect->SetVertexColorEnabled(true);
    // �e�N�X�`���𖳌��ɂ���
    m_basicEffect->SetTextureEnabled(false);
    void const* shaderByteCode;
    size_t byteCodeLength;
    // ���_�V�F�[�_�[���擾����
    m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
    // ���̓��C�A�E�g�𐶐�����
    m_device->CreateInputLayout(
        DirectX::VertexPositionColor::InputElements,
        DirectX::VertexPositionColor::InputElementCount,
        shaderByteCode, byteCodeLength,
        m_inputLayout.ReleaseAndGetAddressOf()
    );
    //	�X�v���C�g�o�b�`�̍쐬
    m_spriteBatch = std::make_unique<SpriteBatch>(
        m_context
    );
    //	�X�v���C�g�t�H���g�̍쐬
    m_spriteFont = std::make_unique<SpriteFont>(
        m_device,
        L"Resources/fonts/SegoeUI_18.spritefont"
    );
    // �v���~�e�B�u�o�b�`�̍쐬
    m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(
        m_deviceResources->GetD3DDeviceContext()
    );

}

#pragma region Frame Update
// �Q�[�����[�v�����s����
void Game::Tick()
{
    m_timer.Tick([&]()
        {
            Update(m_timer);
        });
    // �`�悷��
    Render();
}

// ���[���h���X�V����
void Game::Update(DX::StepTimer const& timer)
{
    (void)timer;

    m_scene->Update((float)timer.GetElapsedSeconds());

}
#pragma endregion

#pragma region Frame Render
// �V�[����`�悷��
void Game::Render()
{
    // �`��\�łȂ��ꍇ�͕��A����
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    // �o�b�N�o�b�t�@���N���A����
    Clear();

    // PIX�J�n�C�x���g
    m_deviceResources->PIXBeginEvent(L"Render");


    // �V�[���̕`��
    m_scene->Render();

    // �r���[���N���A����
    auto context = m_deviceResources->GetD3DDeviceContext();
    // �����_�[�^�[�Q�b�g���擾����
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    // �[�x�X�e���V���r���[���擾����
    auto depthStencil = m_deviceResources->GetDepthStencilView();
    // �����_�[�^�[�Q�b�g��ݒ肷��
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // �X�N���[���r���[�|�[�g���擾����
    auto const viewport = m_deviceResources->GetScreenViewport();
    // �r���[�|�[�g��ݒ肷��
    context->RSSetViewports(1, &viewport);


    wchar_t stringBuffer[128];

    // ��ʃT�C�Y
    int screenWidth, screenHeight;
    // ��ʃT�C�Y���擾����
    GetDefaultSize(screenWidth, screenHeight);

    // �ˉe�s��𐶐����� 
    DirectX::SimpleMath::Matrix projection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
        DirectX::XMConvertToRadians(45.0f),
        screenWidth / (float)screenHeight,
        0.1f,
        100.0f
    );

    // �r���[�s���ݒ肷��
    m_basicEffect->SetView(m_commonResources->GetViewMatrix());
    // �v���W�F�N�V�����s���ݒ肷��
    m_basicEffect->SetProjection(projection);
    // ���[���h�s���ݒ肷��
    m_basicEffect->SetWorld(DirectX::SimpleMath::Matrix::Identity);
    // �R���e�L�X�g��ݒ肷��
    // �[�x�X�e���V���̐ݒ�i�[�x�e�X�g��L�����j
    m_context->OMSetDepthStencilState(nullptr, 0); // �f�t�H���g�̐[�x�X�e�[�g���g�p
    m_basicEffect->Apply(m_context);
    // ���̓��C�A�E�g��ݒ肷��
    m_context->IASetInputLayout(m_inputLayout.Get());

    // �O���b�h��`��
    m_primitiveBatch->Begin();
    DX::DrawGrid(m_primitiveBatch.get(),
        { 5.0f, 0.0f, 0.0f },              // ����
        { 0.0f,0.0f,5.0f },                // �c��
        DirectX::SimpleMath::Vector3::Zero, // ���_
        10,                                 // ��������
        10                                  // �c������
    );

    m_primitiveBatch->End();

    m_spriteBatch->Begin();
    // FPS
    swprintf(stringBuffer, sizeof(stringBuffer) / sizeof(wchar_t), L"FPS : %d", m_timer.GetFramesPerSecond());
    m_spriteFont->DrawString(m_spriteBatch.get(), stringBuffer, SimpleMath::Vector2(10, 20), Colors::White, 0.0f, SimpleMath::Vector2::Zero, 0.8f);
    // �X�N���[���T�C�Y
    swprintf(stringBuffer, sizeof(stringBuffer) / sizeof(wchar_t), L"ScreenW : %d ScreenH : %d", 1280, 720);
    m_spriteFont->DrawString(m_spriteBatch.get(), stringBuffer, SimpleMath::Vector2(10, 40), Colors::White, 0.0f, SimpleMath::Vector2::Zero, 0.8f);
    m_spriteBatch->End();


    //  �V�t���[���̊J�n
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigViewportsNoTaskBarIcon = false; // OS�̃^�X�N�o�[�ɃA�C�R����\�����Ȃ�
    io.ConfigViewportsNoDecoration = false;  // OS�E�B���h�E�̘g������

    // Imgui�̕`��
    m_scene->RenderImGui();

    //  ImGui�̕`�揈��
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        // TODO for OpenGL: restore current GL context.
    }

    // PIX�I���C�x���g
    m_deviceResources->PIXEndEvent();
    // �V�K�t���[����`�悷��
    m_deviceResources->Present();
}

// �o�b�N�o�b�t�@���N���A����
void Game::Clear()
{
    // �r���[���N���A����
    auto context = m_deviceResources->GetD3DDeviceContext();
    // �����_�[�^�[�Q�b�g���擾����
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    // �[�x�X�e���V���r���[���擾����
    auto depthStencil = m_deviceResources->GetDepthStencilView();
    // �����_�[�^�[�Q�b�g�r���[���N���A����
    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    // �[�x�X�e���V���r���[���N���A����
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    // �����_�[�^�[�Q�b�g��ݒ肷��
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // �X�N���[���r���[�|�[�g���擾����
    auto const viewport = m_deviceResources->GetScreenViewport();
    // �r���[�|�[�g��ݒ肷��
    context->RSSetViewports(1, &viewport);
    // PIX�I���C�x���g
    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// ���b�Z�[�W�n���h��
void Game::OnActivated()
{
    // �Q�[�����A�N�e�B�u�E�B���h�E�ɂȂ����Ƃ��̏������L�q����
}

void Game::OnDeactivated()
{
    // �Q�[�����w�i�E�B���h�E�ɂȂ������̏������L�q����
}

void Game::OnSuspending()
{
    // �Q�[���̎��s���ɓd������~�������̏������L�q����
}

void Game::OnResuming()
{
    // �o�ߎ��Ԃ����Z�b�g����
    m_timer.ResetElapsedTime();

    // �Q�[���̎��s���ɓd�������A�������̏������L�q����
}

// �E�B���h�E���ړ������Ƃ��̏������L�q����
void Game::OnWindowMoved()
{
    auto const r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange()
{
    m_deviceResources->UpdateColorSpace();
}

// �E�B���h�E�T�C�Y���ύX���ꂽ���̏������L�q����
void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;
    // �E�B���h�E�T�C�Y�Ɉˑ��������\�[�X�𐶐�����
    CreateWindowSizeDependentResources();

    // �Q�[���E�B���h�E�̃T�C�Y���ύX���ꂽ���̏������L�q����
    //m_commonResources->SetScreenSize(width, height);
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // �K��̃E�B���h�E�T�C�Y
    width = 1280;
    height = 720;
}
#pragma endregion

#pragma region Direct3D Resources
// �f�o�C�X�Ɉˑ��������\�[�X�𐶐�����
void Game::CreateDeviceDependentResources()
{
    //  m_commonResources->SetDeviceResources(m_deviceResources.get());

    auto device = m_deviceResources->GetD3DDevice();
    //	�R�����X�e�[�g�̍쐬
    m_commonStates = std::make_unique<DirectX::CommonStates>(device);
    // �f�o�C�X�Ɉˑ������I�u�W�F�N�g������������
    int width, height;
    // �E�B���h�E�̊���T�C�Y���擾����
    GetDefaultSize(width, height);
    // �u�E�B���h�E�n���h���v�u���v�u�����v��ݒ肷��
    m_deviceResources->SetWindow(m_hWnd, width, height);
}

// �E�B���h�E�T�C�Y���ύX���ꂽ�C�x���g�ɂ�胁�������\�[�X���m�ۂ���
void Game::CreateWindowSizeDependentResources()
{
    // �E�B���h�E�T�C�Y�Ɉˑ������I�u�W�F�N�g������������R�[�h���L�q����
     // �o�̓T�C�Y���擾����
    auto m_size = m_deviceResources->GetOutputSize();
    // �A�X�y�N�g�����v�Z����
    float aspectRatio = float(m_size.right) / float(m_size.bottom);
    // ����p��ݒ肷��
    float fovAngleY = 45.0f * DirectX::XM_PI / 180.0f;
    // �A�v�����c�����܂��̓X�i�b�v�r���[�̂Ƃ��ɕύX����
    if (aspectRatio < 1.0f) { fovAngleY *= 2.0f; }
    // �ˉe�s��𐶐�����
    DirectX::SimpleMath::Matrix projection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
        fovAngleY,
        aspectRatio,
        0.1f,
        1000.0f
    );
    // �ˉe�s���ݒ肷��
    // m_commonResources->SetProjectionMatrix(projection);
}

// �f�o�C�X���X�g�������������̏������L�q����
void Game::OnDeviceLost()
{
    // Direct3D���\�[�X���N���[���A�b�v���鏈�����L�q����
}

// �f�o�C�X�����A�������̏������L�q����
void Game::OnDeviceRestored()
{
    // �f�o�C�X�Ɉˑ��������\�[�X�𐶐�����
    CreateDeviceDependentResources();
    // �E�B���h�E�T�C�Y�Ɉˑ��������\�[�X�𐶐�����
    CreateWindowSizeDependentResources();
}
// �t���X�N���[���Ή�
void Game::SetFullscreenState(BOOL value)
{
    m_full_screen = value;
    m_deviceResources->GetSwapChain()->SetFullscreenState(m_full_screen, nullptr);
    if (value) m_deviceResources->CreateWindowSizeDependentResources();
}

#pragma endregion
