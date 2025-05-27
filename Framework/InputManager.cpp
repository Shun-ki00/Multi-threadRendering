// ============================================
// 
// �t�@�C����: InputManager.cpp
// �T�v: ���͑S�ʂ̏������L�q�i�V���O���g���j
// 
// ����� : �����x��
// 
// ============================================
#include "pch.h"
#include "Framework/InputManager.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
InputManager::InputManager()
	:
	m_mouse{},
	m_mouseStateTracker{},
	m_keyboard{},
	m_keyboardStateTracker{},
	m_gamepad{},
	m_gamePadStateTracker{}
{
	// �}�E�X���g�p�ł����Ԃɂ���
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouseStateTracker = std::make_unique<DirectX::Mouse::ButtonStateTracker>();

	// �L�[�{�[�h���g�p�ł����Ԃɂ���
	m_keyboard = std::make_unique<DirectX::Keyboard>();
	m_keyboardStateTracker = std::make_unique<DirectX::Keyboard::KeyboardStateTracker>();

	// �Q�[���p�b�h���g�p�ł����Ԃɂ���
	m_gamepad = std::make_unique<DirectX::GamePad>();
	m_gamePadStateTracker = std::make_unique<DirectX::GamePad::ButtonStateTracker>();
}

/// <summary>
/// �X�V����
/// </summary>
void InputManager::Update()
{
	// �}�E�X�̏����X�V����
	DirectX::Mouse::State mouseState = m_mouse->GetState();
	m_mouseStateTracker->Update(mouseState);

	// �L�[�{�[�h�̏����X�V����
	DirectX::Keyboard::State keyboardState = m_keyboard->GetState();
	m_keyboardStateTracker->Update(keyboardState);

	// �Q�[���p�b�h�̏����X�V����
	DirectX::GamePad::State gamePadState = m_gamepad->GetState(0);
	m_gamePadStateTracker->Update(gamePadState);
}

/// <summary>
/// �L�[�����݉�����Ă��邩�𔻒肷��
/// </summary>
/// <param name="key">���肷��L�[</param>
/// <returns>�L�[��������Ă���� true�A����ȊO�� false</returns>
bool InputManager::OnKey(Keys key)
{
	return m_keyboard->GetState().IsKeyDown(key);
}
/// <summary>
/// �L�[�������ꂽ�u�Ԃ𔻒肷��
/// </summary>
/// <param name="key">���肷��L�[</param>
/// <returns>�L�[�������ꂽ�u�Ԃł���� true�A����ȊO�� false</returns>
bool InputManager::OnKeyDown(Keys key)
{
	return m_keyboardStateTracker->IsKeyPressed(key);
}
/// <summary>
/// �w�肳�ꂽ�L�[�������ꂽ�u�Ԃ𔻒肷��
/// </summary>
/// <param name="key">���肷��L�[</param>
/// <returns>�L�[�������ꂽ�u�Ԃł���� true�A����ȊO�� false</returns>
bool InputManager::OnKeyUp(Keys key)
{
	return m_keyboardStateTracker->IsKeyReleased(key);
}


/// <summary>
/// �}�E�X�{�^�������݉�����Ă��邩�𔻒肷��
/// </summary>
/// <param name="mouseButton">���肷��}�E�X�{�^��</param>
/// <returns>�{�^����������Ă���� true�A����ȊO�� false</returns>
bool InputManager::OnMouseButton(MouseButtons mouseButton)
{
	DirectX::Mouse::State state = m_mouse->GetState();

	switch (mouseButton)
	{
	case InputManager::MouseButtons::LEFT:
		return state.leftButton;
	case InputManager::MouseButtons::RIGHT:
		return state.rightButton;
	case InputManager::MouseButtons::MIDDLE:
		return state.middleButton;
	default:
		return false;
	}
}
/// <summary>
/// �}�E�X�{�^���������ꂽ�u�Ԃ𔻒肷��
/// </summary>
/// <param name="mouseButton">���肷��}�E�X�{�^��</param>
/// <returns>�{�^���������ꂽ�u�Ԃł���� true�A����ȊO�� false</returns>
bool InputManager::OnMouseButtonDown(MouseButtons mouseButton)
{
	DirectX::Mouse::ButtonStateTracker* state = m_mouseStateTracker.get();

	switch (mouseButton)
	{
	case InputManager::MouseButtons::LEFT:
		return state->leftButton == DirectX::Mouse::ButtonStateTracker::PRESSED;
	case InputManager::MouseButtons::RIGHT:
		return state->rightButton == DirectX::Mouse::ButtonStateTracker::PRESSED;
	case InputManager::MouseButtons::MIDDLE:
		return state->middleButton == DirectX::Mouse::ButtonStateTracker::PRESSED;
	default:
		return false;
	}
}
/// <summary>
/// �}�E�X�{�^���������ꂽ�u�Ԃ𔻒肷��
/// </summary>
/// <param name="mouseButton">���肷��}�E�X�{�^��</param>
/// <returns>�{�^���������ꂽ�u�Ԃł���� true�A����ȊO�� false</returns>
bool InputManager::OnMouseButtonUp(MouseButtons mouseButton)
{
	DirectX::Mouse::ButtonStateTracker* state = m_mouseStateTracker.get();

	switch (mouseButton)
	{
	case InputManager::MouseButtons::LEFT:
		return state->leftButton == DirectX::Mouse::ButtonStateTracker::RELEASED;
	case InputManager::MouseButtons::RIGHT:
		return state->rightButton == DirectX::Mouse::ButtonStateTracker::RELEASED;
	case InputManager::MouseButtons::MIDDLE:
		return state->middleButton == DirectX::Mouse::ButtonStateTracker::RELEASED;
	default:
		return false;
	}
}


