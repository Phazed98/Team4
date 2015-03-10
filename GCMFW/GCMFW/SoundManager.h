#include "SoundSystem.h"
#include <iostream>
#include <time.h>
using namespace std;

class SoundManager{
public:
	static void GenerateSoundplayer();
	static CellSSPlayerConfig sspConfigStereo;
	static void PlaySound(int soundname, int loopcontrol);
	CellAANHandle GetStereo();
	//void EndSound();
	void TestPlay();

	static void UpdateSound();

	static int ChooseAAN();

	static void UseAAN(CellAANHandle temp,int soundname, int loopcontrol);

	static int err;
	static CellAANHandle stereo_player,stereo_player0,stereo_player1,stereo_player2,stereo_player3,stereo_player4,stereo_player5;
	static time_t rawtime,rawtime0,rawtime1,rawtime2,rawtime3,rawtime4,rawtime5;
	static struct tm* timeinfo;
	static struct tm* timeinfo0;
	static struct tm* timeinfo1;
	static struct tm* timeinfo2;
	static struct tm* timeinfo3;
	static struct tm* timeinfo4;
	static struct tm* timeinfo5;
	static int LoopInfo,LoopInfo0,LoopInfo1,LoopInfo2,LoopInfo3,LoopInfo4,LoopInfo5;
	static int minute,minute0,minute1,minute2,minute3,minute4,minute5;
	static int second,second0,second1,second2,second3,second4,second5;
	static CellSSPlayerWaveParam waveInfo;
	static CellSSPlayerCommonParam loopIno;
	static CellSSPlayerRuntimeInfo playbackInfo;
};