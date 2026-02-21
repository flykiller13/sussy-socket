#include <format>
#include <iostream>

#include "ChatClient.h"

#define IP "127.0.0.1"
#define PORT "3490"

using namespace std;

string input_client_name()
{
    string name;
    cout << "Enter username: ";
    getline(cin, name);
    cout << format("Welcome, {}!", name) << endl;
    return name;
}

int main()
{
    ChatClient client(IP, PORT, input_client_name());
    client.run();
}