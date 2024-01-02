#include "files.hpp"
using namespace std;

/*------------------ DIRECTORY CLASS IMPLEMENTATION ------------------*/
/*DIFFERS*/
//FILE * VECTOR
namespace FileSystemKeskin
{
        //CONSTRUCTORS & OVERLOADS
    Directory::Directory() : File("ft_directory") {}
    Directory::Directory(string name, string path, string type) : File(name,path,type) { this->printToSystem(); }
    Directory::Directory(const Directory &oth) : File(oth)
    {
        this->elements.clear();
        if(!oth.elements.empty())
        {
            for(auto elm : oth.elements)
            {
                this->elements.push_back(elm);
            }
        }
    }
    Directory & Directory::operator=(const Directory &oth)
    {
        File::operator=(oth);
        this->elements.clear();
        if(!oth.elements.empty())
        {
            for(auto elm : oth.elements)
            {
                this->elements.push_back(elm);
            }
        }
        return *this;
    }
    Directory::~Directory() 
    {
        for(auto & elm : this->elements)
        {
            delete elm;
        }
    }


    void Directory::printToSystem() 
    {
        File::printToSystem();
        ofstream os("OSKeskin.txt", std::ios::app);
        if(!os.is_open())
        {
            throw invalid_argument("OS cannot open!\n");
        }
        os << endl;
        if(!elements.empty())
        {
            for(auto elm : this->elements)
            {
                os << elm->getName() << " ";    //Printing the elements in the directory
            }
            os << '\n';
        }
        
        os.close();
    }
    void Directory::readFromSystem(int &lineCounter)
    {
        string line;
        string _time;
        vector<string> subStrings;

        ifstream os("OSKeskin.txt");
        if(!os.is_open())
        {
            throw invalid_argument("OS cannot open!\n");
        }
        int index = 0;
        while(index != lineCounter) //Iterating through the line of the file
        {
            getline(os,line);
            index++;
        }
        getline(os,line); //Getting the file info & setting
        this->splitLine(line,subStrings);
        this->setType(subStrings[0]);
        this->setPath(subStrings[1]);
        this->setName(subStrings[2]);
        _time = subStrings[3] + " " + subStrings[4] + " " + subStrings[5] + " " + subStrings[6] + " " + subStrings[7]; //Concatanating the time substrings
        this->setTime(_time);

        os.close();
    }
    void Directory::showContents() const
    {
        for(auto elm : this->elements)
        {
            cout << elm->getName() << " ";
        }
        cout << endl;
    }
    vector<File *> Directory::getElements() const
    {
        return this->elements;
    }
    //void Directory::fileToVector(string filePath) //CP
    //{
    //}

    void Directory::addElements(File * obj)
    {
        this->elements.push_back(obj);
    }
}