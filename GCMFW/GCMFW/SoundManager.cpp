#include "SoundManager.h"

#define STEREO_DATA0 "/app_home/36847__ecodtr__laserrocket2.wav"
#define STEREO_DATA1 "/app_home/14615__man__canon.wav"

CellSSPlayerConfig SoundManager::sspConfigStereo = {2, 0};
int SoundManager::err = NULL;
int SoundManager::minute = NULL;
int SoundManager::minute0 = NULL;
int SoundManager::minute1 = NULL;
int SoundManager::minute2 = NULL;
int SoundManager::minute3 = NULL;
int SoundManager::minute4 = NULL;
int SoundManager::minute5 = NULL;
int SoundManager::second = NULL;
int SoundManager::second0 = NULL;
int SoundManager::second1 = NULL;
int SoundManager::second2 = NULL;
int SoundManager::second3 = NULL;
int SoundManager::second4 = NULL;
int SoundManager::second5 = NULL;
int SoundManager::LoopInfo = NULL;
int SoundManager::LoopInfo0 = NULL;
int SoundManager::LoopInfo1 = NULL;
int SoundManager::LoopInfo2 = NULL;
int SoundManager::LoopInfo3 = NULL;
int SoundManager::LoopInfo4 = NULL;
int SoundManager::LoopInfo5 = NULL;
CellAANHandle SoundManager::stereo_player = NULL;
CellAANHandle SoundManager::stereo_player0 = NULL;
CellAANHandle SoundManager::stereo_player1 = NULL;
CellAANHandle SoundManager::stereo_player2 = NULL;
CellAANHandle SoundManager::stereo_player3 = NULL;
CellAANHandle SoundManager::stereo_player4 = NULL;
CellAANHandle SoundManager::stereo_player5 = NULL;
time_t SoundManager::rawtime = NULL;
time_t SoundManager::rawtime0 = NULL;
time_t SoundManager::rawtime1 = NULL;
time_t SoundManager::rawtime2 = NULL;
time_t SoundManager::rawtime3 = NULL;
time_t SoundManager::rawtime4 = NULL;
time_t SoundManager::rawtime5 = NULL;
struct tm* SoundManager::timeinfo = NULL;
struct tm* SoundManager::timeinfo0 = NULL;
struct tm* SoundManager::timeinfo1 = NULL;
struct tm* SoundManager::timeinfo2 = NULL;
struct tm* SoundManager::timeinfo3 = NULL;
struct tm* SoundManager::timeinfo4 = NULL;
struct tm* SoundManager::timeinfo5 = NULL;
CellSSPlayerWaveParam SoundManager::waveInfo;
CellSSPlayerCommonParam SoundManager::loopIno;
CellSSPlayerRuntimeInfo SoundManager::playbackInfo;

void SoundManager::GenerateSoundplayer(){
}

void SoundManager::PlaySound(int soundname, int loopcontrol){
	switch(ChooseAAN()){
	case 0: UseAAN(stereo_player0,soundname,loopcontrol);
		break;
	case 1: UseAAN(stereo_player1,soundname,loopcontrol);
		break;
	case 2: UseAAN(stereo_player2,soundname,loopcontrol);
		break;
	case 3: UseAAN(stereo_player3,soundname,loopcontrol);
		break;
	case 4: UseAAN(stereo_player4,soundname,loopcontrol);
		break;
	case 5: UseAAN(stereo_player5,soundname,loopcontrol);
		break;

	}
}

CellAANHandle SoundManager::GetStereo(){
	while(1){
		if(CELL_SSPLAYER_STATE_OFF==cellSSPlayerGetState(stereo_player0)){
			return stereo_player0;
		}
	}
}

void SoundManager::TestPlay(){
	//cellSSPlayerPlay(stereo_player, &playbackInfo);
	//stereo_player = NULL;
	cellAANDisconnect(
		SoundSystem::mixerHandle, SoundSystem::strip_0_port_0, stereo_player0, 0
		);
}

void SoundManager::UpdateSound(){
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	minute = timeinfo->tm_min;
	second = timeinfo->tm_sec;

	if(timeinfo0!=NULL && LoopInfo0 ==0){
		if((second - second0) + (minute - minute0)*60 >= 3){
			cellAANDisconnect(SoundSystem::mixerHandle, SoundSystem::strip_0_port_0, stereo_player0, 0);
			timeinfo0 = NULL;
			rawtime0 = NULL;
		}
	}

	if(timeinfo1!=NULL && LoopInfo1 ==0){
		if((second - second1) + (minute - minute1)*60 >= 3){
			cellAANDisconnect(SoundSystem::mixerHandle, SoundSystem::strip_0_port_0, stereo_player1, 0);
			timeinfo1 = NULL;
			rawtime1 = NULL;
		}
	}

	if(timeinfo2!=NULL && LoopInfo2 ==0){
		if((second - second2) + (minute - minute2)*60 >= 3){
			cellAANDisconnect(SoundSystem::mixerHandle, SoundSystem::strip_0_port_0, stereo_player2, 0);
			timeinfo2 = NULL;
			rawtime2 = NULL;
		}
	}

	if(timeinfo3!=NULL && LoopInfo3 ==0){
		if((second - second3) + (minute - minute3)*60 >= 3){
			cellAANDisconnect(SoundSystem::mixerHandle, SoundSystem::strip_0_port_0, stereo_player3, 0);
			timeinfo3 = NULL;
			rawtime3 = NULL;
		}
	}

	if(timeinfo4!=NULL && LoopInfo4 ==0){
		if((second - second4) + (minute - minute4)*60 >= 3){
			cellAANDisconnect(SoundSystem::mixerHandle, SoundSystem::strip_0_port_0, stereo_player4, 0);
			timeinfo4 = NULL;
			rawtime4 = NULL;
		}
	}

	if(timeinfo5!=NULL && LoopInfo5 ==0){
		if((second - second5) + (minute - minute5)*60 >= 3){
			cellAANDisconnect(SoundSystem::mixerHandle, SoundSystem::strip_0_port_0, stereo_player5, 0);
			timeinfo5 = NULL;
			rawtime5 = NULL;
		}
	}
}

int SoundManager::ChooseAAN(){
	if(timeinfo0 == NULL){
		time(&rawtime0);
		timeinfo0 = localtime(&rawtime0);
		minute0 = timeinfo0->tm_min;
		second0 = timeinfo0->tm_sec;
		return 0;
	}
	if(timeinfo1 == NULL){
		time(&rawtime1);
		timeinfo1 = localtime(&rawtime1);
		minute1 = timeinfo1->tm_min;
		second1 = timeinfo1->tm_sec;
		return 1;
	}
	if(timeinfo2 == NULL){
		time(&rawtime2);
		timeinfo2 = localtime(&rawtime2);
		minute2 = timeinfo2->tm_min;
		second2 = timeinfo2->tm_sec;
		return 2;
	}
	if(timeinfo3 == NULL){
		time(&rawtime3);
		timeinfo3 = localtime(&rawtime3);
		minute3 = timeinfo3->tm_min;
		second3 = timeinfo3->tm_sec;
		return 3;
	}
	if(timeinfo4 == NULL){
		time(&rawtime4);
		timeinfo4 = localtime(&rawtime4);
		minute4 = timeinfo4->tm_min;
		second4 = timeinfo4->tm_sec;
		return 4;
	}
	if(timeinfo5 == NULL){
		time(&rawtime5);
		timeinfo5 = localtime(&rawtime5);
		minute5 = timeinfo5->tm_min;
		second5 = timeinfo5->tm_sec;
		return 5;
	}
}

void SoundManager::UseAAN(CellAANHandle temp,int soundname, int loopcontrol){
	err = cellSSPlayerCreate(&temp, &sspConfigStereo);
		err = cellAANConnect(
			SoundSystem::mixerHandle, SoundSystem::strip_0_port_0, temp, 0
			);
		if(soundname == 0){
			err = Wave::readWavfile(STEREO_DATA0);
		}
		if(soundname == 1){
			err = Wave::readWavfile(STEREO_DATA1);
		}
		waveInfo.addr = (void*)Wave::wavetop;
		waveInfo.samples = Wave::waveByteSize / 4;
		waveInfo.loopStartOffset = 1;
		waveInfo.startOffset = 1;

		if(loopcontrol == 0){
			loopIno.loopMode = CELL_SSPLAYER_ONESHOT;
			LoopInfo0 = 0;
		}
		if(loopcontrol == 1){
			loopIno.loopMode = CELL_SSPLAYER_LOOP_ON;
			LoopInfo0 = 1;
		}
		loopIno.attackMode = 0;
		playbackInfo.level = 1.0;
		playbackInfo.speed = 1.0;



		err = cellSSPlayerSetWave(temp, &waveInfo, &loopIno);

		err = cellSurMixerStart();

		cellSSPlayerPlay(temp, &playbackInfo);
}