#include "SoundProgramming.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#include <iostream>
#include <fmod_errors.h>


bool SoundProgramming::startup()
{
    if (Application::startup() == false)
    {
        return false;
    }

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    Gizmos::create();

    m_camera = FlyCamera(1280.0f / 720.0f, 10.0f);
    m_camera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
    m_camera.sensitivity = 3;

	//initialize Fmod
	FMOD_RESULT result;
	m_pFmodSystem = nullptr;

	//create the main system object
	result = FMOD::System_Create(&m_pFmodSystem);

	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}

	//initialize FMOD with 512 channels
	result = m_pFmodSystem->init(512, FMOD_INIT_NORMAL, 0);

	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}

	//play background music
	result = m_pFmodSystem->createSound("./data/audio/background_music.ogg", FMOD_CREATESTREAM, 0, &m_pSound);
	//result = m_pFmodSystem->playSound(m_pSound, 0, false, &m_pChannel);
	m_pChannel->setPitch(2.0f);

	//setup enviroment sounds

	//env sound 1
	result = m_pFmodSystem->createSound("./data/audio/env_effect_01.ogg", FMOD_DEFAULT | FMOD_3D, 0, &m_pSound);

	if (result != FMOD_OK) printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));

	result = m_pFmodSystem->playSound(m_pSound, 0, false, &m_pChannel);

	if (result != FMOD_OK) printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));


	FMOD_VECTOR channel_position = { 10, 0, 0 };
	FMOD_VECTOR channel_velocity = { 0, 0, 0 };
	result = m_pChannel->set3DAttributes(&channel_position,&channel_velocity , 0);

	if (result != FMOD_OK) printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));

	result = m_pChannel->setLoopCount(10);

	if (result != FMOD_OK) printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));


	//env sound 2
	result = m_pFmodSystem->createSound("./data/audio/env_effect_02.ogg", FMOD_DEFAULT | FMOD_3D, 0, &m_pSound);

	if (result != FMOD_OK) printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));

	result = m_pFmodSystem->playSound(m_pSound, 0, false, &m_pChannel);

	if (result != FMOD_OK) printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));


	channel_position.x = -20;
	result = m_pChannel->set3DAttributes(&channel_position, &channel_velocity, 0);

	if (result != FMOD_OK) printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));

	result = m_pChannel->setLoopCount(10);

	if (result != FMOD_OK) printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));


	//set cam listner attributes
	cam_pos = { 0, 0, 0 };
	cam_vel = { 0, 0, 0 };
	cam_forward = { 0, 0, 0 };
	cam_up = { 0, 0, 0 };

    return true;
}

void SoundProgramming::shutdown()
{
    Gizmos::destroy();

	//shutdown fmod
	m_pFmodSystem->close();
	m_pFmodSystem->release();

    Application::shutdown();
}

bool SoundProgramming::update()
{
    if (Application::update() == false)
    {
        return false;
    }
	
	//update listener
	cam_pos.x = m_camera.world[3][0];
	cam_pos.y = m_camera.world[3][1];
	cam_pos.z = m_camera.world[3][2];

	cam_forward.x = m_camera.world[0][0];
	cam_forward.y = m_camera.world[0][1];
	cam_forward.z = m_camera.world[0][2];

	cam_up.x = m_camera.world[1][0];
	cam_up.y= m_camera.world[1][1];
	cam_up.z = m_camera.world[1][2];


	m_pFmodSystem->set3DListenerAttributes(0, &cam_pos, &cam_vel, &cam_forward, &cam_up);

	//update sound
	m_pFmodSystem->update();

    Gizmos::clear();

    float dt = (float)glfwGetTime();
    glfwSetTime(0.0);

    vec4 white(1);
    vec4 black(0, 0, 0, 1);
    for (int i = 0; i <= 20; ++i)
    {
        Gizmos::addLine(vec3(-10 + i, -0.01, -10), vec3(-10 + i, -0.01, 10),
            i == 10 ? white : black);
        Gizmos::addLine(vec3(-10, -0.01, -10 + i), vec3(10, -0.01, -10 + i),
            i == 10 ? white : black);
    }

    m_camera.update(dt);

    return true;
}

void SoundProgramming::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Gizmos::draw(m_camera.proj, m_camera.view);

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}