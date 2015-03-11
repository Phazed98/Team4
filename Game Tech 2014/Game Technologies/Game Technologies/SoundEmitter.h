#pragma once
#include "../nclgl/scenenode.h"
#include "Sound.h"
#include "SoundSystem.h"

#define NUM_STREAM_BUFFERS 3 
enum SoundPriority {
	SOUNDPRIORTY_LOW,
	SOUNDPRIORITY_MEDIUM,
	SOUNDPRIORITY_HIGH,
	SOUNDPRIORITY_ALWAYS
};

struct OALSource;

class SoundEmitter {
public:
	SoundEmitter(void);
	SoundEmitter(Sound* s);
	~SoundEmitter(void);

	void Reset();
	void SetSound(Sound* s);
	inline Sound* GetSound() { return sound; }

	inline void SetPriority(SoundPriority p){ priority = p; }
	inline SoundPriority GetPriority() { return priority; }

	inline void SetVolume(float vol) {
		volume = min(1.0f, max(0.0f, vol));
	}
	inline float GetVolume() { return volume; }

	inline void SetLooping(bool state) { isLooping = state; }
	inline bool GetLooping() { return isLooping; }

	inline void SetRadius(float value) {
		radius = max(0.0f, value);
	}
	inline float GetRadius() { return radius; }

	inline float GetTimeLeft() { return timeLeft; }

	inline OALSource* GetSource() { return currentSource; }

	void SetTarget(SceneNode* s) { target = s; }
	SceneNode* GetTarget(){ return target; }

	Vector3 GetPosition(){ return position; }

	void AttachSource(OALSource* s);
	void DetachSource();

	static bool CompareNodesByPriority(SoundEmitter* a, SoundEmitter* b);

	virtual void Update(float msec);

	bool GetIsGlobal() { return isGlobal; }
	void SetIsGlobal(bool value) { isGlobal = value; }

	Matrix4	GetWorldTransform() const { return worldTransform; }
protected:
	Matrix4 worldTransform;
	Sound* sound;
	OALSource* currentSource;
	SoundPriority priority;
	Vector3 position;
	SceneNode* target;  //add 10.2.2015
	float volume;
	float radius;
	bool isLooping;
	float timeLeft;
	bool isGlobal;
	double streamPos;
	ALuint streamBuffers[NUM_STREAM_BUFFERS];
};
