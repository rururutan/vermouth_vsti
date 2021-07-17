/**
 * @file	timerevent.h
 * @brief	�^�C�} �C�x���g �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/**
 * @brief �^�C�} �C�x���g �N���X
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
	 * �^�C�} �C�x���g�̎��ɌĂ΂��
	 */
	virtual void OnTimerEvent() = 0;

private:
#if (_WIN32_WINNT > 0x0500)
	HANDLE m_hTimer;				/*!< �^�C�} �n���h�� */
	static void CALLBACK OnQueueTimer(PVOID lpParameter, BOOLEAN bTimerOrWaitFired);
#endif	// (_WIN32_WINNT > 0x0500)

	UINT m_nTimerID;				/*!< �}���`���f�B�A �^�C�}��ID */
	static void CALLBACK OnMultimediaTimer(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);
};
