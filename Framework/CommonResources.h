/**********************************************
 *
 *  CommonResources.h
 *  �e���\�[�X�̃��b�v�N���X�i�V���O���g���j
 *
 *  ����ҁF�����x��
 * 
 *
 **********************************************/
#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "CommonStates.h"
#include "StepTimer.h"
#include "InputManager.h"

class CommonResources
{
public:
	// �ݒ�@�擾
	// �f�o�C�X���\�[�X
	void SetDeviceResources(DX::DeviceResources* deviceResources) { m_deviceResources = deviceResources; }
	DX::DeviceResources* GetDeviceResources() { return m_deviceResources; }
	//	�^�C�}�[
	void SetStepTimer(DX::StepTimer* stepTimer) { m_stepTimer = stepTimer; }
	DX::StepTimer* GetStepTimer() { return m_stepTimer; }
	//	�R�����X�e�[�g
	void SetCommonStates(DirectX::CommonStates* commonStates) { m_commonStates = commonStates; }
	DirectX::CommonStates* GetCommonStates() { return m_commonStates; }

	// �X�N���[���T�C�Y
	void GetScreenSize(int& width, int& height) const { width = m_screenW; height = m_screenH; }
	void SetScreenSize(const int& width, const int& height) { m_screenW = width; m_screenH = height; }
	// �r���[�s��
	void SetViewMatrix(const DirectX::SimpleMath::Matrix& view) { m_view = view; }
	const DirectX::SimpleMath::Matrix& GetViewMatrix() { return m_view; };
	// �ˉe�s��
	void SetProjectionMatrix(const DirectX::SimpleMath::Matrix& projection) { m_projection = projection; }
	const DirectX::SimpleMath::Matrix& GetProjectionMatrix() { return m_projection; };

private:
	//	�R���X�g���N�^
	CommonResources() :
		m_deviceResources(nullptr),
		m_stepTimer(nullptr),
		m_commonStates(nullptr),
		m_screenW(0),
		m_screenH(0),
		m_view{},
		m_projection{}
	{};
	//	�f�X�g���N�^
	~CommonResources() = default;

public:
	CommonResources(const CommonResources&) = delete;             // �R�s�[�֎~
	CommonResources& operator=(const CommonResources&) = delete;  // �R�s�[����֎~
	CommonResources(const CommonResources&&) = delete;            // ���[�u�֎~
	CommonResources& operator=(const CommonResources&&) = delete; // ���[�u����֎~

	//	�V���O���g���C���X�^���X�̎擾
	static CommonResources* GetInstance()
	{
		static CommonResources instance;
		return &instance;
	}

private:

	// �f�o�C�X���\�[�X
	DX::DeviceResources* m_deviceResources;
	// �^�C�}�[
	DX::StepTimer* m_stepTimer;
	// �R�����X�e�[�g
	DirectX::CommonStates* m_commonStates;
	
	// �X�N���[���T�C�Y
	int m_screenW, m_screenH;

	// �r���[�s��
	DirectX::SimpleMath::Matrix m_view;
	// �ˉe�s��
	DirectX::SimpleMath::Matrix m_projection;
};