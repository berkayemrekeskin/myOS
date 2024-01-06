#include "shell.hpp"
#include "files.hpp"
#include "regular_file.hpp"
#include "linked_file.hpp"
#include "directory_file.hpp"

using namespace std;
using namespace FileSystemKeskin;

namespace ShellKeskin
{
    Shell::Shell() 
    {
        this->files = new Directory;
        this->rootDirectory = new Directory;
        this->rootDirectory->setParent(nullptr);
        this->rootDirectory->setPath("/");
        this->rootDirectory->setName(".");
        this->currentDirectory = this->rootDirectory;
    }
    Shell::~Shell() {}
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
            throw invalid_argument("OS cannot open!\n");
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
                throw invalid_argument("File type unknown!\n");

            lineCounter++;
            oldLineCounter++;
        }
        os.close();

        this->addToFolders(); // Add special directories elements to them
        this->assignParents(); // Assign parents of directories
        this->checkInput(); // Get & check input
        
    }

    /*----------------------------------------- (LS) -----------------------------------------*/
    void Shell::ls(const Directory *current)
    {
        if(this->inputs.size() == 2 && this->inputs[1] == "-R")
        {
            this->lsRecursive(currentDirectory);
        }
        else if(this->inputs.size() == 1)
        {
            lsPrint(this->currentDirectory);
        }
        else
            throw invalid_argument("ls !\n");
    }
    void Shell::lsRecursive(const Directory *current)
    {
        cout << "./" << current->getName() << ":" << endl;
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
            throw invalid_argument("No file name given!\n");
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
            throw invalid_argument("No file name given!\n");
        }
        else
        {
            for(auto removed : this->files->getElements())
            {
                // Remove files
                if(dynamic_cast<RegularFile*>(removed) || dynamic_cast<SoftLinkedFile*>(removed))
                {
                    if(removed->getName() == inputs[1] && removed->getPath() == absolutePath(removed,this->currentDirectory))
                    {
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

                        for(auto elm : this->files->getElements())
                        {
                            cout << " - " << elm->getName() << endl;
                        }
                        for(auto elm : this->currentDirectory->getElements())
                        {
                            cout << " --- " << elm->getName() << endl;
                        }
                        remove("OSKeskin.txt");
                        this->addToOS();
                        break;
                    }
                }
            }

        }
    }
    /*----------------------------------------- (CP) -----------------------------------------*/
    void Shell::cp()
    {
        for(auto elm : this->files->getElements())
        {
            if(dynamic_cast<Directory*>(elm))
            {
                if(elm->getName() == inputs[1] && elm->getPath() == absolutePath(elm,currentDirectory))
                {
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
                    RegularFile* copiedFile = new RegularFile;
                    copiedFile->setName(this->inputs[2]);
                    copiedFile->setPath(absolutePath(copiedFile,currentDirectory));
                    copiedFile->setData(dynamic_cast<RegularFile*>(elm)->getData());
                    copiedFile->printToSystem();
                    files->addElements(copiedFile);
                    if(currentDirectory != rootDirectory)
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
            obj->printToSystem();
            this->addToFiles(obj);
            this->currentDirectory->addElements(obj);
            return ;
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
                newDirectory->addElements(childDirectory);
                childDirectory->printToSystem();
                addToFiles(childDirectory);
                if(dynamic_cast<Directory*>(elm)->getElements().empty())
                    return;
                cp_directory(childDirectory,dynamic_cast<Directory*>(elm));
            }
        }
    }
    /*----------------------------------------- (LINK) -----------------------------------------*/
    void Shell::link()
    {
        for(auto elm : this->files->getElements())
        {
            if(dynamic_cast<RegularFile*>(elm))
            {
                if(this->inputs[1] == elm->getName())
                {
                    SoftLinkedFile * obj = new SoftLinkedFile;
                    obj->setName(this->inputs[2]);
                    obj->setPath(absolutePath(obj,currentDirectory));
                    obj->setLinkedPath(elm->getPath());
                    obj->setPointer(dynamic_cast<RegularFile*>(elm));
                    obj->printToSystem();
                    this->addToFiles(obj);
                    this->currentDirectory->addElements(obj);
                    return ;
                }
            }
        }
    }
    /*----------------------------------------- (CD) -----------------------------------------*/
    void Shell::cd()
    {
        Directory* temp_parent = currentDirectory;
        if(this->inputs.size() == 1)
        {
            this->currentDirectory = this->files;
            return ;
        }
        if(this->inputs[1] == "." || this->inputs[1] == ".." || this->inputs[1] == "/")
            this->cd_checkInputs(temp_parent);
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
                throw invalid_argument("You are in the rootDirectory!\n");
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
            for(const auto &elm : this->files->getElements())
            {
                if(dynamic_cast<Directory*>(elm))
                {
                    if(elm->getPath() == inputs[1])
                    {
                        this->currentDirectory = dynamic_cast<Directory*>(elm);
                        return ;
                    }
                }
            }
        }
        else
        {
            for(auto elm : this->files->getElements())
            {
                if(this->inputs[1] == elm->getName())
                {
                    if(dynamic_cast<Directory*>(elm))
                    {
                        cout << elm->getPath() << " " << absolutePath(elm,this->currentDirectory) << endl;
                        if(elm->getPath() == absolutePath(elm,this->currentDirectory))
                        {
                            this->currentDirectory = dynamic_cast<Directory*>(elm);
                                return ;
                        }
                    }
                }
            }
        }
        
    }
    /*----------------------------------------- (CAT) -----------------------------------------*/
    void Shell::cat()
    {
        for(auto elm : this->currentDirectory->getElements())
        {
            if(this->inputs[1][0] != '/')
            {
                if(this->inputs[1] == elm->getName() && elm->getPath() == absolutePath(elm,currentDirectory))
                {
                    elm->showContents();
                    return ;
                }
            }
            else
            {
                if(this->inputs[1] == elm->getPath())
                {
                    elm->showContents();
                    return ;
                }
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
        //Roota burada da ekleyebilirsin
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

        while(1)
        {
            inputs = empty;
            this->transformInput(input);

            if(this->inputs[0] == "quit")
                return ;
            else if(this->inputs[0] == "ls")
                this->ls(this->currentDirectory);
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
                throw std::invalid_argument("Wrong command! Command not found!\n");
            input.clear();
        }
    }
}