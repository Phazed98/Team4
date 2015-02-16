#include "SoundSystem.h"
#include "SoundEmitter.h"


SoundSystem* SoundSystem::instance = NULL;

SoundSystem::SoundSystem (unsigned int channels) {
	listener = NULL;

	masterVolume = 1.0f;

	cout <<"Creating SoundSystem!"<<endl;

	cout <<"Found the following devices: "
		<<alcGetString (NULL, ALC_DEVICE_SPECIFIER)<<endl;

	//const char * devicename = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

	device = alcOpenDevice (NULL); // Open the ¡¯best ¡¯ device

	if (!device) {
		cout <<"SoundSystem creation failed ! No valid device !"<<endl;
		return;
	}
	cout <<"SoundSystem created with device: "
		<<alcGetString (device, ALC_DEVICE_SPECIFIER)<<endl;

	context = alcCreateContext (device, NULL);
	alcMakeContextCurrent (context);

	alDistanceModel (AL_LINEAR_DISTANCE_CLAMPED);
	for (unsigned int i = 0; i < channels; ++i) {
		ALuint source;

		alGenSources (1, &source);
		ALenum error = alGetError ();

		if(error == AL_NO_ERROR) {
			sources.push_back(new OALSource (source));
		}
		else{
			break;
		}
	}
	cout <<"SoundSystem has" << sources.size()
		<<"channels available!" <<endl;
	SoundManager::AddSound("D:/Team 4/Team4/Game Tech 2014/Sounds/14615__man__canon.wav");
	SoundManager::AddSound("D:/Team 4/Team4/Game Tech 2014/Sounds/36847__ecodtr__laserrocket2.wav");
	SoundManager::AddSound("D:/Team 4/Team4/Game Tech 2014/Sounds/41579__erdie__steps-on-stone01.wav");
	SoundManager::AddSound("D:/Team 4/Team4/Game Tech 2014/Sounds/56900__syna-max__war.wav");
}
SoundSystem::~SoundSystem (void) {
	for(vector<SoundEmitter*>::iterator i =  emitters.begin(); i != emitters.end(); ++i) {
		(*i)->DetachSource();
	}
	for(vector<SoundEmitter*>::iterator i =  frameEmitters.begin(); i != frameEmitters.end(); ++i) {
		(*i)->DetachSource();
	}

	alcMakeContextCurrent (NULL);
	for (vector<OALSource*>::iterator i = sources.begin ();
		i != sources.end (); ++i) {
			alDeleteSources (1, &(*i ) -> source);
			delete (*i);
	}

	alcDestroyContext (context);
	alcCloseDevice (device);
}

void SoundSystem::SetMasterVolume (float value) {
	value = max (0.0f, value);
	value = min (1.0f, value);
	masterVolume = value;
	alListenerf (AL_GAIN , masterVolume);
}

void SoundSystem::UpdateListener (){
	if(listener) {
		Vector3 worldPos = listenerTransform.GetPositionVector();

		Vector3 dirup[2];
		// forward
		dirup[0].x =- listenerTransform.values[2];
		dirup[0].y =- listenerTransform.values[6];
		dirup[0].z =- listenerTransform.values[10];
		// Up
		dirup[1].x = listenerTransform.values[1];
		dirup[1].y = listenerTransform.values[5];
		dirup[1].z = listenerTransform.values[9];

		alListenerfv (AL_POSITION, (float*)& worldPos);
		alListenerfv (AL_ORIENTATION, (float*)& dirup);
	}
}
void SoundSystem::Update (float msec) {
	UpdateListener();
	UpdateTemporaryEmitters(msec); // Add this line !


	for (vector<SoundEmitter*>::iterator i = emitters.begin();
		i != emitters.end(); ++i) {
			frameEmitters.push_back((*i));
			(*i)->Update(msec);
	}

	CullNodes();
	if(frameEmitters.size() > sources.size()) {
		std::sort (frameEmitters.begin(), frameEmitters.end(),
			SoundEmitter::CompareNodesByPriority);

		DetachSources(frameEmitters.begin()+(sources.size()+1),
			frameEmitters.end());
		AttachSources(frameEmitters.begin(),
			frameEmitters.begin() + sources.size());
	}
	else {
		AttachSources(frameEmitters.begin(), frameEmitters.end());
	}

	frameEmitters.clear();
	//vector<SoundEmitter*>().swap(frameEmitters);
}

void SoundSystem::CullNodes () {
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

	for(vector<SoundEmitter*>::iterator i = emitters.begin(); i != emitters.end();) {

		float length;

		if((*i)->GetIsGlobal()) {
			length = 0.0f;
		}
		
		else{
			length = (listener->GetWorldTransform().GetPositionVector() - 
				(*i)->GetWorldTransform().GetPositionVector()).Length();
		}
		
		if(length > (*i)->GetRadius() || !(*i)->GetSound() || (*i)->GetTimeLeft() < 0) {
			(*i)->DetachSource();	//Important!
			i = emitters.erase(i);
		}
		else{
			++i;
		}
	}

}

void SoundSystem::DetachSources(vector<SoundEmitter*>::iterator from, vector<SoundEmitter*>::iterator to) {
	for(vector<SoundEmitter*>::iterator i = from; i != to; ++i) {
		(*i) -> DetachSource();
	}
}

void SoundSystem::AttachSources(vector<SoundEmitter*>::iterator from, vector<SoundEmitter*>::iterator to) {
	for(vector<SoundEmitter*>::iterator i = from ;i != to; ++i) {
		if (!(*i) -> GetSource()) {
			(*i) -> AttachSource (GetSource());
		}
	}
}

OALSource* SoundSystem::GetSource() {
	for (vector<OALSource*>::iterator i = sources.begin();
		i != sources.end (); ++i) {
			OALSource* s = *i;
			if (!s -> inUse) {
				return s;
			}
	}
	return NULL;
}

void SoundSystem::UpdateTemporaryEmitters(float msec) {
	for(vector<SoundEmitter*>::iterator i = temporaryEmitters.begin();
		i != temporaryEmitters.end();) {
			if ((*i) -> GetTimeLeft() < 0.0f && !(*i) -> GetLooping()) {
				delete (*i);
				i = temporaryEmitters.erase(i);
			}
			else {
				frameEmitters.push_back((*i));
				(*i) -> Update(msec);
				++i;
			}
	}
}

void SoundSystem::PlaySoundA(Sound* s, Vector3 position) {
	SoundEmitter* n = new SoundEmitter();
	n -> SetLooping (false);
	//n -> SetTransform (Matrix4::Translation(position));
	n -> SetSound(s);
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
