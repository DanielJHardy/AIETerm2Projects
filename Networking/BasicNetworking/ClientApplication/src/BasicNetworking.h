#pragma once

#include "BaseApplication.h"

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
public:
	RakNet::RakPeerInterface* m_pPeerInterface;

	const char* IP = "127.0.0.1";
	const unsigned short PORT = 5456;

private:

};