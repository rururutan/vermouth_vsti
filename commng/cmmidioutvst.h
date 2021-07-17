/**
 * @file	cmmidioutvst.h
 * @brief	MIDI OUT VST �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "vsthost\vstbuffer.h"
#include "vsthost\vsteditwnd.h"
#include "vsthost\vsteffect.h"
#include "vsthost\vstmidievent.h"

/**
 * @brief MIDI OUT VST �N���X
 */
class CComMidiOutVst
{
public:
	CComMidiOutVst();
	~CComMidiOutVst();
	bool Initialize(UINT nRate);
	bool Initialize(LPCTSTR lpPath, UINT nRate);
	void SetVolume(float fVol);
	void Short(UINT32 nMessage);
	void Long(const UINT8* lpMessage, UINT cbMessage);
	void Process16(SINT16* lpBuffer, UINT nBufferCount);
	void Process32(SINT32* lpBuffer, UINT nBufferCount);

private:
	UINT m_nBlockSize;			/*!< �u���b�N �T�C�Y */
	UINT m_nIndex;				/*!< �ǂݎ��C���f�b�N�X */
	CVstEffect m_effect;		/*!< �G�t�F�N�g */
	CVstEditWnd m_wnd;			/*!< �E�B���h�E */
	CVstMidiEvent m_event;		/*!< �C�x���g */
	CVstBuffer m_input;			/*!< ���̓o�b�t�@ */
	CVstBuffer m_output;		/*!< �o�̓o�b�t�@ */
	float m_fVol;				/*!< ���H�����[�� */
};
