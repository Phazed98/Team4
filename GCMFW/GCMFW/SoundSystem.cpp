#include "SoundSystem.h"

unsigned int SoundSystem::strip_0_port_0;
CellAANHandle SoundSystem::mixerHandle;
int SoundSystem::err;
int SoundSystem::num;
int SoundSystem::ch_pcm;
int SoundSystem::ch_ac3;
int SoundSystem::ch_dts;
CellAudioOutConfiguration SoundSystem::aConfig;
CellAudioOutState SoundSystem::a_state;
const CellSurMixerConfig SoundSystem::g_SurMixerConfigS0 = {
	400, /* thread priority */
	0, /* the number of 1ch channel strip */
	1, /* the number of 2ch channel strip */
	0, /* the number of 6ch channel strip */
	0 /* the number of 8ch channel strip */
};


void SoundSystem::SystemAudioUtilitySet6ch(){
	//Load the Audio system module
	cellSysmoduleLoadModule(CELL_SYSMODULE_AUDIO);

	ch_pcm = cellAudioOutGetSoundAvailability(
		CELL_AUDIO_OUT_PRIMARY,
		CELL_AUDIO_OUT_CODING_TYPE_LPCM,
		CELL_AUDIO_OUT_FS_48KHZ, 0
		);
	// How many Dolby AC−3 channels available?
	ch_ac3 = cellAudioOutGetSoundAvailability(
		CELL_AUDIO_OUT_PRIMARY,
		CELL_AUDIO_OUT_CODING_TYPE_AC3,
		CELL_AUDIO_OUT_FS_48KHZ, 0
		);

	// How many Dolby DTS channels available?
	ch_dts = cellAudioOutGetSoundAvailability(
		CELL_AUDIO_OUT_PRIMARY,
		CELL_AUDIO_OUT_CODING_TYPE_DTS,
		CELL_AUDIO_OUT_FS_48KHZ, 0
		);

	// Chose an output mode −−−−−−−−−−−−−

	// The target structure must be zero cleared.
	memset(&aConfig, 0, sizeof(CellAudioOutConfiguration));

	if( ch_pcm >= 6 ) {
		// If 6ch of linear PCM is available, we use this.
		aConfig.channel = 6;
		aConfig.encoder=CELL_AUDIO_OUT_CODING_TYPE_LPCM;
		printf("Audio Encoder: PCM, Channels: 6\n");
	}
	else if( ch_ac3 >= 6 ) {
		// else if AC−3 is available, we use this.
		aConfig.channel = 6;
		aConfig.encoder = CELL_AUDIO_OUT_CODING_TYPE_AC3;
		printf("Audio Encoder: AC−3, Channels: 6\n");
	}
	else if( ch_dts >= 6 ) {
		// else if DTS is available, we use this.
		aConfig.channel = 6;
		aConfig.encoder = CELL_AUDIO_OUT_CODING_TYPE_DTS;
		printf("Audio Encoder: DTS, Channels: 6\n");
	}
	else {
		// else if neither is available, use 2ch of linear PCM.
		aConfig.channel = 2;
		aConfig.encoder=CELL_AUDIO_OUT_CODING_TYPE_LPCM;
		printf("Encoder: PCM, Channels: 2, Downmixed from 6\n");
		// We must then downmix the multiple channels to stereo.
		aConfig.downMixer =
			CELL_AUDIO_OUT_DOWNMIXER_TYPE_A;
	}

	// Apply the selected audio settings −−−−−−−−−−−−−

	// This may take some time if the audio system is busy
	// Keep looping until it works or there is an error.
	for(num=0; num< 400; num++ ) {
		err = cellAudioOutConfigure(
			CELL_AUDIO_OUT_PRIMARY, &aConfig, NULL, 0
			);
		if( err == CELL_OK ) {
			break; //Successs!
		}
		else if(err!=CELL_AUDIO_IN_ERROR_CONDITION_BUSY)
		{
			break;
		}
		sys_timer_usleep(5000);
	}

	// Check to see if the audio system is now ready. Loop like before.
	for(num=0; num< 400; num++ ) {
		err = cellAudioOutGetState(
			CELL_AUDIO_OUT_PRIMARY, 0, &a_state
			);
		if( err == CELL_OK ) {
			if( a_state.state ==
				CELL_AUDIO_OUT_OUTPUT_STATE_ENABLED )
			{
				// Audio is enabled and ready
				break;
			}
		}
		else if(err!=CELL_AUDIO_IN_ERROR_CONDITION_BUSY)
		{
			break;
		}
		sys_timer_usleep(5000);
	}

	// Initialize audio system
	err = cellAudioInit();
	// Create a mixer object
	err = cellSurMixerCreate(&g_SurMixerConfigS0);
	// Get handle of Surround Mixer
	err = cellSurMixerGetAANHandle(&mixerHandle);
	// Get port number of Channel Strip
	err = cellSurMixerChStripGetAANPortNo(
		&strip_0_port_0,CELL_SURMIXER_CHSTRIP_TYPE2A,0
		);

}