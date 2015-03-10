#include "Wave.h"

#include <math.h>

#include <sys/paths.h>
#include <sys/timer.h>

#include <cell/audio.h>
#include <cell/mixer.h>

#include <cell/sysmodule.h>
#include <sysutil/sysutil_sysparam.h>

#include <string>

class SoundSystem
{
public:
	static void SystemAudioUtilitySet6ch();

	static const CellSurMixerConfig g_SurMixerConfigS0;
	static CellAANHandle mixerHandle;
	static unsigned int strip_0_port_0;
	static int err, num, ch_pcm, ch_ac3,ch_dts;
	static CellAudioOutConfiguration aConfig;
	static CellAudioOutState a_state;
};