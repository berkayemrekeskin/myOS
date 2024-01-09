#include "../includes/shell.hpp"
#include "../includes/files.hpp"
#include "../includes/regular_file.hpp"
#include "../includes/linked_file.hpp"
#include "../includes/directory_file.hpp"

using namespace std;
using namespace FileSystemKeskin;

namespace ShellKeskin
{
    const size_t Shell::MAX_SIZE = 10*1024*1024;    // Setting the size of the OSKeskin, which is 10MB

    Shell::Shell() // Constructor
    {
        // Initializing the variables
        this->fileSize = 0; // Setting the size of the OSKeskin to 0
        this->files = new Directory;
        this->rootDirectory = new Directory;
        this->rootDirectory->setParent(nullptr);
        this->rootDirectory->setPath("/");
        this->rootDirectory->setName(".");
        this->currentDirectory = this->rootDirectory;
    }
    Shell::~Shell() 
    { 
        for(auto elm : this->files->getElements())
            delete elm;
        delete files;
        delete rootDirectory;
    }
    // Start the OS
    void Shell::startOS() 
    {
        int lineCounter = 0;
        int oldLineCounter = 0;
        int totalLine = 0;

        string line;
        vector<string> fileInfo;

        //Count the total number of lines in OS
        ifstream os("OSKeskin.txt");
        if(!os.is_open())
        {
            ofstream os("OSKeskin.txt");
            os.close();
        }
        while(getline(os,line))
            totalLine++;
        os.close();
        //Read from the file & add to files
        os.open("OSKeskin.txt");
        if(!os.is_open())
            throw invalid_argument("error: OSKeskin cannot open!");

        while(lineCounter <= totalLine-1)
        {
            // Count until data has been read
            while(oldLineCounter < lineCounter) 
            {
                string temp;
                getline(os,line);
                line.clear();
                oldLineCounter++;
            }

            line.clear();
            getline(os,line);
            fileInfo.clear();
            this->splitLine(line,fileInfo);
            
            // Check the file type and add to files
            if(fileInfo[0] == "ft_regular")
            {
                RegularFile * obj = new RegularFile;
                obj->readFromSystem(lineCounter);
                this->fileSize += obj->getSize();
                this->addToFiles(obj);
                if(obj->getPath() == absolutePath(obj,rootDirectory))
                    rootDirectory->addElements(obj);
            }
            else if(fileInfo[0] == "ft_linked")
            {
                SoftLinkedFile *obj = new SoftLinkedFile;
                obj->readFromSystem(lineCounter);
                bool isPointerAlive = false;
                for(auto elm : this->files->getElements())
                {
                    if(elm->getPath() == obj->getLinkedPath())
                    {
                        isPointerAlive = true;
                        obj->setPointer(dynamic_cast<RegularFile*>(elm));
                        break;
                    }
                }
                if(!isPointerAlive)
                    obj->setPointer(nullptr);
                if(obj->getPointer() != nullptr)
                    this->fileSize += obj->getPointer()->getSize();
                this->addToFiles(obj);
                if(obj->getPath() == absolutePath(obj,rootDirectory))
                    rootDirectory->addElements(obj);
            }
            else if(fileInfo[0] == "ft_directory")
            {
                Directory *obj = new Directory;
                obj->readFromSystem(lineCounter);
                this->addToFiles(obj);
                if(obj->getPath() == absolutePath(obj,rootDirectory))
                    rootDirectory->addElements(obj);
            }
            else
                throw invalid_argument("error: file type unknown!");
            
            lineCounter++;
            oldLineCounter++;
        }
        os.close();

        this->addToFolders(); // Add special directories elements to themselves
        this->assignParents(); // Assign parents of directories
        this->checkInput(); // Get & check input
    }

    /*----------------------------------------- (LS) -----------------------------------------*/
    void Shell::ls()
    {
        if(this->inputs.size() > 2)
        {
            throw invalid_argument("ls: too much inputs!"); 
        }
        else if(this->inputs.size() == 2 && this->inputs[1] == "-R")
        {
            this->lsRecursive(currentDirectory); // Recursive ls
        }
        else if(this->inputs.size() == 1)
        {
            lsPrint(this->currentDirectory); // Print current directory
        }
        else
            throw invalid_argument("ls: wrong inputs!");
    }
    void Shell::lsRecursive(const Directory *current) // Recursive ls
    {
        cout << "." << current->getPath() << ":" << endl; 
        lsPrint(current);
        cout << endl;
        for(auto elm : current->getElements()) // Recursive call
        {
            if(dynamic_cast<Directory *>(elm))
            {
                lsRecursive(dynamic_cast<Directory*>(elm));
            }
        }
    }
    void Shell::lsPrint(const Directory* current) // Print current directory
    {
        if(current == rootDirectory)
        {
            cout << "D " << "." << "        " << files->getTime() << endl;
        }
        else
        {
            cout << "D " << "." << "        " << files->getTime() << endl;
            cout << "D " << ".." << "        " << files->getTime() << endl;
        }
        for(auto elm : current->getElements())  // Print elements of current directory
        {
            if(absolutePath(elm,current) == elm->getPath())
            {
                if(elm->getType() == "ft_directory")
                {
                    cout << "D " << elm->getName() << "        " << elm->getTime() << endl;
                }
                else if(elm->getType() == "ft_regular")
                {
                    RegularFile * obj = dynamic_cast<RegularFile *>(elm);
                    cout << "F " << obj->getName() << "        " << obj->getTime() << "        " << obj->getSize() << "Bytes" << endl;
                }
                else if(elm->getType() == "ft_linked")
                {
                    SoftLinkedFile * obj = dynamic_cast<SoftLinkedFile *>(elm);
                    cout << "L " << obj->getName() << "        " << obj->getTime();
                    if(obj->getPointer() == nullptr)
                        cout << "        " << "Pointed file is removed!" << endl;
                    else
                        cout <<  "        " << obj->getName() << " -> " << obj->getLinkedPath() << endl;
                }
            }
        }
    }

    /*----------------------------------------- (MKDIR) -----------------------------------------*/
    // Create directories
    void Shell::mkdir()
    {
        if(this->inputs.size() == 1)
        {
            throw invalid_argument("mkdir: no file name given!");
        }
        else if(this->inputs.size() > 2)
        {
            throw invalid_argument("mkdir: too much inputs!");
        }
        else if(this->inputs[1] == ".")
        {
            throw invalid_argument("mkdir: cannot name a directory '.'!");
        }
        else
        {
            //Check if directory is already in OSKeskin
            for(auto elm : files->getElements())
            {
                //If it is in OSKeskin
                if(elm->getName() == inputs[1] && elm->getPath() == absolutePath(elm,this->currentDirectory))
                {
                    if(dynamic_cast<Directory*>(elm))
                    {
                        //Remove directory from OSKeskin
                        rmdir(elm,currentDirectory);
                        remove("OSKeskin.txt");
                        this->addToOS();
                        return;
                    }
                }
            }
            //Create directory on current path
            Directory *obj = new Directory;
            obj->setName(inputs[1]);
            obj->setPath(absolutePath(obj,this->currentDirectory)); 
            obj->setParent(currentDirectory);
            this->addToFiles(obj);
            obj->printToSystem();
            currentDirectory->addElements(obj);

            this->fileSize += 4096;
        }
    }
    void Shell::removeFromFiles(File* file, Directory* current) // Remove from files
    {
        if(dynamic_cast<RegularFile*>(file))
        {
            this->fileSize -= dynamic_cast<RegularFile*>(file)->getSize();
        }
        else if(dynamic_cast<SoftLinkedFile*>(file) && dynamic_cast<SoftLinkedFile*>(file)->getPointer() != nullptr )
        {
            this->fileSize -= dynamic_cast<SoftLinkedFile*>(file)->getPointer()->getSize();
        }
        else if(dynamic_cast<Directory*>(file))
        {
            this->fileSize -= 4096;
        }
        this->files->removeFile(file);
        this->currentDirectory->removeFile(file);
    }
    void Shell::rmdir(File *file, Directory* current) // Remove directories
    {
        if (file->getName() == "/")
        {
            throw std::runtime_error("rmdir: root cannot be removed!");
        }
        else if (file->getType() == "ft_directory")
        {
            Directory *childDirectory = dynamic_cast<Directory *>(file);
            vector<File *> childFiles = childDirectory->getElements();
            for (const auto& element : childFiles)
            {
                if (element->getType() == "ft_regular" || element->getType() == "ft_linked")
                    removeFromFiles(element, childDirectory);
                else if (element->getType() == "ft_directory")
                    rmdir(element, childDirectory);
            }
            this->files->removeFile(file);
            this->currentDirectory->removeFile(file);
        }
    }
    /*----------------------------------------- (RM) -----------------------------------------*/
    // Remove files
    void Shell::rm()
    {
        if(this->inputs.size() == 1)
        {
            throw invalid_argument("rm: no file name given!");
        }
        else if(this->inputs.size() > 2)
        {
            throw invalid_argument("rm: too much inputs!");
        }
        else if(this->inputs[2] == ".")
        {
            throw invalid_argument("rm: cannot name a file '.'!");
        }
        else
        {
            bool isFound = false;
            for(auto removed : this->files->getElements())
            {
                if(dynamic_cast<RegularFile*>(removed) || dynamic_cast<SoftLinkedFile*>(removed))
                {
                    if(removed->getName() == inputs[1] && removed->getPath() == absolutePath(removed,this->currentDirectory))
                    {
                        isFound = true;
                        vector<File*> temp_files;
                        vector<File*> temp_parent;
                        //Remove from OSKeskin
                        for(auto elm : this->files->getElements())
                        {
                            if(elm != removed)
                            {
                                temp_files.push_back(elm);
                            }
                        }
                        //Remove from directory
                        for(auto elm : this->currentDirectory->getElements())
                        {
                            if(elm != removed)
                            {
                                temp_parent.push_back(elm);
                            }
                        }
                        if(dynamic_cast<RegularFile*>(removed))
                        {
                            for(auto elm : this->files->getElements())
                            {
                                if(dynamic_cast<SoftLinkedFile*>(elm))
                                {
                                    if(dynamic_cast<SoftLinkedFile*>(elm)->getPointer() == dynamic_cast<RegularFile*>(removed))
                                    {
                                        dynamic_cast<SoftLinkedFile*>(elm)->setPointer(nullptr);
                                    }
                                }
                            }
                        }
                        
                        //Delete the file
                        this->files->setElements(temp_files);
                        this->currentDirectory->setElements(temp_parent);
                        if(dynamic_cast<RegularFile*>(removed))
                            this->fileSize -= dynamic_cast<RegularFile*>(removed)->getSize();
                        else if(dynamic_cast<SoftLinkedFile*>(removed) && dynamic_cast<SoftLinkedFile*>(removed)->getPointer() != nullptr)  
                            this->fileSize -= dynamic_cast<SoftLinkedFile*>(removed)->getPointer()->getSize();

                        delete removed;
                        remove("OSKeskin.txt");
                        this->addToOS();
                        break;
                    }
                }
            }
            if(!isFound)
            {
                throw invalid_argument("rm: file not found!");
            }
        }
    }
    /*----------------------------------------- (CP) -----------------------------------------*/
    void Shell::cp()
    {
        //Check if file is already in OSKeskin
        for(auto elm : this->currentDirectory->getElements())
        {
            if(inputs[2] == elm->getName())
                throw invalid_argument("cp: file occurs on current directory!");
        }
        if(inputs.size() == 2)
        {
            throw invalid_argument("cp: no file or directory name given!");
        }
        else if(inputs.size() > 3)
        {
            throw invalid_argument("cp: too much inputs!");
        }
        else if(inputs[2] == ".")
        {
            throw invalid_argument("cp: cannot name a file '.'!");
        }
        else
        {
            bool isDirectoryIn = false;
            bool OSKeskinFile = false;
            for(auto elm : this->files->getElements())
            {
                if(dynamic_cast<Directory*>(elm))
                {
                    if(elm->getName() == inputs[1] && elm->getPath() == absolutePath(elm,currentDirectory))
                    {
                        isDirectoryIn = true;
                        Directory* newDirectory = new Directory;
                        newDirectory->setName(this->inputs[2]);
                        newDirectory->setPath(absolutePath(newDirectory,currentDirectory));
                        newDirectory->setParent(currentDirectory);

                        cp_directory(newDirectory,dynamic_cast<Directory*>(elm));
                        this->files->addElements(newDirectory);
                        this->currentDirectory->addElements(newDirectory);
                        for(auto elm : newDirectory->getElements())
                        {
                            if(dynamic_cast<SoftLinkedFile*>(elm))
                            {
                                link_recursive(dynamic_cast<SoftLinkedFile*>(elm),newDirectory);
                            }
                        }
                        remove("OSKeskin.txt");
                        this->addToOS();
                        return ;
                    }
                }
            }
            //If it is a file
                //If it is OSKeskin to OSKeskin
            for(auto elm : this->files->getElements())
            {
                if(dynamic_cast<RegularFile*>(elm))
                {
                    if(this->inputs[1] == elm->getName() && elm->getPath() == absolutePath(elm,this->currentDirectory))
                    {
                        OSKeskinFile = true;

                        RegularFile* copiedFile = new RegularFile;
                        copiedFile->setName(this->inputs[2]);
                        copiedFile->setPath(absolutePath(copiedFile,currentDirectory));
                        copiedFile->setData(dynamic_cast<RegularFile*>(elm)->getData());

                        if(this->fileSize + copiedFile->getSize() > MAX_SIZE)
                            throw runtime_error("size: size becomes bigger than 10mb!");
                        this->fileSize += copiedFile->getSize();

                        copiedFile->printToSystem();
                        this->files->addElements(copiedFile);
                        this->currentDirectory->addElements(copiedFile);
                        return ;
                    }
                }
            }
            //If it is from Linux to OSKeskin
            bool flag = false;
            for(auto elm : this->files->getElements())
            {
                if(dynamic_cast<RegularFile*>(elm))
                {
                    if(this->inputs[2] == elm->getName() && elm->getPath() == absolutePath(elm,this->currentDirectory))
                    {
                        flag = true;
                        RegularFile *obj = new RegularFile;
                        obj = dynamic_cast<RegularFile*>(elm);
                        obj->fileToVector(this->inputs[1]);

                        //Check if size is bigger than 10MB
                        if(this->fileSize + obj->getSize() > MAX_SIZE)
                            throw runtime_error("size: size becomes bigger than 10mb!");
                        this->fileSize += obj->getSize();

                        elm = obj;
                        remove("OSKeskin.txt");

                        this->addToOS();
                        this->addToFiles(obj);
                        this->currentDirectory->addElements(obj);
                        return ;
                    }
                }
            }
            if(!flag)
            {
                RegularFile *obj = new RegularFile;
                obj->setName(inputs[2]);
                obj->setPath(absolutePath(obj,this->currentDirectory));
                obj->fileToVector(this->inputs[1]);

                //Check if size is bigger than 10MB
                if(this->fileSize + obj->getSize() > MAX_SIZE)
                    throw runtime_error("size: size becomes bigger than 10mb!");
                this->fileSize += obj->getSize();

                obj->printToSystem();
                this->addToFiles(obj);
                this->currentDirectory->addElements(obj);
                return ;
            }
            if(!OSKeskinFile && !isDirectoryIn)
            {
                throw invalid_argument("cp: source file not found!");
            }
        }
    }
    // Copy directories recursively
    void Shell::cp_directory(Directory* newDirectory, Directory* current)
    {
        for(auto elm : current->getElements())
        {
            //If it is a file
            if(elm->getType() == "ft_regular")
            {
                RegularFile* copyRegular = new RegularFile;
                copyRegular->setName(elm->getName());
                copyRegular->setPath(absolutePath(copyRegular,newDirectory));
                copyRegular->setData(dynamic_cast<RegularFile*>(elm)->getData());
                
                //Check if size is bigger than 10MB
                if(this->fileSize + copyRegular->getSize() > MAX_SIZE)
                    throw runtime_error("size: size becomes bigger than 10mb!");
                this->fileSize += copyRegular->getSize();
                
                newDirectory->addElements(copyRegular);
                addToFiles(copyRegular);
            }
            //If it is a linked file
            else if(elm->getType() == "ft_linked")
            {
                SoftLinkedFile* copyLinked = new SoftLinkedFile;
                copyLinked->setName(elm->getName());
                copyLinked->setPath(absolutePath(copyLinked,newDirectory));
                copyLinked->setLinkedPath(dynamic_cast<SoftLinkedFile*>(elm)->getLinkedPath());
                newDirectory->addElements(copyLinked);
                addToFiles(copyLinked);
            }
        }
        //If there is a directory
        for(auto elm : current->getElements())
        {
            if(dynamic_cast<Directory*>(elm))
            {
                Directory* childDirectory = new Directory;
                childDirectory->setName(elm->getName());
                childDirectory->setPath(absolutePath(childDirectory,newDirectory));
                childDirectory->setParent(newDirectory);
                
                addToFiles(childDirectory);
                newDirectory->addElements(childDirectory);
                if(dynamic_cast<Directory*>(elm)->getElements().empty())
                    return;
                //Recursive call
                cp_directory(childDirectory,dynamic_cast<Directory*>(elm));
            }
        }
        for(auto elm : newDirectory->getElements())
        {
            if(dynamic_cast<SoftLinkedFile*>(elm))
            {
                link_recursive(dynamic_cast<SoftLinkedFile*>(elm),newDirectory);
            }
        }
    }
    void Shell::link_recursive(SoftLinkedFile* linked, Directory* newDirectory)
    {
        for(auto elm : newDirectory->getElements())
        {
            if(dynamic_cast<RegularFile*>(elm))
            {
                if(elm->getPath().substr(elm->getPath().find_last_of("/"),elm->getPath().size()) == 
                linked->getLinkedPath().substr(linked->getLinkedPath().find_last_of("/"),linked->getLinkedPath().size()))
                {
                    if(this->fileSize + dynamic_cast<RegularFile*>(elm)->getSize() > MAX_SIZE)
                        throw runtime_error("size: size becomes bigger than 10mb!");
                    this->fileSize += dynamic_cast<RegularFile*>(elm)->getSize();
                    
                    linked->setPointer(dynamic_cast<RegularFile*>(elm));
                    linked->setLinkedPath(elm->getPath());
                    return ;
                }
            }
            else if(dynamic_cast<Directory*>(elm))
            {
                link_recursive(linked,dynamic_cast<Directory*>(elm));
            }
        }
    }
    /*----------------------------------------- (LINK) -----------------------------------------*/
    // Link files
    void Shell::link()
    {
        if(inputs.size() == 1)
        {
            throw invalid_argument("link: no source and destination name given!");
        }
        else if(inputs.size() == 2)
        {
            throw invalid_argument("link: no destination name given!");
        }
        else if(inputs.size() > 3)
        {
            throw invalid_argument("link: too much inputs!");
        }
        else
        {
            for(auto elm : this->currentDirectory->getElements())
            {
                if(inputs[2] == elm->getName())
                    throw invalid_argument("link: file occurs on current directory!");
            }
            //Check if file is in OSKeskin
            bool isRegularFound = false;
            for(auto elm : this->currentDirectory->getElements())
            {
                if(dynamic_cast<RegularFile*>(elm))
                {
                    if(this->inputs[1] == elm->getName())
                    {
                        isRegularFound = true;
                        SoftLinkedFile * obj = new SoftLinkedFile;
                        obj->setName(this->inputs[2]);
                        obj->setPath(absolutePath(obj,currentDirectory));
                        obj->setLinkedPath(elm->getPath());
                        obj->setPointer(dynamic_cast<RegularFile*>(elm));

                        //Check if size is bigger than 10MB
                        if(this->fileSize + obj->getPointer()->getSize() > MAX_SIZE)
                            throw runtime_error("size: size becomes bigger than 10mb!");
                        this->fileSize += obj->getPointer()->getSize();
                        
                        obj->printToSystem();
                        this->addToFiles(obj);
                        this->currentDirectory->addElements(obj);
                        return ;
                    }
                }
            }
            if(!isRegularFound)
            {
                throw invalid_argument("link: source file not found!");
            }
        }
    }
    /*----------------------------------------- (CD) -----------------------------------------*/
    // Change directories 
    void Shell::cd()
    {
        Directory* temp_parent = currentDirectory;
        if(this->inputs.size() > 2)
        {
            throw invalid_argument("cd: too much inputs!");
        }
        else if(this->inputs.size() == 1) // If there is no input
        {
            this->currentDirectory = this->rootDirectory;
            return ;
        }
        else if(this->inputs[1] == "." || this->inputs[1] == ".." || this->inputs[1] == "/") // If it is a special input
        {
            this->cd_checkInputs(temp_parent);
        }
        else // If it is a path
        {
            temp_parent = currentDirectory;
            this->cd_checkPaths();
        }
    }
    void Shell::cd_checkInputs(const Directory* temp_parent)
    {
        if(this->inputs[1] == ".")  // If it is a current directory
            return ;
        if(this->inputs[1] == "/") // If it is a root directory
        {
            this->currentDirectory = this->rootDirectory;
            return ;
        }
        if(this->inputs[1] == "..") // If it is a parent directory
        {
            if(currentDirectory == rootDirectory) // If it is a root directory
            {
                throw invalid_argument("cd: you are in the root!");
                return;
            }
            if(currentDirectory->getParent() == rootDirectory) // If it is a parent of root directory
            {
                this->currentDirectory = this->rootDirectory;
                return;
            }
            for(auto elm : this->files->getElements()) 
            {
                if(dynamic_cast<Directory*>(elm))
                {
                    if(dynamic_cast<Directory*>(elm) == temp_parent) // If it is a parent directory
                    {
                        this->currentDirectory = dynamic_cast<Directory*>(elm)->getParent();
                        return ;
                    }
                }
            }
        }
        return ;
    }
    void Shell::cd_checkPaths()
    {
        if(this->inputs[1][0] == '/') // If it is a path
        {
            bool isIn = false;
            for(const auto &elm : this->files->getElements())
            {
                if(dynamic_cast<Directory*>(elm))
                {
                    if(elm->getPath() == inputs[1]) // If path is found
                    {
                        isIn = true;
                        this->currentDirectory = dynamic_cast<Directory*>(elm);
                        return ;
                    }
                }
            }
            if(!isIn)
            {
                throw invalid_argument("cd: directory not found!");
            }
        }
        else
        {
            bool isIn = false;
            for(auto elm : this->files->getElements())
            {
                if(this->inputs[1] == elm->getName())
                {
                    if(dynamic_cast<Directory*>(elm))
                    {
                        if(elm->getPath() == absolutePath(elm,this->currentDirectory)) // If path is found
                        {
                            isIn = true;
                            this->currentDirectory = dynamic_cast<Directory*>(elm);
                            return ;
                        }
                    }
                }
            }
            if(!isIn)
            {
                throw invalid_argument("cd: directory not found!");
            }
        }
        
    }
    /*----------------------------------------- (CAT) -----------------------------------------*/
    // Show contents of files
    void Shell::cat()
    {
        if(this->inputs.size() == 1)
        {
            throw invalid_argument("cat: no file or directory name given!");
        }
        else if(this->inputs.size() > 2)
        {
            throw invalid_argument("cat: too much inputs!");
        }
        else
        {
            bool isFound = false;
            if(this->inputs[1][0] != '/')
            {
                for(auto elm : this->currentDirectory->getElements())
                {
                    if(this->inputs[1] == elm->getName() && elm->getPath() == absolutePath(elm,currentDirectory)) // If file is found
                    {
                        isFound = true; 
                        if(dynamic_cast<Directory*>(elm))
                            throw invalid_argument("cat: cannot show contents of directory!");
                        else if(dynamic_cast<SoftLinkedFile*>(elm) && dynamic_cast<SoftLinkedFile*>(elm)->getPointer() == nullptr)
                            throw invalid_argument("cat: cannot show contents of removed file!");
                        else
                            elm->showContents(); // Show contents of file
                        return ;
                    }
                }
            }
            else
            {
                for(auto elm : this->files->getElements())
                {
                    if(this->inputs[1] == elm->getPath()) // If path is found
                    {
                        isFound = true;
                        if(dynamic_cast<Directory*>(elm))
                            throw invalid_argument("cat: cannot show contents of directory!");
                        else if(dynamic_cast<SoftLinkedFile*>(elm) && dynamic_cast<SoftLinkedFile*>(elm)->getPointer() == nullptr)
                            throw invalid_argument("cat: cannot show contents of removed file!");
                        else
                            elm->showContents(); // Show contents of file
                        return ;
                    }
                }
            }
            if(!isFound)
            {
                throw invalid_argument("cat: file or directory not found!");
            }
        }
    }
    /*----------------------------------------- (HELPERS) -----------------------------------------*/
    
    const string Shell::absolutePath(const File* obj, const Directory* current) const // Get absolute path
    {
        if(current == rootDirectory)
            return rootDirectory->getPath() + obj->getName();
        else
            return current->getPath() + "/" + obj->getName();
    }
    
    void Shell::addToFolders() // Add special directories elements to themselves
    {
        for(auto directory : this->files->getElements())
        {
            if(dynamic_cast<Directory*>(directory))
            {
                for(auto element : this->files->getElements())
                {
                    if(element->getPath() == directory->getPath() + "/" + element->getName())
                    {
                        dynamic_cast<Directory*>(directory)->addElements(element);
                    }
                }
            }
        }
    }
    void Shell::assignParents() // Assign parents of directories
    {
        for(auto folder : this->files->getElements())
        {
            if(dynamic_cast<Directory*>(folder))
            {
                if(folder->getPath() == this->rootDirectory->getPath() + folder->getName())
                {
                    dynamic_cast<Directory*>(folder)->setParent(this->rootDirectory);
                }
                for(auto elements : dynamic_cast<Directory*>(folder)->getElements())
                {
                    if(dynamic_cast<Directory*>(elements))
                    {
                        dynamic_cast<Directory*>(elements)->setParent(dynamic_cast<Directory*>(folder));
                    }
                }
            }
        }
    }
    void Shell::addToFiles(File *obj) // Add to files
    {
        this->files->addElements(obj); 
    }

    void Shell::addToOS() // Add to OSKeskin
    {
        for(auto elm : this->files->getElements())
        {
            elm->printToSystem();
        }
    }
    void Shell::getInput(string &input) // Get input
    {
        cout << "> ";
        getline(cin,input);
    }
    void Shell::transformInput(string &input) // Transform input
    {
        this->getInput(input);
        this->splitLine(input,this->inputs);
    }
    void Shell::splitLine(const string &input, vector<string> &vect) // Split input
    {
        stringstream ss(input);
        string word;
        while(ss >> word)
            vect.push_back(word);
    }
    void Shell::checkInput() // Check input
    {
            bool isDone = false;
            while(!isDone)  // Check input until quit
            {
                try // Check if there is an error
                {   
                    string input;
                    vector<string> empty;
                    inputs = empty; // Clear the inputs
                    this->transformInput(input); // Transform input

                    // Check the command
                    if(this->inputs[0] == "quit")
                    {
                        return;
                    }
                    else if(this->inputs[0] == "ls")
                        this->ls();
                    else if(this->inputs[0] == "mkdir")
                        this->mkdir();
                    else if(this->inputs[0] == "rm")
                        this->rm();
                    else if(this->inputs[0] == "cp")
                        this->cp();
                    else if(this->inputs[0] == "link")
                        this->link();
                    else if(this->inputs[0] == "cd")
                        this->cd();
                    else if(this->inputs[0] == "cat")
                        this->cat();
                    else
                        throw invalid_argument("error: wrong command, command not found!");
                    input.clear();
                }
                catch(const std::exception& e) // Catch the error
                {
                    cout << e.what() << endl; // Print the error
                }
            }
            return ;
    }
}