#include "../includes/files.hpp"
#include "../includes/directory_file.hpp"
using namespace std;

/*------------------ DIRECTORY CLASS IMPLEMENTATION ------------------*/
namespace FileSystemKeskin
{
    Directory::Directory() : File("ft_directory") { /* Default is enough */}

    // Setters & getters
    void Directory::addElements(File * obj) { this->elements.push_back(obj); }
    void Directory::setElements(const vector<File*> &other) { this->elements = other; }
    void Directory::setParent(Directory* parent) { this->parentDirectory = parent; }
    vector<File *> Directory::getElements() const { return this->elements; }
    Directory* Directory::getParent() const { return this->parentDirectory; }

    void Directory::printToSystem() // Prints the info of the directory to the OSKeskin.txt
    {
        File::printToSystem(); // Calling the base virtual class function to print the name,type,path & time
        ofstream os("OSKeskin.txt", std::ios::app);
        if(!os.is_open())
        {
            throw invalid_argument("OS cannot open!\n");
        }
        os << endl;
        os.close();
    }
    void Directory::readFromSystem(int &lineCounter) // Reads from the OSKeskin.txt & initializes the data
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
        while(index != lineCounter) // Iterating through the line of the directory info
        {
            getline(os,line);
            index++;
        }
        getline(os,line); //Getting the file info & initializing the infos
        this->splitLine(line,subStrings); // Input helper to split the string
        // Setting all the data into the directory object
        this->setType(subStrings[0]);
        this->setPath(subStrings[1]);
        this->setName(subStrings[2]);
        _time = subStrings[3] + " " + subStrings[4] + " " + subStrings[5] + " " + subStrings[6] + " " + subStrings[7]; //Concatanating the time substrings
        this->setTime(_time);

        os.close();
    }
    void Directory::removeFile(File *file)  // Removing files for the 'rmdir' command
    {
        for (auto it = elements.begin(); it != elements.end(); ++it) // Iterating through the elements
        {
            if (*it == file) // Finding & erasing the element
            {
                elements.erase(it); 
                return ;
            }
        }
    }
    void Directory::showContents() const // Pure virtual 'cat' command function override, prints the elements of the directory to the screen
    {
        if(!this->elements.empty())
        {
            for(auto elm : this->elements)
            {
                cout << elm->getName() << " ";
            }
            cout << endl;
        }
        else
        {
            throw invalid_argument("cat: directory is empty!");
        }
    }
}