/**
 * @file	timerevent.h
 * @brief	タイマ イベント クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

/**
 * @brief タイマ イベント クラス
 */
class CTimerEvent
{
public:
	CTimerEvent();
	virtual ~CTimerEvent();
	bool SetTimerEvent(UINT nDelay, bool bOneShot = false);
	void KillTimerEvent();

protected:
	/**
	 * タイマ イベントの時に呼ばれる
	 */
	virtual void OnTimerEvent() = 0;

private:
#if (_WIN32_WINNT > 0x0500)
	HANDLE m_hTimer;				/*!< タイマ ハンドル */
	static void CALLBACK OnQueueTimer(PVOID lpParameter, BOOLEAN bTimerOrWaitFired);
#endif	// (_WIN32_WINNT > 0x0500)

	UINT m_nTimerID;				/*!< マルチメディア タイマのID */
	static void CALLBACK OnMultimediaTimer(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);
};
