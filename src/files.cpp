#include "../includes/files.hpp"
using namespace std;

/*------------------------------------ FILE CLASS IMPLEMENTATION ------------------------------------*/
namespace FileSystemKeskin
{
    File::File() { /*Default is enough */}
    File::File(string _type) : type(_type) // Setting the time when the object gets created
    { 
        time_t currentTime = std::time(nullptr);
        time = std::ctime(&currentTime);
        time.erase(time.length()-1);
    }
    File::~File() { /*Default is enough*/ }

    // Setters & getters
    const string File::getName() const { return name; }
    const string File::getPath() const { return path; }
    const string File::getType() const { return type; }
    const string File::getTime() const { return time; }
    void File::setName(const string &n) { name = n; }
    void File::setPath(const string &p) { path = p; }
    void File::setType(const string &t) { type = t; }
    void File::setTime(const string &t) { time = t; }
    
    void File::printToSystem() // Virtual function to print the information and data (if there is) to the OSKeskin.txt
    {
        ofstream os("OSKeskin.txt", std::ios::app);
        if(!os.is_open())
        {
            throw invalid_argument("OS cannot open!\n");
        }
        os << type << " " << path << " " << name << " " << time << " "; // Printing the base classes members
        os.close();
    }
    void File::splitLine(const string &input, vector<string> &vect) // General use function to split the string
    {
        stringstream ss(input);
        string word;
        while(ss >> word)
        {
            vect.push_back(word);
        }
    }
}
