#include "files.hpp"
#include "linked_file.hpp"
using namespace std;

/*------------------------------------ LINKED FILE CLASS IMPLEMENTATION ------------------------------------*/
namespace FileSystemKeskin
{
//BUNUN CONSTRUCTORLARI FALAN FULL DEĞİŞMELİ POINTERIN ÖZELLİKLERİNİ TUTMALI
    SoftLinkedFile::SoftLinkedFile() : File("ft_linked") {}
    SoftLinkedFile::SoftLinkedFile(string name, string path, string type, string linkedPath) : File(name,path,type), linkedFilePath(linkedPath) { this->printToSystem(); }

    void SoftLinkedFile::setLinkedPath(const string &path) { this->linkedFilePath = path; }
    void SoftLinkedFile::setPointer(RegularFile *obj) { this->linkedRegular = obj; } 
    RegularFile * SoftLinkedFile::getPointer() const { return this->linkedRegular; }
    const string SoftLinkedFile::getLinkedPath() const { return this->linkedFilePath; }

    void SoftLinkedFile::printToSystem()  
    {
        File::printToSystem();
        ofstream os("OSKeskin.txt", std::ios::app);
        if(!os.is_open())
        {
            throw invalid_argument("OS cannot open!\n");
        }
        if(linkedRegular != nullptr)
        {
            os << linkedFilePath << " " << linkedRegular->getSize() << endl; //Print linkedFile's name and size
        }
        else
        {
            os << "RegularRemoved" << " " << "0" << endl;
        }
        os.close();
    }
    void SoftLinkedFile::readFromSystem(int &lineCounter)
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
        this->linkedFilePath = subStrings[8];

        os.close();
    }
    void SoftLinkedFile::showContents() const 
    {
        if(linkedRegular->getSize() != 0)
        {
            linkedRegular->showContents(); 
        }
        else
        {
            throw runtime_error("File is empty\n");
        }
    }
}
    