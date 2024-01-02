#include "files.hpp"
using namespace std;
/*------------------------------------ REGULAR FILE CLASS IMPLEMENTATION ------------------------------------*/
/*DIFFERS*/
//SIZE_T SIZE
//STRING VECTOR DATA
namespace FileSystemKeskin
{
        //CONSTRUCTORS & OVERLOADS
    RegularFile::RegularFile() : File("ft_regular"), size(0) {/*Default is enough*/}
    RegularFile::RegularFile(string name, string path, string type) : File(name,path,type), size(0) { this->printToSystem(); }

    RegularFile::RegularFile(const RegularFile &oth) : File(oth), size(oth.size)
    {
        this->data.clear();
        if(size != 0)
        {
            for(auto elm : oth.data)
            {
                this->data.push_back(elm);
            }
        }
    }
    RegularFile & RegularFile::operator=(const RegularFile &oth)
    {
        File::operator=(oth);
        this->size = oth.size;
        this->data.clear();
        if(size != 0)
        {
            for(auto elm : oth.data)
            {
                this->data.push_back(elm);
            }
        }
        return *this;
    }
    RegularFile::~RegularFile() {}
    

        //GETTERS & SETTERS,
    const size_t RegularFile::getSize() const {return size;}

        //GENERAL FUNCTIONS
            //VIRTUAL FUNCTIONS
    void RegularFile::printToSystem() {
        
        File::printToSystem();
        
        ofstream os("OSKeskin.txt", std::ios::app);
        if(!os.is_open())
        {
            throw invalid_argument("OS cannot open!\n");
        }
        if(!data.empty())
        {
            for(auto line : data)
            {
                size += sizeof(line) * line.length();
            }
        }
        os << size << endl;
        if(!data.empty())
        {
            for(auto line : data)
            {
                os << line << endl;
            }
            os << "$" << '\n';
        }
        os.close();
    }
    void RegularFile::readFromSystem(int &lineCounter)
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

        std::istringstream iss(subStrings[8]);
        iss >> this->size;
        if(size != 0)
        {
            this->data.clear();
            line.clear();
            while(getline(os,line))
            {
                lineCounter++;
                if(line == "$")
                {
                    break;
                }
                data.push_back(line);
               
            }
            //lineCounter++; //Adding one more becuse of $
        }
        os.close();
    }
    void RegularFile::showContents() const  //CAT
    {   
        if(!data.empty())
        {
            for(auto elm : this->data)
            {
                if(elm != "$")
                {
                    cout << elm << endl; //Prints line by line
                }
            }
        }
    }
    //#just copies to the vector (handle the file(OSKEskin.txt) problem somewhere another)
    //#kendi OSime printlerken $ unutma vectorde $ yok
    void RegularFile::fileToVector(string filePath) //CP
    {
        ifstream OSFile(filePath);
        if(!OSFile.is_open())
        {
            throw invalid_argument("Invalid file name!\n");
        }
        string line;
        this->data.clear();
        while(getline(OSFile,line)) //Read windows/linux file line by line
        {
            data.push_back(line);
        }
        this->printToSystem(); //# bunu bi düzenle sıkıntı çıkarabilir
        OSFile.close();
    }
}
    