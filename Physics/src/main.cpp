
#include "myPhysicsEngine.h"
#include "Callbacks.h"

int main()
{
	MyPhysicsEngine app;

	if (app.Startup() == false)
	{
		return -1;
	}

	SetCallbacks(app.m_window);

	while (app.Update() == true)
	{
		app.Draw();
	}

	return 0;

	
}