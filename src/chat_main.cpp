#include "ChatServer.h"

#define PORT "3490"

using namespace std;

/*
 * Main: create a listener and connection set, loop forever
 * processing connections.
 */
int main(void)
{
	ChatServer chat(PORT);
	chat.run();

	return 0;
}
