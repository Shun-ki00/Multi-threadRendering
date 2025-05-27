//
// Game.h
//

#include "DeviceResources.h"

#pragma once


class CommonResources;
class InputManager;
class Scene;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:
    // �R���X�g���N�^
    Game() noexcept(false);
    // �f�X�g���N�^
    ~Game();
    Game(Game&&) = default;
    Game& operator= (Game&&) = default;
    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // ����������
    void Initialize(HWND window, int width, int height);

    // �Q�[�����[�v
    void Tick();

    // IDeviceNotify�C���^�[�t�F�[�X
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // ���b�Z�[�W�n���h���[
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // �K��E�B���h�E�T�C�Y�v���p�e�B���擾����
    void GetDefaultSize(int& width, int& height) const noexcept;
    // ��ʃ��[�h��ύX����֐��iTRUE:�t���X�N���[���j
    void SetFullscreenState(BOOL value);
private:
    // �X�V����
    void Update(DX::StepTimer const& timer);
    // �`�揈��
    void Render();

    void Clear();
    // �f�o�C�X�ˑ��������\�[�X�𐶐�����
    void CreateDeviceDependentResources();
    // �E�B���h�E�T�C�Y�Ɉˑ��������\�[�X�𐶐�����
    void CreateWindowSizeDependentResources();


private:
    // �E�B���h�E�n���h��
    HWND            m_hWnd;
    // �^�C�}�[
    DX::StepTimer   m_timer;

    // �f�o�C�X
    ID3D11Device1* m_device;
    // �R���e�L�X�g
    ID3D11DeviceContext1* m_context;
    //  �R�����X�e�[�g
    std::unique_ptr<DirectX::CommonStates>  m_commonStates;
    // �f�o�C�X���\�[�X
    std::unique_ptr<DX::DeviceResources> m_deviceResources;
    // �t���X�N���[��
    BOOL m_full_screen;

    // ���L���\�[�X
    CommonResources* m_commonResources;
    // ���̓}�l�[�W���[
    InputManager* m_inputManager;

    // �V�[��
    std::unique_ptr<Scene> m_scene;

    // �x�[�V�b�N�G�t�F�N�g
    std::unique_ptr<DirectX::BasicEffect> m_basicEffect;
    // �v���~�e�B�u�o�b�`
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;
    // ���̓��C�A�E�g
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

    //	�X�v���C�g�o�b�`
    std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
    //	�X�v���C�g�t�H���g
    std::unique_ptr<DirectX::SpriteFont>  m_spriteFont;
};
