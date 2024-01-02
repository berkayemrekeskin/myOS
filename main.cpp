#include "files.hpp"
#include "shell.hpp"
using namespace std;
using namespace FileSystemKeskin;
using namespace ShellKeskin;

int main()
{
    try
    {
        Shell myShell;
        cout << "myShell\n";
        myShell.startOS();
    }
    catch(std::exception &e)
    {
        cout << e.what() << endl;
    }
    return 0;
}