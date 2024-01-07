#include "../includes/files.hpp"
#include "../includes/shell.hpp"
using namespace std;
using namespace FileSystemKeskin;
using namespace ShellKeskin;

int main()
{
    try
    {
        Shell myShell;
        cout << "myShell" << endl;
        myShell.startOS();
    }
    catch(std::exception &e)
    {
        cout << e.what() << endl;
    }
    return 0;
}