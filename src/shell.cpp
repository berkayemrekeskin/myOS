#include "../includes/shell.hpp"
#include "../includes/files.hpp"
#include "../includes/regular_file.hpp"
#include "../includes/linked_file.hpp"
#include "../includes/directory_file.hpp"

using namespace std;
using namespace FileSystemKeskin;

namespace ShellKeskin
{
    Shell::Shell() 
    {
        this->fileSize = 0;
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
                for(auto elm : this->files->getElements())
                {
                    if(elm->getPath() == obj->getLinkedPath())
                    {
                        obj->setPointer(dynamic_cast<RegularFile*>(elm));
                        break;
                    }
                }
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

        this->addToFolders(); // Add special directories elements to them
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
            this->lsRecursive(currentDirectory);
        }
        else if(this->inputs.size() == 1)
        {
            lsPrint(this->currentDirectory);
        }
        else
            throw invalid_argument("ls: wrong inputs!");
    }
    void Shell::lsRecursive(const Directory *current)
    {
        cout << "." << current->getPath() << ":" << endl;
        lsPrint(current);
        cout << endl;
        for(const auto elm : current->getElements())
        {
            if(dynamic_cast<Directory *>(elm))
            {
                lsRecursive(dynamic_cast<Directory*>(elm));
            }
        }
    }
    void Shell::lsPrint(const Directory* current)
    {
        //#bunları daha detaylı yapabilirsin ama çalışıyolar
        cout << this->fileSize << endl;
        if(current == rootDirectory)
        {
            cout << "D " << "." << "        " << files->getTime() << endl;
        }
        else
        {
            cout << "D " << "." << "        " << files->getTime() << endl;
            cout << "D " << ".." << "        " << files->getTime() << endl;
        }
        for(auto elm : current->getElements())
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
                        cout << "F " << obj->getName() << "        " << obj->getTime() << "        " << obj->getSize() << endl;
                    }
                    else if(elm->getType() == "ft_linked")
                    {
                        SoftLinkedFile * obj = dynamic_cast<SoftLinkedFile *>(elm);
                        cout << "L " << obj->getName() << "        " << obj->getTime() << "        " << obj->getPointer()->getSize() << endl;
                    }
            }
        }
    }
    /*----------------------------------------- (MKDIR) -----------------------------------------*/
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
        else
        {
            for(auto elm : files->getElements())
            {
                if(elm->getName() == inputs[1] && elm->getPath() == absolutePath(elm,this->currentDirectory))
                {
                    if(dynamic_cast<Directory*>(elm))
                    {
                        rmdir(dynamic_cast<Directory*>(elm));
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
        }
    }
    void Shell::rmdir(Directory* removed)
    {
        int len_of_path = absolutePath(removed,currentDirectory).length();
        vector<File*> files_others;
        vector<File*> current_others;
        for(auto elm : this->files->getElements())
        {
            if(elm->getPath().substr(0,len_of_path) != absolutePath(removed,currentDirectory))
            {
                files_others.push_back(elm);
            }
        }
        for(auto elm : this->currentDirectory->getElements())
        {
            if(elm->getPath().substr(0,len_of_path) != absolutePath(removed,currentDirectory))
            {
                current_others.push_back(elm);
            }
        }
        this->files->setElements(files_others);
        this->currentDirectory->setElements(current_others);
        remove("OSKeskin.txt");
        addToOS();
    }
    /*----------------------------------------- (RM) -----------------------------------------*/
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
        else
        {
            bool isFound = false;
            for(auto removed : this->files->getElements())
            {
                // Remove files
                if(dynamic_cast<RegularFile*>(removed) || dynamic_cast<SoftLinkedFile*>(removed))
                {
                    if(removed->getName() == inputs[1] && removed->getPath() == absolutePath(removed,this->currentDirectory))
                    {
                        isFound = true;
                        vector<File*> temp_files;
                        vector<File*> temp_parent;
                        //remove from files
                        for(auto elm : this->files->getElements())
                        {
                            if(elm != removed)
                            {
                                cout << elm->getName() << endl;
                                temp_files.push_back(elm);
                            }
                        }
                        //remove from directory
                        for(auto elm : this->currentDirectory->getElements())
                        {
                            if(elm != removed)
                            {
                                cout << elm->getName() << endl;
                                temp_parent.push_back(elm);
                            }
                        }
                        this->files->setElements(temp_files);
                        this->currentDirectory->setElements(temp_parent);

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
        for(auto elm : this->files->getElements())
        {
            if(inputs[2] == elm->getName())
                throw invalid_argument("cp: file occurs on os!");
        }
        if(inputs.size() == 1)
        {
            throw invalid_argument("cp: no file or directory name given!");
        }
        else if(inputs.size() > 3)
        {
            throw invalid_argument("cp: too much inputs!");
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

                        newDirectory->printToSystem();

                        cp_directory(newDirectory,dynamic_cast<Directory*>(elm));

                        this->files->addElements(newDirectory);
                        currentDirectory->addElements(newDirectory);
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

                        if(this->fileSize + copiedFile->getSize() > 10*1024*1024)
                            throw runtime_error("size: size becomes bigger than 10mb!");
                        this->fileSize += copiedFile->getSize();
                        copiedFile->printToSystem();

                        files->addElements(copiedFile);
                        currentDirectory->addElements(copiedFile);
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

                        if(this->fileSize + obj->getSize() > 10*1024*1024)
                            throw runtime_error("size: size becomes bigger than 10mb!");
                        this->fileSize += obj->getSize();

                        cout << obj->getSize() << " " << this->fileSize << endl; 

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

                if(this->fileSize + obj->getSize() > 10*1024*1024)
                    throw runtime_error("size: size becomes bigger than 10mb!");
                this->fileSize += obj->getSize();

                cout << obj->getSize() << " " << this->fileSize << endl; 

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
    void Shell::cp_directory(Directory* newDirectory, Directory* current)
    {
        for(auto elm : current->getElements())
        {
            if(elm->getType() == "ft_regular")
            {
                RegularFile* copyRegular = new RegularFile;
                copyRegular->setName(elm->getName());
                copyRegular->setPath(absolutePath(copyRegular,newDirectory));
                copyRegular->setData(dynamic_cast<RegularFile*>(elm)->getData());
                
                if(this->fileSize + copyRegular->getSize() > 10*1024*1024)
                    throw runtime_error("size: size becomes bigger than 10mb!");
                this->fileSize += copyRegular->getSize();
                
                newDirectory->addElements(copyRegular);
                addToFiles(copyRegular);
                copyRegular->printToSystem();
            }
            else if(elm->getType() == "ft_linked")
            {
                SoftLinkedFile* copyLinked = new SoftLinkedFile;
                copyLinked->setName(elm->getName());
                copyLinked->setPath(absolutePath(copyLinked,newDirectory));
                copyLinked->setPointer(dynamic_cast<SoftLinkedFile*>(elm)->getPointer());
                
                if(this->fileSize + copyLinked->getPointer()->getSize() > 10*1024*1024)
                    throw runtime_error("size: size becomes bigger than 10mb!");
                this->fileSize += copyLinked->getPointer()->getSize();
                
                newDirectory->addElements(copyLinked);
                addToFiles(copyLinked);
                copyLinked->printToSystem();
            }
        }
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
                childDirectory->printToSystem();
                
                if(dynamic_cast<Directory*>(elm)->getElements().empty())
                    return;
                
                cp_directory(childDirectory,dynamic_cast<Directory*>(elm));
            }
        }
    }
    /*----------------------------------------- (LINK) -----------------------------------------*/
    void Shell::link()
    {
        if(inputs.size() == 1)
        {
            throw invalid_argument("link: no file name given!");
        }
        else if(inputs.size() > 3)
        {
            throw invalid_argument("link: too much inputs!");
        }
        else
        {
            bool isRegularFound = false;
            for(auto elm : this->files->getElements())
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
                        
                        if(this->fileSize + obj->getPointer()->getSize() > 10*1024*1024)
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
    void Shell::cd()
    {
        Directory* temp_parent = currentDirectory;
        if(this->inputs.size() > 2)
        {
            throw invalid_argument("cd: too much inputs!");
        }
        else if(this->inputs.size() == 1)
        {
            this->currentDirectory = this->rootDirectory;
            return ;
        }
        else if(this->inputs[1] == "." || this->inputs[1] == ".." || this->inputs[1] == "/")
        {
            this->cd_checkInputs(temp_parent);
        }
        else
        {
            temp_parent = currentDirectory;
            this->cd_checkPaths();
        }
    }
    void Shell::cd_checkInputs(const Directory* temp_parent)
    {
        
        if(this->inputs[1] == ".")
            return ;
        if(this->inputs[1] == "/")
        {
            this->currentDirectory = this->rootDirectory;
            return ;
        }
        if(this->inputs[1] == "..")
        {
            if(currentDirectory == rootDirectory)
            {
                throw invalid_argument("cd: you are in the root!");
                return;
            }
            if(currentDirectory->getParent() == rootDirectory)
            {
                this->currentDirectory = this->rootDirectory;
                return;
            }
            for(auto elm : this->files->getElements())
            {
                if(dynamic_cast<Directory*>(elm))
                {
                    if(dynamic_cast<Directory*>(elm) == temp_parent)
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
        if(this->inputs[1][0] == '/')
        {
            bool isIn = false;
            for(const auto &elm : this->files->getElements())
            {
                if(dynamic_cast<Directory*>(elm))
                {
                    if(elm->getPath() == inputs[1])
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
                        if(elm->getPath() == absolutePath(elm,this->currentDirectory))
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
            for(auto elm : this->currentDirectory->getElements())
            {
                if(this->inputs[1][0] != '/')
                {
                    if(this->inputs[1] == elm->getName() && elm->getPath() == absolutePath(elm,currentDirectory))
                    {
                        isFound = true;
                        elm->showContents();
                        return ;
                    }
                }
                else
                {
                    if(this->inputs[1] == elm->getPath())
                    {
                        isFound = true;
                        elm->showContents();
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
    const string Shell::absolutePath(const File* obj, const Directory* current) const
    {
        if(current == rootDirectory)
            return rootDirectory->getPath() + obj->getName();
        else
            return current->getPath() + "/" + obj->getName();
    }
    
    void Shell::addToFolders()
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
    void Shell::assignParents()
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
    void Shell::addToFiles(File *obj)
    {
        this->files->addElements(obj); 
    }

    void Shell::addToOS()
    {
        for(auto elm : this->files->getElements())
        {
            elm->printToSystem();
        }
    }
    void Shell::getInput(string &input)
    {
        cout << "> ";
        getline(cin,input);
    }
    void Shell::transformInput(string &input) 
    {
        this->getInput(input);
        this->splitLine(input,this->inputs);
    }
    void Shell::splitLine(const string &input, vector<string> &vect)
    {
        stringstream ss(input);
        string word;
        while(ss >> word)
            vect.push_back(word);
    }
    void Shell::checkInput()
    {
            string input;
            vector<string> empty;
            bool isDone = false;
            while(!isDone)
            {
                try
                {   
                inputs = empty;
                this->transformInput(input);

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
                catch(const std::exception& e)
                {
                    cout << e.what() << endl;
                }
            }
            return ;
    }
}