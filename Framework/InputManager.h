// ============================================
// 
// �t�@�C����: InputManager.h
// �T�v: InputManager.cpp�̃w�b�_�[�t�@�C��
// 
// ����� : �����x��
// 
// ============================================
#pragma once
#include <Mouse.h>
#include <Keyboard.h>
#include <GamePad.h>

enum class KeyType : int
{
	ON_KEY,
	ON_KEY_DOWN,
	ON_KEY_UP,
};

class InputManager
{
public:
	using Keys = DirectX::Keyboard::Keys;

	enum class MouseButtons
	{
		LEFT,		//	��
		RIGHT,		//	�E
		MIDDLE,		//	�z�C�[��
	};
	
public:
	// �}�E�X�̏�Ԃ�ύX
	void SetMouseMode(DirectX::Mouse::Mode mouse_mode) { m_mouse->SetMode(mouse_mode); }
	// �}�E�X���擾����
	DirectX::Mouse* GetMouse() const { return m_mouse.get(); }
	// �}�E�X�̃{�^���X�e�[�g�g���b�J�[���擾����
	DirectX::Mouse::ButtonStateTracker* GetMouseTracker() const { return m_mouseStateTracker.get(); }

	// �L�[�{�[�h���擾����
	DirectX::Keyboard* GetKeyboard() const {return m_keyboard.get(); }
	// �L�[�{�[�h�X�e�[�g�g���b�J�[���擾����
	DirectX::Keyboard::KeyboardStateTracker* GetKeyboardStateTracker() const { return m_keyboardStateTracker.get(); }

	// �Q�[���p�b�h�擾����
	DirectX::GamePad* GetGamePad() const { return m_gamepad.get(); }
	// �Q�[���p�b�g�X�e�[�g�g���b�J�[���擾����
	DirectX::GamePad::ButtonStateTracker* GetButtonStateTracker() const { return m_gamePadStateTracker.get(); }

private:
	//	�R���X�g���N�^
	InputManager();
	//	�f�X�g���N�^
	~InputManager() = default;
public:
	InputManager(const InputManager&) = delete;             // �R�s�[�֎~
	InputManager& operator=(const InputManager&) = delete;  // �R�s�[����֎~
	InputManager(const InputManager&&) = delete;            // ���[�u�֎~
	InputManager& operator=(const InputManager&&) = delete; // ���[�u����֎~

	//	�V���O���g���C���X�^���X�̎擾
	static InputManager* GetInstance()
	{
		static InputManager instance;
		return &instance;
	}
public:
	// �X�V����
	void Update();

	// �L�[����
	bool OnKey(Keys key);     // �L�[�����݉�����Ă��邩�𔻒肷��
	bool OnKeyDown(Keys key); // �L�[�������ꂽ�u�Ԃ𔻒肷��
	bool OnKeyUp(Keys key);   // �L�[�������ꂽ�u�Ԃ𔻒肷��
	// �}�E�X����
	bool OnMouseButton(MouseButtons mouseButton);     // �}�E�X�{�^�������݉�����Ă��邩�𔻒肷��
	bool OnMouseButtonDown(MouseButtons mouseButton); // �}�E�X�{�^���������ꂽ�u�Ԃ𔻒肷��
	bool OnMouseButtonUp(MouseButtons mouseButton);   // �}�E�X�{�^���������ꂽ�u�Ԃ𔻒肷��


private:
	// �}�E�X
	std::unique_ptr<DirectX::Mouse> m_mouse;
	std::unique_ptr<DirectX::Mouse::ButtonStateTracker> m_mouseStateTracker;
	// �L�[�{�[�h
	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Keyboard::KeyboardStateTracker> m_keyboardStateTracker;
	// �p�b�h
	std::unique_ptr<DirectX::GamePad> m_gamepad;
	std::unique_ptr<DirectX::GamePad::ButtonStateTracker> m_gamePadStateTracker;
};