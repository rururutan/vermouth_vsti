/**
 * @file	cmmidioutvst.cpp
 * @brief	MIDI OUT VST クラスの動作の定義を行います
 */

#include "compiler.h"
#include "cmmidioutvst.h"
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

/**
 * コンストラクタ
 */
CComMidiOutVst::CComMidiOutVst()
	: m_nBlockSize(128)
	, m_nIndex(0)
	, m_fVol(32767.0f)
{
}

/**
 * デストラクタ
 */
CComMidiOutVst::~CComMidiOutVst()
{
	m_wnd.Destroy();
	m_effect.close();
	m_effect.Unload();
}

/**
 * 初期化
 * @param[in] nRate サンプリング レート
 * @retval true 成功
 * @retval false 失敗
 */
bool CComMidiOutVst::Initialize(UINT nRate)
{
	TCHAR szModule[MAX_PATH];
	::ExpandEnvironmentStrings(TEXT("%ProgramFiles%\\Roland\\Sound Canvas VA\\SOUND Canvas VA.dll"), szModule, _countof(szModule));
	return Initialize(szModule, nRate);
}

/**
 * 初期化
 * @param[in] lpPath パス
 * @param[in] nRate サンプリング レート
 * @retval true 成功
 * @retval false 失敗
 */
bool CComMidiOutVst::Initialize(LPCTSTR lpPath, UINT nRate)
{
	m_effect;
	if (!m_effect.Load(lpPath))
	{
		printf("Cloudn't attach VSTi.\n");
		return false;
	}

	// Effect をオープン
	m_effect.open();

	// サンプリング レートを設定
	m_effect.setSampleRate(static_cast<float>(nRate));

	// ブロックサイズを設定
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
 * ヴォリューム設定
 * @param[in] fVol ヴォリューム
 */
void CComMidiOutVst::SetVolume(float fVol)
{
	m_fVol = fVol;
}

/**
 * ショート メッセージ
 * @param[in] nMessage メッセージ
 */
void CComMidiOutVst::Short(UINT32 nMessage)
{
	m_event.ShortMessage(m_nIndex, nMessage);
}

/**
 * ロング メッセージ
 * @param[in] lpMessage メッセージ ポインタ
 * @param[in] cbMessage メッセージ サイズ
 */
void CComMidiOutVst::Long(const UINT8* lpMessage, UINT cbMessage)
{
	m_event.LongMessage(m_nIndex, lpMessage, cbMessage);
}

/**
 * プロセス (16bit)
 * @param[out] lpBuffer バッファ
 * @param[in] nBufferCount サンプル数
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
 * プロセス (32bit)
 * @param[out] lpBuffer バッファ
 * @param[in] nBufferCount サンプル数
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
