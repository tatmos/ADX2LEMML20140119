#include "SoundManager.h"

/* ACF/ACB�̃}�N����`�w�b�_ */
#include "Public/CueSheet_0.h"
#include "Public/Doremi_acf.h"

/* �f�[�^�f�B���N�g���ւ̃p�X */
#define PATH	"Public/"

/* �T���v���Ŏg�p����t�@�C���� */
#define ACF_FILE			"Doremi.acf"
#define ACB_FILE			"CueSheet_0.acb"
#define AWB_FILE			"CueSheet_0.awb"

/* �ő�{�C�X���𑝂₷���߂̊֘A�p�����[�^ */
#define MAX_VOICE			(24)
#define MAX_VIRTUAL_VOICE	(MAX_VOICE + 8)		/* �o�[�`�����{�C�X�͑��� */
#define MAX_CRIFS_LOADER	(MAX_VOICE + 8)		/* �ǂݍ��ݐ������߂� */

/* �ő�T���v�����O���[�g�i�s�b�`�ύX�܂ށj */
#define MAX_SAMPLING_RATE	(48000*4)

SoundManager::SoundManager()
{
	/* �G���[�R�[���o�b�N�֐��̓o�^ */
	criErr_SetCallback(user_error_callback_func);

	/* �������A���P�[�^�̓o�^ */
	criAtomEx_SetUserAllocator(user_alloc_func, user_free_func, NULL);

	/* ���C�u�����̏������i�ő�{�C�X���ύX�j */
	CriAtomExConfig_PC lib_config;
	CriFsConfig fs_config;
	criAtomEx_SetDefaultConfig_PC(&lib_config);
	criFs_SetDefaultConfig(&fs_config);
	lib_config.atom_ex.max_virtual_voices = MAX_VIRTUAL_VOICE;
	fs_config.num_loaders = MAX_CRIFS_LOADER;
	lib_config.atom_ex.fs_config = &fs_config;
	criAtomEx_Initialize_PC(&lib_config, NULL, 0);

	/* D-Bas�̍쐬�i�ő�X�g���[�����͂����Ō��܂�܂��j */
	dbas_id = criAtomDbas_Create(NULL, NULL, 0);

	/* ACF�t�@�C���̓ǂݍ��݂Ɠo�^ */
	criAtomEx_RegisterAcfFile(NULL, PATH ACF_FILE, NULL, 0);

	/* DSP�ݒ�̃A�^�b�` */
	criAtomEx_AttachDspBusSetting("DspBusSetting_0", NULL, 0);

	/* �{�C�X�v�[���̍쐬�i�ő�{�C�X���ύX�^�ő�s�b�`�ύX�^�X�g���[���Đ��Ή��j */
	CriAtomExStandardVoicePoolConfig vpool_config;
	criAtomExVoicePool_SetDefaultConfigForStandardVoicePool(&vpool_config);
	vpool_config.num_voices = MAX_VOICE;
	vpool_config.player_config.max_sampling_rate = MAX_SAMPLING_RATE;
	vpool_config.player_config.streaming_flag = CRI_TRUE;
	voice_pool = criAtomExVoicePool_AllocateStandardVoicePool(&vpool_config, NULL, 0);

	/* ACB�t�@�C����ǂݍ��݁AACB�n���h�����쐬 */
	acb_hn = criAtomExAcb_LoadAcbFile(NULL, PATH ACB_FILE, NULL, PATH AWB_FILE, NULL, 0);

	/* �v���[���̍쐬 */
	player = criAtomExPlayer_Create(NULL, NULL, 0);

	criAtomExPlayer_SetCueId(player, acb_hn, 1);

}


SoundManager::~SoundManager()
{
	/* DSP�̃f�^�b�` */
	criAtomEx_DetachDspBusSetting();

	/* �v���[���n���h���̔j�� */
	criAtomExPlayer_Destroy(player);

	/* �{�C�X�v�[���̔j�� */
	criAtomExVoicePool_Free(voice_pool);

	/* ACB�n���h���̔j�� */
	criAtomExAcb_Release(acb_hn);

	/* ACF�̓o�^���� */
	criAtomEx_UnregisterAcf();

	/* D-BAS�̔j�� */
	criAtomDbas_Destroy(dbas_id);

	/* ���C�u�����̏I�� */
	criAtomEx_Finalize_PC();
}

enum MMLType{
	none,
	note,
	len,
	rest,
	and,
	tempo,
	period,
	octave,
	neiro,
	volume,
};

class Note
{
public:
	int lenNo = 0;
	int number = 0;
	MMLType mmlType = MMLType::none;
	int noteNo = 0;
	int octNo = 4;
	int volNo = 15;
};

bool IsInteger(const string &str)
{
	if (str.find_first_not_of("0123456789") != string::npos) {
		return false;
	}

	return true;
}


void SoundManager::Play(string &mml)
{
	string::iterator it = mml.begin();

	vector<Note> noteList;


	Note* newNote = NULL;
	MMLType mmlType = MMLType::none;



	while (it != mml.end())
	{
		string numberStr = "";
		int number = 0;

		string tmp;
		std::copy(it, it + 1, std::inserter(tmp, tmp.begin()));

		bool noteFlag = false;

		if (IsInteger(tmp))
		{
			numberStr = "";
			while (it != mml.end())
			{
				numberStr += tmp;
				it++;
				if (it == mml.end())break;
				tmp = "";
				std::copy(it, it + 1, std::inserter(tmp, tmp.begin()));
				if (IsInteger(tmp) == false){ it--; break; }
			}
			number = stoi(numberStr);

			switch (mmlType)
			{
			case MMLType::note:
			case MMLType::rest:
				if (newNote != NULL){
					newNote->lenNo = number;
				}break;
			case MMLType::len:lenNo = number; break;
			case MMLType::octave:octNo = number; break;
			case MMLType::volume:
				if (newNote != NULL){
					newNote->volNo = number;
					volNo = number;
				}break;
			case MMLType::tempo:
				if (newNote != NULL){
					newNote->number = number;
				}break;
			default:
				if (newNote != NULL){
					newNote->number = number;
				}break;
			}

		}
		else if (mblen(&((tmp.c_str())[0]), MB_CUR_MAX) != 1)
		{
			//cout << "!!" << tmp << endl;
			if (it == mml.end())break;
			it++;
		}
		else {

			if (tmp == "c"){ noteNo = 0; noteFlag = true; mmlType = MMLType::note; }
			else if (tmp == "d"){ noteNo = 2; noteFlag = true; mmlType = MMLType::note; }
			else if (tmp == "e"){ noteNo = 4; noteFlag = true; mmlType = MMLType::note; }
			else if (tmp == "f"){ noteNo = 5; noteFlag = true; mmlType = MMLType::note; }
			else if (tmp == "g"){ noteNo = 7; noteFlag = true; mmlType = MMLType::note; }
			else if (tmp == "a"){ noteNo = 9; noteFlag = true; mmlType = MMLType::note; }
			else if (tmp == "b"){ noteNo = 11; noteFlag = true; mmlType = MMLType::note; }
			else if (tmp == "r"){ noteNo = -255; noteFlag = true; mmlType = MMLType::rest; }
			else if (tmp == "&" || tmp == "^"){ noteNo = -254; noteFlag = true; mmlType = MMLType::and; }
			else if (tmp == "@"){ noteNo = -253; noteFlag = true; mmlType = MMLType::neiro; }
			else if (tmp == "o"){ noteNo = -252; noteFlag = true; mmlType = MMLType::octave; }
			else if (tmp == "l"){ noteNo = -251; noteFlag = true; mmlType = MMLType::len; }
			else if (tmp == "t"){ noteNo = -250; noteFlag = true; mmlType = MMLType::tempo; }
			else if (tmp == "."){ noteNo = -249; noteFlag = true; mmlType = MMLType::period; }
			else if (tmp == "+")noteNo++;
			else if (tmp == "-")noteNo--;
			else if (tmp == ">")octNo++;
			else if (tmp == "<")octNo--;
			else if (tmp == "v"){ noteNo = -248; noteFlag = true; mmlType = MMLType::volume; }
			else { mmlType = MMLType::none; }

			if (noteFlag){
				newNote = new Note();
				noteList.push_back(*newNote);
				newNote = &(noteList[noteList.size() - 1]);
				newNote->octNo = octNo;
				newNote->lenNo = lenNo;
				newNote->volNo = volNo;
				newNote->mmlType = mmlType;
			}
		}
		if (newNote != NULL){
			newNote->noteNo = noteNo;
		}

		if (it == mml.end())break;
		it++;
	}

	{
		bool andFlag = false;
		float deltaLength = 0;

		vector<Note>::iterator it = noteList.begin();
		while (it != noteList.end())
		{
			switch ((*it).mmlType)
			{
			case MMLType::rest:criAtomExPlayer_Stop(player); break;
			case MMLType::note:
			{
				if (andFlag == false){
					criAtomExPlayer_Stop(player);
				}

				float pitch = (float)((((*it).octNo - 4) * 12) + (*it).noteNo) * 100 + 300;
				cout << pitch << " ";
				criAtomExPlayer_SetPitch(player, pitch);
				if (andFlag == false){
					playback_id = criAtomExPlayer_Start(player);
				}
				else {
					criAtomExPlayer_UpdateAll(player);
					andFlag = false;
				}

				deltaLength = (float)((*it).lenNo);
			} break;
			case MMLType::neiro:criAtomExPlayer_SetCueId(player, acb_hn, (*it).number); deltaLength = 0; break;
			case MMLType::tempo:tempo = (*it).number; deltaLength = 0; break;
			case MMLType::octave:deltaLength = 0; break;
			case MMLType::period: deltaLength = (float)deltaLength * 2; break;
			case MMLType::and:deltaLength = 0; andFlag = true; break;
			case MMLType::volume:{
									 criAtomExPlayer_SetVolume(player, ((float)(*it).volNo / 15));
									 deltaLength = 0;
			} break;
			default:deltaLength = 0; break;
			}

			if (deltaLength != 0 && tempo != 0){
				float sleepTime = (500 * ((float)120 / tempo)) * ((float)4 / deltaLength);
				cout << sleepTime << endl;
				Sleep((int)sleepTime);
			}
			else {
				Sleep(0);
			}
			++it;
		}

		cout << "end" << endl;
		criAtomExPlayer_Stop(player);
	}




}