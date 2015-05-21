#ifndef SOUND_PROGRAMMING_H_
#define SOUND_PROGRAMMING_H_

#include "Application.h"
#include "Camera.h"

#include <fmod.hpp>



class SoundProgramming : public Application
{
public:
    virtual bool startup();
    virtual void shutdown();
    virtual bool update();
    virtual void draw();

public:
    FlyCamera m_camera;
	
	//FMOD
	FMOD::System* m_pFmodSystem;
	
	FMOD::Sound* m_pSound;
	FMOD::Channel* m_pChannel;

private:
	FMOD_VECTOR cam_pos;
	FMOD_VECTOR cam_vel;
	FMOD_VECTOR cam_forward;
	FMOD_VECTOR cam_up;
};

#endif //CAM_PROJ_H_