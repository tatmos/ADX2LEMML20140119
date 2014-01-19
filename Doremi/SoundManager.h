#pragma once
#include <cri_adx2le_pc.h>
#include <iostream>
#include <string>
#include <iterator>
#include <vector>
#include <cctype>

using namespace std;

class SoundManager
{
public:
	SoundManager();
	~SoundManager();

	void Play(string &mml);

private:

	/* エラーコールバック関数 */
	static void user_error_callback_func(const CriChar8 *errid, CriUint32 p1, CriUint32 p2, CriUint32 *parray)
	{
		const CriChar8 *errmsg;

		/* エラー文字列の表示 */
		errmsg = criErr_ConvertIdToMessage(errid, p1, p2);
		cout << ("%s\n", errmsg);

		return;
	}

	static void *user_alloc_func(void *obj, CriUint32 size)
	{
		void *ptr;
		ptr = malloc(size);
		return ptr;
	}

	static void user_free_func(void *obj, void *ptr)
	{
		free(ptr);
	}

private:
	CriAtomExPlayerHn		player;		/* 再生プレーヤ */
	CriAtomExVoicePoolHn	voice_pool;	/* ボイスプール */
	CriAtomExAcbHn			acb_hn;		/* ACBハンドル(音声データ) */
	CriAtomDbasId			dbas_id;	/* D-BASの作成*/

	CriAtomExPlaybackId		playback_id;	/* VoiceキューのプレイバックID(再生開始時に保持する) */

	int noteNo = 0;
	int octNo = 4;
	int lenNo = 4;
	int volNo = 15;
	int tempo = 120;

};

