/**
 * @file	timerevent.cpp
 * @brief	タイマ イベント クラスの動作の定義を行います
 */

#include "compiler.h"
#include "timerevent.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

/**
 * コンストラクタ
 */
CTimerEvent::CTimerEvent()
	: m_nTimerID(0)
#if (_WIN32_WINNT > 0x0500)
	, m_hTimer(NULL)
#endif	// (_WIN32_WINNT > 0x0500)
{
}

/**
 * デストラクタ
 */
CTimerEvent::~CTimerEvent()
{
	KillTimerEvent();
}

/**
 * タイマイベント開始
 * @param[in] nDelay  間隔
 * @param[in] bOneShot ワンショット フラグ
 * @retval true 成功
 * @retval false 失敗
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
 * タイマイベント終了
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
 * コールバック
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
