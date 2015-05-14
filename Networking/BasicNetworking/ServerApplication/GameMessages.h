#pragma once

#include "MessageIdentifiers.h"

enum GameMessages
{
	ID_SERVER_TEXT_MESSAGE = ID_USER_PACKET_ENUM + 1,
	ID_SERVER_CLIENT_ID = ID_USER_PACKET_ENUM + 2,
	ID_CLIENT_CREATE_OBJECT = ID_USER_PACKET_ENUM + 3,
	ID_SERVER_FULL_OBJECT_DATA = ID_USER_PACKET_ENUM + 4,
	ID_CLIENT_GET_ALL_OBJECTS = ID_USER_PACKET_ENUM + 5,

	ID_UPDATE_OBJECT_POSITION = ID_USER_PACKET_ENUM + 6,		// Used by client and server in different ways. The client sends it to update its object.
	ID_UPDATE_OBJECT_VELOCITY = ID_USER_PACKET_ENUM + 7,		// The server sends it to all other clients to update it on their side.
};