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

	/* �G���[�R�[���o�b�N�֐� */
	static void user_error_callback_func(const CriChar8 *errid, CriUint32 p1, CriUint32 p2, CriUint32 *parray)
	{
		const CriChar8 *errmsg;

		/* �G���[������̕\�� */
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
	CriAtomExPlayerHn		player;		/* �Đ��v���[�� */
	CriAtomExVoicePoolHn	voice_pool;	/* �{�C�X�v�[�� */
	CriAtomExAcbHn			acb_hn;		/* ACB�n���h��(�����f�[�^) */
	CriAtomDbasId			dbas_id;	/* D-BAS�̍쐬*/

	CriAtomExPlaybackId		playback_id;	/* Voice�L���[�̃v���C�o�b�NID(�Đ��J�n���ɕێ�����) */

	int noteNo = 0;
	int octNo = 4;
	int lenNo = 4;
	int volNo = 15;
	int tempo = 120;

};

