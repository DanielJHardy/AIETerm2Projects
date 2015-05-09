#include "BasicNetworking.h"

#include <iostream>
#include <string>


#include "MessageIdentifiers.h"
#include "../../ServerApplication/GameMessages.h"
#include "Gizmos.h"

#include <GLFW\glfw3.h>


BasicNetworkingApplication::BasicNetworkingApplication()
{
	//generate random color
	float R = float(rand()) / RAND_MAX * 1;
	float G = float(rand()) / RAND_MAX * 1;
	float B = float(rand()) / RAND_MAX * 1;
	m_myColour = glm::vec3(R,G,B);
}

BasicNetworkingApplication::~BasicNetworkingApplication()
{

}

bool BasicNetworkingApplication::startup()
{


	//Setup the basic window
	createWindow("Client Application", 1280, 720);

	//connect to server
	handleNetworkConnection();

	//setup camera
	m_cam = new Camera(60.0f, 1280.0f / 720.0f, 0.1f, 10000.0f);

	//setup Gizmos
	Gizmos::create();

	return true;
}

void BasicNetworkingApplication::shutdown()
{

}

bool BasicNetworkingApplication::update(float deltaTime)
{
	//update cam
	m_cam->update(deltaTime);

	//check for network messages
	handleNetworkMessages();

	moveClientObject(deltaTime);

	return true;
}

void BasicNetworkingApplication::draw()
{
	Gizmos::clear();
	for (int i = 0; i < m_gameObjects.size(); i++)
	{
		GameObject& obj = m_gameObjects[i];
		Gizmos::addSphere(glm::vec3(obj.fXPos, 2, obj.fZPos), 2, 32, 32, glm::vec4(obj.fRedColour, obj.fGreenColour, obj.fBlueColour, 1), nullptr);
	}
	Gizmos::draw(m_cam->getProjectionView());
}

void BasicNetworkingApplication::handleNetworkConnection()
{
	//Initialize the Raknet peer interface first
	m_pPeerInterface = RakNet::RakPeerInterface::GetInstance();
	initializeClientConnection();
}

void BasicNetworkingApplication::initializeClientConnection()
{
	//Create a socket discriptor to describe this connection
	//No data needed, as we will be connecting to a server
	RakNet::SocketDescriptor sd;

	//Now call startup - max of 1 connections (to the server)
	m_pPeerInterface->Startup(1, &sd, 1);

	std::cout << "Connecting to the server at: " << IP << std::endl;

	//Npw call connect to attempt to connect to the given server
	RakNet::ConnectionAttemptResult res = m_pPeerInterface->Connect(IP, PORT, nullptr, 0);

	//Finally, check to see if we connected, and if not, throw an error
	if (res != RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		std::cout << "Unable to start connection, Error number: " << res << std::endl;
	}
}

void BasicNetworkingApplication::handleNetworkMessages()
{
	RakNet::Packet* packet;

	for (packet = m_pPeerInterface->Receive(); packet; m_pPeerInterface->DeallocatePacket(packet), packet = m_pPeerInterface->Receive())
	{
		switch (packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			std::cout << "Another client has disconnected.\n";
			break;
		case ID_REMOTE_CONNECTION_LOST:
			std::cout << "Another client has lost connection.\n";
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			std::cout << "Another client has connected.\n";
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
			std::cout << "Our connect request has been accepted.\n";
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			std::cout << "The server is full.\n";
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			std::cout << "We have been disconnected.\n";
			break;
		case ID_CONNECTION_LOST:
			std::cout << "Connection lost.\n";
			break;
		case ID_SERVER_TEXT_MESSAGE:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

			RakNet::RakString str;
			bsIn.Read(str);
			std::cout << str.C_String() << std::endl;
			break;
		}
		case ID_SERVER_FULL_OBJECT_DATA:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			readObjectDataFromServer(bsIn);
			break;
		}
		case ID_SERVER_CLIENT_ID:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(m_uiClientID);

			std::cout << "Server has given us an ID of: " << m_uiClientID << std::endl;

			createGameObject();

			break;
		}
		default:
			std::cout << "Received a message with an unknown id: " << packet->data[0] << std::endl;
			break;
		}
	}
}

void BasicNetworkingApplication::readObjectDataFromServer(RakNet::BitStream& bsIn)
{
	//create a temp object that we will put all the object data into
	GameObject tempGameObject;

	//Read in the object data
	bsIn.Read(tempGameObject.fXPos);
	bsIn.Read(tempGameObject.fZPos);
	bsIn.Read(tempGameObject.fRedColour);
	bsIn.Read(tempGameObject.fGreenColour);
	bsIn.Read(tempGameObject.fBlueColour);
	bsIn.Read(tempGameObject.uiOwnerClientID);
	bsIn.Read(tempGameObject.uiObjectID);

	//check to see if this object is already stored in our local game object list
	bool bFound = false;
	for (int i = 0; i < m_gameObjects.size(); i++)
	{
		if (m_gameObjects[i].uiObjectID == tempGameObject.uiObjectID)
		{
			bFound = true;

			//update the game object
			GameObject& obj = m_gameObjects[i];
			obj.fXPos = tempGameObject.fXPos;
			obj.fZPos = tempGameObject.fZPos;
			obj.fRedColour = tempGameObject.fRedColour;
			obj.fGreenColour = tempGameObject.fGreenColour;
			obj.fBlueColour = tempGameObject.fBlueColour;
			obj.uiOwnerClientID = tempGameObject.uiOwnerClientID;
		}
	}

	//If we didnt find it, then its a new object - add it to  our object list
	if (!bFound)
	{
		m_gameObjects.push_back(tempGameObject);
		if (tempGameObject.uiOwnerClientID == m_uiClientID)
		{
			m_uiclientObjectIndex = m_gameObjects.size() - 1;
		}
	}
}

void BasicNetworkingApplication::createGameObject()
{
	//tell the server we want to create a new game object that will represent us
	RakNet::BitStream bsOut;

	GameObject tempGameObject;
	tempGameObject.fXPos = 0.0f;
	tempGameObject.fZPos = 0.0f;
	tempGameObject.fRedColour = m_myColour.r;
	tempGameObject.fGreenColour = m_myColour.g;
	tempGameObject.fBlueColour = m_myColour.b;

	//ensure that the write order is the same as the read order on the server
	bsOut.Write((RakNet::MessageID)GameMessages::ID_CLIENT_CREATE_OBJECT);
	bsOut.Write(tempGameObject.fXPos);
	bsOut.Write(tempGameObject.fZPos);
	bsOut.Write(tempGameObject.fRedColour);
	bsOut.Write(tempGameObject.fGreenColour);
	bsOut.Write(tempGameObject.fBlueColour);

	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void BasicNetworkingApplication::moveClientObject(float deltaTime)
{
	//we dont have a valid client ID, so we dont have a game object!
	if (m_uiClientID == 0) return;

	//No game object sent to us, so we dont know who we are yet
	if (m_gameObjects.size() == 0) return;

	bool bUpdatedObjectPosition = false;

	GameObject& myClientObject = m_gameObjects[m_uiclientObjectIndex];

	if (glfwGetKey(m_window, GLFW_KEY_UP))
	{
		myClientObject.fZPos += 2 * deltaTime;
		bUpdatedObjectPosition = true;
	}
	if (glfwGetKey(m_window, GLFW_KEY_DOWN))
	{
		myClientObject.fZPos -= 2 * deltaTime;
		bUpdatedObjectPosition = true;
	}

	if (bUpdatedObjectPosition)
	{
		sendUpdatedObjectPositionToServer(myClientObject);
	}


}

void BasicNetworkingApplication::sendUpdatedObjectPositionToServer(GameObject& obj)
{
	RakNet::BitStream bsOut;

	bsOut.Write((RakNet::MessageID)GameMessages::ID_CLIENT_UPDATE_OBJECT_POSITION);
	bsOut.Write(obj.uiObjectID);
	bsOut.Write(obj.fXPos);
	bsOut.Write(obj.fZPos);

	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

}