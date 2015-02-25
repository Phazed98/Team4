#pragma once

#include <map>
#include "Sound.h"
#include <vector>

using std::map;
//class OggSound;
class SoundManager {
public:
	static void AddSound(string n);
	static Sound* GetSound(string name);

	static void DeleteSounds();
protected:
	SoundManager(void);
	~SoundManager(void);

	static map <string, Sound*> sounds;
	vector<string> SoundsName;
};