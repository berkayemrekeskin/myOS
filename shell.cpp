#include "files.hpp"
#include "shell.hpp"
using namespace std;
using namespace FileSystemKeskin;

//# TODO : önceden linklenenlerin pointerları farklı çıkıyor, cat ve ls çalışmıyor onlarda

// 10mb : 10000000 bytes
/*TO DO

*/

namespace ShellKeskin
{
    Shell::Shell() 
    {
        this->root = new Directory;
        this->root->setParent(nullptr);
        this->root->setPath("/");
        this->root->setName(".");
        this->currentDirectory = root;
    }
    Shell::~Shell() 
    {
        // Delete all the objects that have been created
    }
    // First, read from file than add them to root, then search for the directories inside the root and add their elements to them.
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
            throw invalid_argument("OS cannot open!\n");
        while(getline(os,line))
            totalLine++;
        os.close();

        //Read from the file & add to root
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
                this->addToRoot(obj);
            }
            else if(fileInfo[0] == "ft_linked")
            {
                SoftLinkedFile *obj = new SoftLinkedFile;
                obj->readFromSystem(lineCounter);
                for(auto elm : this->root->getElements())
                {
                    if(elm->getName() == obj->getLinkedName())
                    {
                        obj->setPointer(dynamic_cast<RegularFile*>(elm));
                        break;
                    }
                }
                this->addToRoot(obj);
            }
            else if(fileInfo[0] == "ft_directory")
            {
                Directory *obj = new Directory;
                obj->readFromSystem(lineCounter);
                this->addToRoot(obj);

            }
            else
                throw invalid_argument("File type unknown!\n");

            lineCounter++;
            oldLineCounter++;
        }
        os.close();

        this->addToFolders(); // Add special directories elements to them
        this->assignParents(); // Assign parents of directories

        for(auto elm : this->root->getElements())
        {
            cout << elm->getName() << " " << elm->getPath()  << endl;
        }

        this->checkInput(); // Get & check input
    }
    void Shell::addToFolders()
    {
        for(auto directory : this->root->getElements())
        {
            if(dynamic_cast<Directory*>(directory))
            {
                for(auto element : this->root->getElements())
                {
                    if(element->getPath() == directory->getPath() + "/" + element->getName())
                    {
                        dynamic_cast<Directory*>(directory)->addElements(element);
                        cout << directory->getName() << " " << element->getName() << endl;
                    }
                }
            }
        }
    }
    void Shell::assignParents()
    {
        for(auto folder : this->root->getElements())
        {
            if(dynamic_cast<Directory*>(folder))
            {
                if(folder->getPath() == this->root->getPath() + folder->getName())
                {
                    dynamic_cast<Directory*>(folder)->setParent(this->root);
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
    void Shell::addToRoot(File *obj)
    {
        this->root->addElements(obj); 
    }

    void Shell::addToOS()
    {
        for(auto elm : this->root->getElements())
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
            cout << " C: " << current->getName() << endl;
            cout << " ELM: " << elm->getName() << endl;
            if(dynamic_cast<Directory *>(elm))
            {
                cout << " ELMP: " <<elm->getPath() << endl;
                lsRecursive(dynamic_cast<Directory*>(elm));

            }
        }
    }
    void Shell::lsPrint(const Directory* current)
    {
        //#bunları daha detaylı yapabilirsin ama çalışıyolar
        if(current == root)
        {
            cout << "D " << "." << "        " << root->getTime() << endl;
        }
        else
        {
            cout << "D " << "." << "        " << root->getTime() << endl;
            cout << "D " << ".." << "        " << root->getTime() << endl;
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
                        //#okurken pointer yanlış adres tutuyor çöz onu
                        //cout << obj << endl;
                        //cout << dynamic_cast<SoftLinkedFile *>(elm) << " ";
                        //cout << dynamic_cast<SoftLinkedFile *>(elm)->getPointer()  << " " << obj->getPointer() << endl;
                        cout << "F " << obj->getName() << "        " << obj->getTime() << "        " << obj->getPointer() << endl;
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
            for(auto elm : root->getElements())
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
            this->addToRoot(obj);
            obj->printToSystem();
            if(currentDirectory != root)
                currentDirectory->addElements(obj);
        }
    }
    void Shell::rmdir(Directory* removed)
    {
        int len_of_path = absolutePath(removed,currentDirectory).length();
        vector<File*> root_others;
        vector<File*> current_others;
        for(auto elm : this->root->getElements())
        {
            if(elm->getPath().substr(0,len_of_path) != absolutePath(removed,currentDirectory))
            {
                root_others.push_back(elm);
            }
        }
        for(auto elm : this->currentDirectory->getElements())
        {
            if(elm->getPath().substr(0,len_of_path) != absolutePath(removed,currentDirectory))
            {
                current_others.push_back(elm);
            }
        }
        this->root->setElements(root_others);
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
            for(auto removed : this->root->getElements())
            {
                // Remove files
                if(dynamic_cast<RegularFile*>(removed))
                {
                    if(removed->getName() == inputs[1] && removed->getPath() == absolutePath(removed,this->currentDirectory))
                    {
                        vector<File*> temp_root;
                        vector<File*> temp_parent;
                        //remove from root
                        for(auto elm : this->root->getElements())
                        {
                            if(elm != removed)
                            {
                                temp_root.push_back(elm);
                            }
                        }
                        //remove from directory
                        for(auto elm : this->currentDirectory->getElements())
                        {
                            if(elm != removed)
                            {
                                temp_parent.push_back(elm);
                            }
                        }
                        //for(auto elm : this->root->getElements())
                        //{
                        //    if(dynamic_cast<Directory*>(elm))
                        //    {
                        //        if(elm->getPath() == removed->getPath().substr(0,removed->getPath().find_last_of("/")))
                        //        {
                        //            for(auto obj : dynamic_cast<Directory*>(elm)->getElements())
                        //            {
                        //                if(obj != removed)
                        //                {
                        //                    temp_parent.push_back(obj);
                        //                }
                        //            }
                        //            dynamic_cast<Directory*>(elm)->setElements(temp_parent);
                        //            break;
                        //        }
                        //    }
                        //}
                        this->root->setElements(temp_root);
                        this->currentDirectory->setElements(temp_parent);
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
        
        for(auto elm : this->root->getElements())
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
                    currentDirectory->addElements(newDirectory);
                    return ;
                }
            }
        }

        //If it is a file
            //If it is OSKeskin to OSKeskin
        for(auto elm : this->root->getElements())
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
                    root->addElements(copiedFile);
                    if(currentDirectory != root)
                        currentDirectory->addElements(copiedFile);
                    return ;
                }
            }
        }
            //If it is from Linux to OSKeskin
        bool flag = false;
        for(auto elm : this->root->getElements())
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
                    this->addToRoot(obj);
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
            this->addToRoot(obj);
            if(currentDirectory != root)
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
                addToRoot(copyRegular);
                copyRegular->printToSystem();
            }
            else if(elm->getType() == "ft_linked")
            {
                SoftLinkedFile* copyLinked = new SoftLinkedFile;
                copyLinked->setName(elm->getName());
                copyLinked->setPath(absolutePath(copyLinked,newDirectory));
                copyLinked->setPointer(dynamic_cast<SoftLinkedFile*>(elm)->getPointer());
                newDirectory->addElements(copyLinked);
                addToRoot(copyLinked);
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
                addToRoot(childDirectory);
                if(dynamic_cast<Directory*>(elm)->getElements().empty())
                    return;
                cp_directory(childDirectory,dynamic_cast<Directory*>(elm));
            }
        }
    }
    /*----------------------------------------- (LINK) -----------------------------------------*/
    void Shell::link()
    {
        for(auto elm : this->root->getElements())
        {
            if(dynamic_cast<RegularFile*>(elm))
            {
                if(this->inputs[1] == elm->getName() && elm->getPath() == this->currentDirectory->getPath())
                {
                    SoftLinkedFile * obj = new SoftLinkedFile;
                    obj->setName(this->inputs[2]);
                    obj->setPath(this->currentDirectory->getPath());
                    obj->setLinkedName(elm->getName());
                    obj->setPointer(dynamic_cast<RegularFile*>(elm));
                    obj->printToSystem();
                    this->addToRoot(obj);

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
            this->currentDirectory = this->root;
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
            this->currentDirectory = this->root;
            return ;
        }
        if(this->inputs[1] == "..")
        {
            if(currentDirectory == root)
            {
                throw invalid_argument("You are in the root!\n");
                return;
            }
            if(currentDirectory->getParent() == root)
            {
                this->currentDirectory = this->root;
                return;
            }
            for(auto elm : this->root->getElements())
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
        for(auto elm : this->root->getElements())
        {
            if(this->inputs[1] == elm->getName())
            {
                if(dynamic_cast<Directory*>(elm))
                {
                    if(elm->getPath() == absolutePath(elm,this->currentDirectory))
                    {
                        this->currentDirectory = dynamic_cast<Directory*>(elm);
                            return ;
                    }
                }
            }
            if(this->inputs[1][0] == '/')
            {
                for(const auto &elm : this->root->getElements())
                {
                    if(dynamic_cast<Directory*>(elm))
                    {
                        if(dynamic_cast<Directory*>(elm)->getPath() == inputs[1])
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
            if(this->inputs[1] == elm->getName() && elm->getPath() == absolutePath(elm,this->currentDirectory))
                {
                    elm->showContents();
                    return ;
                }
        }
    }
    const string Shell::absolutePath(const File* obj, const Directory* current) const
    {
        if(current == root)
            return root->getPath() + obj->getName();
        else
            return current->getPath() + "/" + obj->getName();
    }
}