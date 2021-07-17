/**
 * @file	timerevent.cpp
 * @brief	�^�C�} �C�x���g �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "timerevent.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

/**
 * �R���X�g���N�^
 */
CTimerEvent::CTimerEvent()
	: m_nTimerID(0)
#if (_WIN32_WINNT > 0x0500)
	, m_hTimer(NULL)
#endif	// (_WIN32_WINNT > 0x0500)
{
}

/**
 * �f�X�g���N�^
 */
CTimerEvent::~CTimerEvent()
{
	KillTimerEvent();
}

/**
 * �^�C�}�C�x���g�J�n
 * @param[in] nDelay  �Ԋu
 * @param[in] bOneShot �����V���b�g �t���O
 * @retval true ����
 * @retval false ���s
 */
bool CTimerEvent::SetTimerEvent(UINT nDelay, bool bOneShot)
{
	KillTimerEvent();

	bool bResult = false;

#if (_WIN32_WINNT > 0x0500)
	if (!bResult)
	{
		const UINT nPeriod = (bOneShot) ? 0 : nDelay;
		if (::CreateTimerQueueTimer(&m_hTimer, NULL, OnQueueTimer, static_cast<PVOID>(this), nDelay, nPeriod, WT_EXECUTEDEFAULT))
		{
			bResult = true;
		}
	}
#endif	// (_WIN32_WINNT > 0x0500)

	if (!bResult)
	{
		const UINT nEvent = (bOneShot) ? TIME_ONESHOT : TIME_PERIODIC;
		MMRESULT r = ::timeSetEvent(nDelay, 0, OnMultimediaTimer, reinterpret_cast<DWORD_PTR>(this), nEvent);
		if (r != NULL)
		{
			m_nTimerID = static_cast<UINT>(r);
			bResult = true;
		}
	}
	return bResult;
}

/**
 * �^�C�}�C�x���g�I��
 */
void CTimerEvent::KillTimerEvent()
{
#if (_WIN32_WINNT > 0x0500)
	if (m_hTimer)
	{
		::DeleteTimerQueueTimer(NULL, m_hTimer, NULL);
		m_hTimer = NULL;
	}
#endif	// (_WIN32_WINNT > 0x0500)

	if (m_nTimerID)
	{
		::timeKillEvent(m_nTimerID);
		m_nTimerID = 0;
	}
}

#if (_WIN32_WINNT > 0x0500)
/**
 * WaitOrTimerCallback callback function
 * @param[in] lpParameter A parameter of the CreateTimerQueueTimer
 * @param[in] bTimerOrWaitFired If this parameter is TRUE, the wait timed out (This parameter is always TRUE for timer callbacks.)
 */
void CALLBACK CTimerEvent::OnQueueTimer(PVOID lpParameter, BOOLEAN bTimerOrWaitFired)
{
	static_cast<CTimerEvent*>(lpParameter)->OnTimerEvent();
}
#endif	// (_WIN32_WINNT > 0x0500)

/**
 * �R�[���o�b�N
 * @param[in] uTimerID The identifier of the timer
 * @param[in] uMsg Reserved.
 * @param[in] dwUser The value that was specified for the dwUser parameter of the timeSetEvent function
 * @param[in] dw1 Reserved.
 * @param[in] dw2 Reserved.
 */
void CALLBACK CTimerEvent::OnMultimediaTimer(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	reinterpret_cast<CTimerEvent*>(dwUser)->OnTimerEvent();
}
