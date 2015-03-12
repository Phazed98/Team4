#pragma once

#include <vector>
#include <algorithm>
#include "SoundEmitter.h"
#include "Sound.h"
#include "SoundManager.h"
#include "../OpenAL 1.1 SDK/include/al.h"
#include "../OpenAL 1.1 SDK/include/alc.h"


using std::vector;

class SoundEmitter;

struct OALSource {
	ALuint source;
	bool inUse;

	OALSource(ALuint src) {
		source = src;
		inUse = false;
	}
};

class SoundSystem {
public:
	static void Initialise(unsigned int channels = 32) {
		if (!instance) { instance = new SoundSystem(channels); }
	}

	static void Destroy() { delete instance; }

	inline static SoundSystem* GetSoundSystem() { return instance; }
	//static SoundSystem* GetSoundSystem () {return instance;}
	void SetListenerTransform(const Matrix4& transform) {
		listenerTransform = transform;
	}
	Matrix4 GetListenerTransform() {
		return listenerTransform;
	}
	void AddSoundEmitter(SoundEmitter* s) { emitters.push_back(s); }
	void RemoveSoundEmitter(SoundEmitter* s){};

	void Update(float msec);
	void SetMasterVolume(float value);
	void PlaySoundA(Sound* s, Vector3 position);
	//void PlaySound(Sound* s, SoundPriority p = SOUNDPRIORTY_LOW);
	void ChangeSceneSound(Sound* s);
	void SwitchBoard(Sound* s);
	void GameStart();
	void PlayMove();
	void PlaySoundB(Sound* s, Vector3 position);
protected:
	SoundSystem(unsigned int channels = 32);
	~SoundSystem(void);
	void UpdateListener();
	void UpdateTemporaryEmitters(float msec);

	void DetachSources(vector<SoundEmitter*>::iterator from,
		vector<SoundEmitter*>::iterator to);
	void AttachSources(vector<SoundEmitter*>::iterator from,
		vector<SoundEmitter*>::iterator to);

	void CullNodes();
	OALSource* GetSource();
	Matrix4 listenerTransform;
	float masterVolume;
	ALCcontext* context;
	ALCdevice* device;
	SceneNode* listener;

	SoundEmitter* GlobleSound01;
	SoundEmitter* GlobleSound02;
	SoundEmitter* GlobleSound03;
	SoundEmitter* VehicleMove;

	vector<OALSource*> sources;
	vector<SoundEmitter*> emitters;
	vector<SoundEmitter*> frameEmitters;

	static SoundSystem* instance;

	vector<SoundEmitter*> temporaryEmitters;

	vector<string> SoundsName;
};
