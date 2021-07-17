/**
 * @file	cmmidioutvst.cpp
 * @brief	MIDI OUT VST �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "cmmidioutvst.h"
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

/**
 * �R���X�g���N�^
 */
CComMidiOutVst::CComMidiOutVst()
	: m_nBlockSize(128)
	, m_nIndex(0)
	, m_fVol(32767.0f)
{
}

/**
 * �f�X�g���N�^
 */
CComMidiOutVst::~CComMidiOutVst()
{
	m_wnd.Destroy();
	m_effect.close();
	m_effect.Unload();
}

/**
 * ������
 * @param[in] nRate �T���v�����O ���[�g
 * @retval true ����
 * @retval false ���s
 */
bool CComMidiOutVst::Initialize(UINT nRate)
{
	TCHAR szModule[MAX_PATH];
	::ExpandEnvironmentStrings(TEXT("%ProgramFiles%\\Roland\\Sound Canvas VA\\SOUND Canvas VA.dll"), szModule, _countof(szModule));
	return Initialize(szModule, nRate);
}

/**
 * ������
 * @param[in] lpPath �p�X
 * @param[in] nRate �T���v�����O ���[�g
 * @retval true ����
 * @retval false ���s
 */
bool CComMidiOutVst::Initialize(LPCTSTR lpPath, UINT nRate)
{
	m_effect;
	if (!m_effect.Load(lpPath))
	{
		printf("Cloudn't attach VSTi.\n");
		return false;
	}

	// Effect ���I�[�v��
	m_effect.open();

	// �T���v�����O ���[�g��ݒ�
	m_effect.setSampleRate(static_cast<float>(nRate));

	// �u���b�N�T�C�Y��ݒ�
	m_effect.setBlockSize(m_nBlockSize);
	m_effect.resume();

	m_effect.beginSetProgram();
	m_effect.setProgram(0);
	m_effect.endSetProgram();

	m_input.Alloc(2, m_nBlockSize);
	m_output.Alloc(2, m_nBlockSize);

	m_wnd.Create(&m_effect, TEXT("VSTi"), WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX);

	return true;
}

/**
 * ���H�����[���ݒ�
 * @param[in] fVol ���H�����[��
 */
void CComMidiOutVst::SetVolume(float fVol)
{
	m_fVol = fVol;
}

/**
 * �V���[�g ���b�Z�[�W
 * @param[in] nMessage ���b�Z�[�W
 */
void CComMidiOutVst::Short(UINT32 nMessage)
{
	m_event.ShortMessage(m_nIndex, nMessage);
}

/**
 * �����O ���b�Z�[�W
 * @param[in] lpMessage ���b�Z�[�W �|�C���^
 * @param[in] cbMessage ���b�Z�[�W �T�C�Y
 */
void CComMidiOutVst::Long(const UINT8* lpMessage, UINT cbMessage)
{
	m_event.LongMessage(m_nIndex, lpMessage, cbMessage);
}

/**
 * �v���Z�X (16bit)
 * @param[out] lpBuffer �o�b�t�@
 * @param[in] nBufferCount �T���v����
 */
void CComMidiOutVst::Process16(SINT16* lpBuffer, UINT nBufferCount)
{
	while (nBufferCount)
	{
		if (m_nIndex >= m_nBlockSize)
		{
			m_nIndex = 0;
			m_effect.processEvents(m_event.GetEvents());
			m_effect.processReplacing(m_input.GetBuffer(), m_output.GetBuffer(), m_nBlockSize);
			m_event.Clear();
		}

		UINT nSize = m_nBlockSize - m_nIndex;
		nSize = min(nSize, nBufferCount);
		nBufferCount -= nSize;
		float** output = m_output.GetBuffer();
		do
		{
			float l = output[0][m_nIndex];
			l = min(l, 1.0f);
			l = max(l, -1.0f);
			lpBuffer[0] = static_cast<SINT16>(l * m_fVol - 0.5f);

			float r = output[1][m_nIndex];
			r = min(l, 1.0f);
			r = max(l, -1.0f);
			lpBuffer[1] = static_cast<SINT16>(r * m_fVol - 0.5f);
			lpBuffer += 2;
			m_nIndex++;
		} while (--nSize);
	}
}

/**
 * �v���Z�X (32bit)
 * @param[out] lpBuffer �o�b�t�@
 * @param[in] nBufferCount �T���v����
 */
void CComMidiOutVst::Process32(SINT32* lpBuffer, UINT nBufferCount)
{
	while (nBufferCount)
	{
		if (m_nIndex >= m_nBlockSize)
		{
			m_nIndex = 0;
			m_effect.processEvents(m_event.GetEvents());
			m_effect.processReplacing(m_input.GetBuffer(), m_output.GetBuffer(), m_nBlockSize);
			m_event.Clear();
		}

		UINT nSize = m_nBlockSize - m_nIndex;
		nSize = min(nSize, nBufferCount);
		nBufferCount -= nSize;
		float** output = m_output.GetBuffer();
		do
		{
			lpBuffer[0] = static_cast<SINT32>(output[0][m_nIndex] * m_fVol - 0.5f);
			lpBuffer[1] = static_cast<SINT32>(output[1][m_nIndex] * m_fVol - 0.5f);
			lpBuffer += 2;
			m_nIndex++;
		} while (--nSize);
	}
}
