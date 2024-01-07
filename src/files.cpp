#include "../includes/files.hpp"
using namespace std;

/*------------------------------------ FILE CLASS IMPLEMENTATION ------------------------------------*/
namespace FileSystemKeskin
{
    
    File::File(string _type) : type(_type)
    { 
        time_t currentTime = std::time(nullptr);
        struct tm *timeinfo = std::localtime(&currentTime);
        //auto currentTimePoint = std::chrono::system_clock::now();
        //std::time_t currentTime = std::chrono::system_clock::to_time_t(currentTimePoint);
        time = std::ctime(&currentTime);
        time.erase(time.length()-1);
    }
    File::File(string n, string p, string t) : name(n), path(p), type(t) 
    {
        time_t currentTime = std::time(nullptr);
        struct tm *timeinfo = std::localtime(&currentTime);
        //auto currentTimePoint = std::chrono::system_clock::now();
        //std::time_t currentTime = std::chrono::system_clock::to_time_t(currentTimePoint);
        time = std::ctime(&currentTime);
        time.erase(time.length()-1);
    }
    File::~File() { /*Default is enough*/ }

    const string File::getName() const { return name; }
    const string File::getPath() const { return path; }
    const string File::getType() const { return type; }
    const string File::getTime() const { return time; }
    void File::setName(const string &n) { name = n; }
    void File::setPath(const string &p) { path = p; }
    void File::setType(const string &t) { type = t; }
    void File::setTime(const string &t) { time = t; }
    
    void File::printToSystem() // Print obj to system
    {
        ofstream os("OSKeskin.txt", std::ios::app);
        if(!os.is_open())
        {
            throw invalid_argument("OS cannot open!\n");
        }
        os << type << " " << path << " " << name << " " << time << " ";
        os.close();
    }
    void File::splitLine(const string &input, vector<string> &vect)
    {
        stringstream ss(input);
        string word;
        while(ss >> word)
        {
            vect.push_back(word);
        }
    }
}
