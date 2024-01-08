#include "../includes/files.hpp"
#include "../includes/linked_file.hpp"
using namespace std;

/*------------------------------------ LINKED FILE CLASS IMPLEMENTATION ------------------------------------*/
namespace FileSystemKeskin
{
    SoftLinkedFile::SoftLinkedFile() : File("ft_linked") { /*Default is enough*/ }

    //Setters & getters
    void SoftLinkedFile::setLinkedPath(const string &path) { this->linkedFilePath = path; }
    void SoftLinkedFile::setPointer(RegularFile *obj) { this->linkedRegular = obj; } 
    RegularFile * SoftLinkedFile::getPointer() const { return this->linkedRegular; }
    const string SoftLinkedFile::getLinkedPath() const { return this->linkedFilePath; }

    void SoftLinkedFile::printToSystem() // Virtual function overload, prints the information and the data of the RegularFile object to the OSKeskin.txt
    {
        File::printToSystem(); // Calling the base classes function
        
        ofstream os("OSKeskin.txt", std::ios::app);
        if(!os.is_open())
        {
            throw invalid_argument("error: os cannot open!");
        }
        if(linkedRegular != nullptr) // Printing if the SoftLinkedFile's pointer is not nullptr
        {
            os << linkedFilePath << " " << linkedRegular->getSize() << endl; //Print linkedFile's name and size
        }
        else
        {
            os << "RegularRemoved" << " " << "0" << endl;
        }
        os.close();
    }
    void SoftLinkedFile::readFromSystem(int &lineCounter) // Virtual function overload, reads the information from the OSKeskin.txt
    {
        string line;
        string _time;
        vector<string> subStrings;

        ifstream os("OSKeskin.txt");
        if(!os.is_open())
        {
            throw invalid_argument("error: os cannot open!");
        }
        int index = 0;
        while(index != lineCounter) //Iterating through the line of the file
        {
            getline(os,line);
            index++;
        }
        getline(os,line); // Getting the file info & initializing the infos
        this->splitLine(line,subStrings);  // Input helper to split the string
        // Setting all the data into the directory object
        this->setType(subStrings[0]);
        this->setPath(subStrings[1]);
        this->setName(subStrings[2]);
        _time = subStrings[3] + " " + subStrings[4] + " " + subStrings[5] + " " + subStrings[6] + " " + subStrings[7]; //Concatanating the time substrings
        this->setTime(_time);
        this->linkedFilePath = subStrings[8];

        os.close();
    }
    void SoftLinkedFile::showContents() const // Pure virtual 'cat' command function override, prints the elements of the RegularFile object to the screen
    {
        if(linkedRegular->getSize() != 0)
        {
            linkedRegular->showContents(); 
        }
        else
        {
            throw runtime_error("cat: file is empty!");
        }
    }
}
    