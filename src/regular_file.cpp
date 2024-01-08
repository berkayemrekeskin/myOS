#include "../includes/files.hpp"
#include "../includes/regular_file.hpp"
using namespace std;
/*------------------------------------ REGULAR FILE CLASS IMPLEMENTATION ------------------------------------*/
namespace FileSystemKeskin
{
    RegularFile::RegularFile() : File("ft_regular"), size(0) { /* Default is enough */ }
    
    // Setters & getters
    size_t RegularFile::getSize() const {return size;}
    vector<string> RegularFile::getData() const { return this->data; }
    void RegularFile::setData(const vector<string> &newData) 
    { 
        this->data = newData; 
        this->size = 0;
        for(auto elm : this->data)
            this->size += sizeof(elm);
    }

    void RegularFile::printToSystem() // Virtual function overload, prints the information and the data to the OSKeskin.txt
    { 
        File::printToSystem(); // Calling the base classes function

        ofstream os("OSKeskin.txt", std::ios::app);
        if(!os.is_open())
        {
            throw invalid_argument("error: os cannot open!");
        }
        os << size << endl;

        if(!data.empty()) // Printing the data line by line to the OSKeskin.txt
        {
            for(auto line : data)
            {
                os << line << endl;
            }
            os << "$" << '\n';
        }
        os.close();
    }
    void RegularFile::readFromSystem(int &lineCounter) // Virtual function overload, reads the information from the OSKeskin.txt
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
        this->splitLine(line,subStrings); // Input helper to split the string
        // Setting all the data into the directory object
        this->setType(subStrings[0]);
        this->setPath(subStrings[1]);
        this->setName(subStrings[2]);
        _time = subStrings[3] + " " + subStrings[4] + " " + subStrings[5] + " " + subStrings[6] + " " + subStrings[7]; //Concatanating the time substrings
        this->setTime(_time);

        std::istringstream iss(subStrings[8]); // Getting the size of the RegularFile object
        iss >> this->size;
        if(size != 0) // If size is not 0 than it has data
        {
            this->data.clear();
            line.clear();
            while(getline(os,line)) // Getting the data until it reads "$"
            {
                lineCounter++;
                if(line == "$")
                {
                    break;
                }
                data.push_back(line);
            }
        }
        os.close();
    }
    void RegularFile::showContents() const // Pure virtual 'cat' command function override, prints the data to the screen
    {   
        if(!data.empty())
        {
            for(auto elm : this->data)
            {
                if(elm != "$")
                {
                    cout << elm << endl;
                }
            }
        }
        else
        {
            throw runtime_error("cat: file is empty!");
        }
    }
    void RegularFile::fileToVector(string filePath) // Helper for the 'cp' command, opens a file in the regular OS and copies the elements to the object's data
    {
        ifstream OSFile(filePath);
        if(!OSFile.is_open())
        {
            throw invalid_argument("cp: file not found!");
        }
        string line;
        this->data.clear();
        while(getline(OSFile,line)) //Read windows/linux file line by line
        {
            data.push_back(line);
        }
        OSFile.close();
        
        if(!data.empty()) // If data is not empty, add to the size of the data
        {
            for(auto line : data)
            {
                size += sizeof(line);
            }
        }
    }
}
    