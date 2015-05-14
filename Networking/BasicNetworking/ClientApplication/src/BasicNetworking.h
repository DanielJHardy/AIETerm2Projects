#pragma once

#include <vector>

#include "BaseApplication.h"
#include "Camera.h"
#include "../../ServerApplication/GameObject.h"

#include "RakPeerInterface.h"
#include "BitStream.h"


namespace RakNet
{
	class RakPeerInterface;
}

class BasicNetworkingApplication : public BaseApplication
{
public:
	BasicNetworkingApplication();
	~BasicNetworkingApplication();

	virtual bool startup();

	virtual void shutdown();

	virtual bool update(float deltaTime);

	virtual void draw();

	//network functions

	//init the connection
	void handleNetworkConnection();
	void initializeClientConnection();

	//handle incoming packets
	void handleNetworkMessages();

	void readObjectDataFromServer(RakNet::BitStream& bsIn);

	void createGameObject();

	void moveClientObject(float deltaTime);

	void sendUpdatedObjectPositionToServer(GameObject& obj);
	void sendUpdatedObjectVelocityToServer(GameObject& obj);

	void updateLocalObjectPosition(RakNet::BitStream& bsIn);
	void updateLocalObjectVelocity(RakNet::BitStream& bsIn);

	void getAllObjects();

public:
	RakNet::RakPeerInterface* m_pPeerInterface;

	const char* IP = "127.0.0.1";
	const unsigned short PORT = 5456;

	unsigned int m_uiClientID;

	Camera* m_cam;

	glm::vec3 m_myColour;
	std::vector<GameObject> m_gameObjects;

	unsigned int m_uiclientObjectIndex;

private:

};