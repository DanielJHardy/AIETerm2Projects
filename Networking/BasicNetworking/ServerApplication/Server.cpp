#include "Server.h"

Server::Server()
{
	//Initialize the Raknet peer interface first
	m_pPeerInterface = RakNet::RakPeerInterface::GetInstance();
	m_pPeerInterface->ApplyNetworkSimulator(0.0f, 200, 20);

	m_uiConnectionCounter = 1;
	m_uiObjectCounter = 1;
}

Server::~Server()
{

}

void Server::run()
{

	//Startup the server, and start it listening to clients
	std::cout << "Starting up the server..." << std::endl;

	//Create a socket descriptor to describe this connection
	RakNet::SocketDescriptor sd(PORT, 0);

	//Now call startup - max of 32 connections, on the assigned port
	m_pPeerInterface->Startup(32, &sd, 1);
	m_pPeerInterface->SetMaximumIncomingConnections(32);


	handleNetworkMessages();
}


void Server::handleNetworkMessages()
{
	RakNet::Packet* packet = nullptr;

	while (1)
	{
		for (packet = m_pPeerInterface->Receive(); packet; m_pPeerInterface->DeallocatePacket(packet), packet = m_pPeerInterface->Receive())
		{
			switch (packet->data[0])
			{
			case ID_NEW_INCOMING_CONNECTION:
			{
				addNewConnection(packet->systemAddress);
				std::cout << "A connection is incoming.\n";
				break;
			}
			case ID_DISCONNECTION_NOTIFICATION:
				std::cout << "A client has disconnected.\n";
				removeConnection(packet->systemAddress);
				break;
			case ID_CONNECTION_LOST:
				std::cout << "A client lost the connection.\n";
				removeConnection(packet->systemAddress);
				break;
			case ID_CLIENT_CREATE_OBJECT:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				createNewObject(bsIn, packet->systemAddress);
				break;
			}
			case ID_UPDATE_OBJECT_POSITION:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				updateObjectPosition(bsIn, packet->systemAddress);
				break;
			}
			case ID_UPDATE_OBJECT_VELOCITY:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				updateObjectVelocity(bsIn, packet->systemAddress);
				break;
			}
			case ID_CLIENT_GET_ALL_OBJECTS:
			{
				sendAllGameObjectsToClient(packet->systemAddress);
				break;
			}
			default:
				std::cout << "Received a message with a unknown id: " << packet->data[0];
				break;
			}
		}
	}
}


void Server::addNewConnection(RakNet::SystemAddress systemAddress)
{
	ConnectionInfo info;
	info.sysAddress = systemAddress;
	info.uiConnectionID = m_uiConnectionCounter++;
	m_connectedClients[info.uiConnectionID] = info;

	sendClientIDToClient(info.uiConnectionID);
}

void Server::removeConnection(RakNet::SystemAddress systemAddress)
{
	for (auto it = m_connectedClients.begin(); it != m_connectedClients.end(); it++)
	{
		if (it->second.sysAddress == systemAddress)
		{
			m_connectedClients.erase(it);
			break;
		}
	}
}

unsigned int Server::systemAddressToClientID(RakNet::SystemAddress& systemAddress)
{
	for (auto it = m_connectedClients.begin(); it != m_connectedClients.end(); it++)
	{
		if (it->second.sysAddress == systemAddress)
		{
			return it->first;
		}
	}

	return 0;
}


void Server::sendClientIDToClient(unsigned int uiClientID)
{
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_CLIENT_ID);
	bs.Write(uiClientID);

	m_pPeerInterface->Send(&bs, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, m_connectedClients[uiClientID].sysAddress, false);
}

void Server::createNewObject(RakNet::BitStream& bsIn, RakNet::SystemAddress& ownerSysAddress)
{
	GameObject newGameObject;

	//Read in the information from the packet
	bsIn.Read(newGameObject.fXPos);
	bsIn.Read(newGameObject.fZPos);

	bsIn.Read(newGameObject.fRedColour);
	bsIn.Read(newGameObject.fGreenColour);
	bsIn.Read(newGameObject.fBlueColour);

	bsIn.Read(newGameObject.fXVelocity);
	bsIn.Read(newGameObject.fZVelocity);

	newGameObject.uiOwnerClientID = systemAddressToClientID(ownerSysAddress);
	newGameObject.uiObjectID = m_uiObjectCounter++;

	m_gameObjects.push_back(newGameObject);

	sendGameObjectToAllClients(newGameObject, (RakNet::SystemAddress)RakNet::UNASSIGNED_SYSTEM_ADDRESS);
}

void Server::updateObjectPosition(RakNet::BitStream& bsIn, RakNet::SystemAddress& ownerSysAddress)
{
	unsigned int tempObjID;
	bsIn.Read(tempObjID);

	//If sender doesnt owns the obj, return
	if (m_gameObjects[tempObjID-1].uiOwnerClientID != systemAddressToClientID(ownerSysAddress)) return;

	//update position
	bsIn.Read(m_gameObjects[tempObjID-1].fXPos);
	bsIn.Read(m_gameObjects[tempObjID-1].fZPos);

	//send to all other clients
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)GameMessages::ID_UPDATE_OBJECT_POSITION);
	bsOut.Write(tempObjID);
	bsOut.Write(m_gameObjects[tempObjID - 1].fXPos);
	bsOut.Write(m_gameObjects[tempObjID - 1].fZPos);

	m_pPeerInterface->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, 0, ownerSysAddress, true);

}

void Server::updateObjectVelocity(RakNet::BitStream& bsIn, RakNet::SystemAddress& ownerSysAddress)
{
	unsigned int inObjID;
	bsIn.Read(inObjID);

	//if the sender doesnt own this object, return
	if (m_gameObjects[inObjID - 1].uiOwnerClientID != systemAddressToClientID(ownerSysAddress)) return;

	//update velocity
	bsIn.Read(m_gameObjects[inObjID - 1].fXVelocity);
	bsIn.Read(m_gameObjects[inObjID - 1].fZVelocity);

	//send to all other clients
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)GameMessages::ID_UPDATE_OBJECT_VELOCITY);
	bsOut.Write(inObjID);
	bsOut.Write(m_gameObjects[inObjID - 1].fXVelocity);
	bsOut.Write(m_gameObjects[inObjID - 1].fZVelocity);

	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, ownerSysAddress, true);

}

void Server::sendGameObjectToAllClients(GameObject& gameObject, RakNet::SystemAddress& ownerSystemAddress)
{

	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)GameMessages::ID_SERVER_FULL_OBJECT_DATA);
	bsOut.Write(gameObject.fXPos);
	bsOut.Write(gameObject.fZPos);
	bsOut.Write(gameObject.fRedColour);
	bsOut.Write(gameObject.fGreenColour);
	bsOut.Write(gameObject.fBlueColour);
	bsOut.Write(gameObject.uiOwnerClientID);
	bsOut.Write(gameObject.uiObjectID);
	bsOut.Write(gameObject.fXVelocity);
	bsOut.Write(gameObject.fZVelocity);

	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, ownerSystemAddress, true);
}

void Server::sendAllGameObjectsToClient(RakNet::SystemAddress& client)
{
	GameObject objectOut;

	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		RakNet::BitStream bsOut;

		//create temp object to read data from
		objectOut = m_gameObjects[i];
		
		//write header
		bsOut.Write((RakNet::MessageID)GameMessages::ID_SERVER_FULL_OBJECT_DATA);

		//write data
		bsOut.Write(objectOut.fXPos);
		bsOut.Write(objectOut.fZPos);
		bsOut.Write(objectOut.fRedColour);
		bsOut.Write(objectOut.fGreenColour);
		bsOut.Write(objectOut.fBlueColour);
		bsOut.Write(objectOut.uiOwnerClientID);
		bsOut.Write(objectOut.uiObjectID);
		bsOut.Write(objectOut.fXVelocity);
		bsOut.Write(objectOut.fZVelocity);

		//send packet
		m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, client, false);
	}
}
