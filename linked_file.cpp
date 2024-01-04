#include "files.hpp"
using namespace std;

/*------------------------------------ LINKED FILE CLASS IMPLEMENTATION ------------------------------------*/
/*DIFFERS*/
//PTR TO REGULAR FILE
namespace FileSystemKeskin
{
//BUNUN CONSTRUCTORLARI FALAN FULL DEĞİŞMELİ POINTERIN ÖZELLİKLERİNİ TUTMALI
    SoftLinkedFile::SoftLinkedFile() : File("ft_linked") {}
    SoftLinkedFile::SoftLinkedFile(string name, string path, string type, string linkedName) : File(name,path,type), linkedFileName(linkedName) { this->printToSystem(); }

    SoftLinkedFile::SoftLinkedFile(const SoftLinkedFile &oth) : File(oth), linkedFileName(oth.linkedFileName), linkedRegular(oth.linkedRegular)
    { /* #pointer olaylarını shellde yap*/ }
    SoftLinkedFile & SoftLinkedFile::operator=(const SoftLinkedFile &oth)
    {
        File::operator=(oth);
        linkedFileName = oth.linkedFileName;
        linkedRegular = oth.linkedRegular;
        return *this;
    }
    SoftLinkedFile::~SoftLinkedFile() {} 

    void SoftLinkedFile::setLinkedName(const string &name) { this->linkedFileName = name; }
    void SoftLinkedFile::setPointer(RegularFile *obj) { this->linkedRegular = obj; } 
    RegularFile * SoftLinkedFile::getPointer() const { return this->linkedRegular; }
    const string SoftLinkedFile::getLinkedName() const { return this->linkedFileName; }

    void SoftLinkedFile::printToSystem()  
    {
        File::printToSystem();
        ofstream os("OSKeskin.txt", std::ios::app);
        if(!os.is_open())
        {
            throw invalid_argument("OS cannot open!\n");
        }
        //string sizeString = to_string(linkedRegular->getSize());
        os << linkedFileName << endl; //<< " " << sizeString << endl; //Print linkedFile's name and size
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
        this->linkedFileName = subStrings[8];

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
    