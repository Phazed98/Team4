#include "SoundSystem.h"
#include "SoundEmitter.h"


SoundSystem* SoundSystem::instance = NULL;

SoundSystem::SoundSystem(unsigned int channels) {
	listener = NULL;

	masterVolume = 1.0f;

	cout << "Creating SoundSystem!" << endl;

	cout << "Found the following devices: "
		<< alcGetString(NULL, ALC_DEVICE_SPECIFIER) << endl;

	//const char * devicename = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

	device = alcOpenDevice(NULL); // Open the ¡¯best ¡¯ device

	if (!device) {
		cout << "SoundSystem creation failed ! No valid device !" << endl;
		return;
	}
	cout << "SoundSystem created with device: "
		<< alcGetString(device, ALC_DEVICE_SPECIFIER) << endl;

	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);

	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
	for (unsigned int i = 0; i < channels; ++i) {
		ALuint source;

		alGenSources(1, &source);
		ALenum error = alGetError();

		if (error == AL_NO_ERROR) {
			sources.push_back(new OALSource(source));
		}
		else{
			break;
		}
	}
	cout << "SoundSystem has" << sources.size()
		<< "channels available!" << endl;
	SoundManager::AddSound("../../Sounds/14615__man__canon.wav");
	SoundManager::AddSound("../../Sounds/36847__ecodtr__laserrocket2.wav");
	SoundManager::AddSound("../../Sounds/41579__erdie__steps-on-stone01.wav");
	SoundManager::AddSound("../../Sounds/56900__syna-max__war.wav");
	SoundManager::AddSound("../../Sounds/thunder.wav");
	SoundManager::AddSound("../../Sounds/water.wav");
	SoundManager::AddSound("../../Sounds/wind.wav");
	SoundManager::AddSound("../../Sounds/test.wav");
	SoundManager::AddSound("../../Sounds/test2.wav");
	SoundManager::AddSound("../../Sounds/void.wav");
	SoundManager::AddSound("../../Sounds/engine01.wav");
	SoundManager::AddSound("../../Sounds/engine05.wav");
	SoundManager::AddSound("../../Sounds/switchsound03.wav");
	SoundManager::AddSound("../../Sounds/switchsound01.wav");
	SoundManager::AddSound("../../Sounds/button.wav");
	SoundManager::AddSound("../../Sounds/collision01.wav");
	SoundManager::AddSound("../../Sounds/bullets.wav");
	SoundManager::AddSound("../../Sounds/move.wav");
	SoundManager::AddSound("../../Sounds/move02.wav");
	SoundManager::AddSound("../../Sounds/firecross.wav");
	SoundManager::AddSound("../../Sounds/thundercross.wav");
	SoundManager::AddSound("../../Sounds/watercross.wav");
	SoundManager::AddSound("../../Sounds/jet.wav");
	/*GlobleSound01 = new SoundEmitter();
	GlobleSound01->SetVolume(0.5);
	GlobleSound01->SetSound(SoundManager::GetSound("../../Sounds/test2.wav"));
	temporaryEmitters.push_back(GlobleSound01);
	GlobleSound02 = new SoundEmitter();
	GlobleSound02->SetSound(SoundManager::GetSound("../../Sounds/test2.wav"));
	temporaryEmitters.push_back(GlobleSound02);
	GlobleSound03 = new SoundEmitter();
	GlobleSound03->SetSound(SoundManager::GetSound("../../Sounds/engine05.wav"));
	GlobleSound03->SetVolume(0.2);
	temporaryEmitters.push_back(GlobleSound03);*/
	VehicleMove = NULL;
}

void SoundSystem::GameStart(){
	GlobleSound01 = new SoundEmitter();
	GlobleSound01->SetVolume(0.5);
	GlobleSound01->SetSound(SoundManager::GetSound("../../Sounds/test2.wav"));
	temporaryEmitters.push_back(GlobleSound01);
	GlobleSound02 = new SoundEmitter();
	GlobleSound02->SetVolume(1);
	GlobleSound02->SetSound(SoundManager::GetSound("../../Sounds/test2.wav"));
	temporaryEmitters.push_back(GlobleSound02);
	GlobleSound03 = new SoundEmitter();
	GlobleSound03->SetVolume(0.2);
	GlobleSound03->SetSound(SoundManager::GetSound("../../Sounds/engine05.wav"));
	temporaryEmitters.push_back(GlobleSound03);
	VehicleMove = new SoundEmitter();
	VehicleMove->SetVolume(0);
	VehicleMove->SetSound(SoundManager::GetSound("../../Sounds/jet.wav"));
	temporaryEmitters.push_back(VehicleMove);
}

SoundSystem::~SoundSystem(void) {
	for (vector<SoundEmitter*>::iterator i = emitters.begin(); i != emitters.end(); ++i) {
		(*i)->DetachSource();
	}
	for (vector<SoundEmitter*>::iterator i = frameEmitters.begin(); i != frameEmitters.end(); ++i) {
		(*i)->DetachSource();
	}

	alcMakeContextCurrent(NULL);
	for (vector<OALSource*>::iterator i = sources.begin();
		i != sources.end(); ++i) {
		alDeleteSources(1, &(*i)->source);
		delete (*i);
	}

	alcDestroyContext(context);
	alcCloseDevice(device);
}

void SoundSystem::SetMasterVolume(float value) {
	value = max(0.0f, value);
	value = min(1.0f, value);
	masterVolume = value;
	alListenerf(AL_GAIN, masterVolume);
}

void SoundSystem::UpdateListener(){
	if (listener) {
		Vector3 worldPos = listenerTransform.GetPositionVector();

		Vector3 dirup[2];
		// forward
		dirup[0].x = -listenerTransform.values[2];
		dirup[0].y = -listenerTransform.values[6];
		dirup[0].z = -listenerTransform.values[10];
		// Up
		dirup[1].x = listenerTransform.values[1];
		dirup[1].y = listenerTransform.values[5];
		dirup[1].z = listenerTransform.values[9];

		alListenerfv(AL_POSITION, (float*)& worldPos);
		alListenerfv(AL_ORIENTATION, (float*)& dirup);
	}
}
void SoundSystem::Update(float msec) {
	UpdateListener();
	UpdateTemporaryEmitters(msec); // Add this line !


	for (vector<SoundEmitter*>::iterator i = emitters.begin();
		i != emitters.end(); ++i) {
		frameEmitters.push_back((*i));
		(*i)->Update(msec);
	}

	CullNodes();
	if (frameEmitters.size() > sources.size()) {
		std::sort(frameEmitters.begin(), frameEmitters.end(),
			SoundEmitter::CompareNodesByPriority);

		DetachSources(frameEmitters.begin() + (sources.size() + 1),
			frameEmitters.end());
		AttachSources(frameEmitters.begin(),
			frameEmitters.begin() + sources.size());
	}
	else {
		AttachSources(frameEmitters.begin(), frameEmitters.end());
	}

	frameEmitters.clear();
	if (VehicleMove != NULL){
		VehicleMove->SetVolume(0);
	}
	//vector<SoundEmitter*>().swap(frameEmitters);
}

void SoundSystem::CullNodes() {
	/*for(vector<SoundEmitter*>::iterator i = frameEmitters.begin();
	i != frameEmitters.end();) {
	SoundEmitter* e = (*i);

	float length;


	if(e -> target) {
	length = (listenerTransform.GetPositionVector() -
	e -> target -> GetWorldTransform().GetPositionVector()).Length();
	}
	else {
	length = (listenerTransform.GetPositionVector() -
	e -> position).Length();
	}

	if( length > e -> GetRadius() || !e -> GetSound() ||
	e -> GetTimeLeft() < 0) {
	e -> DetachSource();
	i = frameEmitters.erase(i);
	}
	else {
	++i;
	}
	}*/

	for (vector<SoundEmitter*>::iterator i = frameEmitters.begin(); i != frameEmitters.end();) {

		float length;

		if ((*i)->GetIsGlobal()) {
			length = 0.0f;
		}

		else{

			length = (listenerTransform.GetPositionVector() -
				(*i)->GetWorldTransform().GetPositionVector()).Length();

		}
		/*if (length < (*i)->GetRadius())
		{
			(*i)->SetVolume(1 - (length / (*i)->GetRadius()));
		}*/

		if (length > (*i)->GetRadius() || !(*i)->GetSound() || (*i)->GetTimeLeft() < 0) {
			(*i)->DetachSource();	//Important!
			i = frameEmitters.erase(i);
		}
		else{
			++i;
		}
	}

}

void SoundSystem::DetachSources(vector<SoundEmitter*>::iterator from, vector<SoundEmitter*>::iterator to) {
	for (vector<SoundEmitter*>::iterator i = from; i != to; ++i) {
		(*i)->DetachSource();
	}
}

void SoundSystem::AttachSources(vector<SoundEmitter*>::iterator from, vector<SoundEmitter*>::iterator to) {
	for (vector<SoundEmitter*>::iterator i = from; i != to; ++i) {
		if (!(*i)->GetSource()) {
			(*i)->AttachSource(GetSource());
		}
	}
}

OALSource* SoundSystem::GetSource() {
	for (vector<OALSource*>::iterator i = sources.begin();
		i != sources.end(); ++i) {
		OALSource* s = *i;
		if (!s->inUse) {
			return s;
		}
	}
	return NULL;
}

void SoundSystem::UpdateTemporaryEmitters(float msec) {
	for (vector<SoundEmitter*>::iterator i = temporaryEmitters.begin();
		i != temporaryEmitters.end();) {
		if ((*i)->GetTimeLeft() < 0.0f && !(*i)->GetLooping()) {
			delete (*i);
			i = temporaryEmitters.erase(i);
		}
		else {
			frameEmitters.push_back((*i));
			(*i)->Update(msec);
			++i;
		}
	}
}

void SoundSystem::PlaySoundA(Sound* s, Vector3 position) {
	SoundEmitter* n = new SoundEmitter();
	n->SetLooping(false);
	//n -> SetTransform (Matrix4::Translation(position));
	n->SetSound(s);
	//n->SetIsGlobal(true);
	temporaryEmitters.push_back(n);
}
//void SoundSystem::PlaySound (Sound* s, SoundPriority p) {
//	SoundEmitter* n = new SoundEmitter();
//	n -> SetLooping(false);
//	n -> SetSound(s);
//	n -> SetIsGlobal(true);
//	n -> SetPriority(p);
//	temporaryEmitters.push_back(n);
//}
void SoundSystem::ChangeSceneSound(Sound* s){
	GlobleSound02->SetSound(s);
	GlobleSound02->SetLooping(true);
	GlobleSound02->SetIsGlobal(true);
}

void SoundSystem::SwitchBoard(Sound* s) {
	SoundEmitter* n = new SoundEmitter();
	n->SetLooping(false);
	//n -> SetTransform (Matrix4::Translation(position));
	n->SetSound(s);
	n->SetVolume(0.5);
	//n->SetIsGlobal(true);
	temporaryEmitters.push_back(n);
}

void SoundSystem::PlaySoundB(Sound* s, Vector3 position) {
	SoundEmitter* n = new SoundEmitter();
	n->SetLooping(false);
	//SceneNode* p = new SceneNode();


	n->SetWorldTransform(Matrix4::Translation(position));
	//n -> SetWorldTransform(Matrix4::Translation(position));
	n->SetSound(s);
	//n->SetIsGlobal(true);
	temporaryEmitters.push_back(n);
}

void SoundSystem::PlayMove() {
	VehicleMove->SetVolume(0.5);
}