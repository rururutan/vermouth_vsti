/**
 * @file	cmmidioutvst.h
 * @brief	MIDI OUT VST クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "vsthost\vstbuffer.h"
#include "vsthost\vsteditwnd.h"
#include "vsthost\vsteffect.h"
#include "vsthost\vstmidievent.h"

/**
 * @brief MIDI OUT VST クラス
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
	UINT m_nBlockSize;			/*!< ブロック サイズ */
	UINT m_nIndex;				/*!< 読み取りインデックス */
	CVstEffect m_effect;		/*!< エフェクト */
	CVstEditWnd m_wnd;			/*!< ウィンドウ */
	CVstMidiEvent m_event;		/*!< イベント */
	CVstBuffer m_input;			/*!< 入力バッファ */
	CVstBuffer m_output;		/*!< 出力バッファ */
	float m_fVol;				/*!< ヴォリューム */
};
