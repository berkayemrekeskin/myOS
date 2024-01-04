#include "files.hpp"
#include "shell.hpp"
using namespace std;
using namespace FileSystemKeskin;

//# TODO : önceden linklenenlerin pointerları farklı çıkıyor, cat ve ls çalışmıyor onlarda
// NOT : absolute path = kendi pathi + ismi
namespace ShellKeskin
{
    Shell::Shell() 
    {
        this->root = new Directory;
        this->root->setParent(nullptr);
        this->root->setPath("/");
        this->root->setName("");
        this->currentDirectory = root;
    }
    Shell::~Shell() 
    {
        // Delete all the objects that have been created
        for(auto elm : this->root->getElements())
        {
            delete elm;
        }
        delete currentDirectory;
        delete root;
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
            fileInfo.clear(); // #Burda hata olabilir belki (pek önemli değil)
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
                for(auto &elm : this->root->getElements())
                {
                    if(elm->getName() == obj->getLinkedName() && elm->getPath() == obj->getPath())
                    {
                        obj->setPointer(dynamic_cast<RegularFile*>(elm));
                        break;
                    }
                }
                this->addToRoot(obj);
            }
            else if(fileInfo[0] == "ft_directory")
            {
                // #Burda . ve .. oluştur (path checkleyip . ve .. veya sadece .)
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
        //for(const auto &folder : this->root->getElements())
        //{
        //    if(dynamic_cast<Directory *>(folder))
        //        if(folder->getPath() == root->getPath()) 
        //            dynamic_cast<Directory *>(folder)->setParent(root); 
        //}
//
        //for(const auto &folder : this->root->getElements())
        //{
        //    for(const auto &element : this->root->getElements())
        //    {
        //        if(dynamic_cast<Directory *>(folder))   // Find directory
        //        {
        //            cout << element->getPath() << " " << absolutePath(folder) << endl;
        //            if(element->getPath() == absolutePath(folder)) // Check their paths
        //            {
        //                if(dynamic_cast<Directory*>(element))   // Check if the element is a directory
        //                    dynamic_cast<Directory*>(element)->setParent(dynamic_cast<Directory*>(folder)); // Set it's parent if it is directory
        //                Directory *obj = dynamic_cast<Directory *>(folder);
        //                obj->addElements(element);
        //            } 
        //        }
        //    }
        //}
         //TEST THE IS ROOT GETS THE ELEMENTS
        //for(const auto& elm : this->root->getElements())
        //{
        //    cout << elm->getName() << endl;
        //    if(dynamic_cast<Directory *>(elm))
        //    {
        //        for(const auto& obj : dynamic_cast<Directory *>(elm)->getElements())
        //        {
        //            cout << " - - - " << obj->getName() << endl;
        //        }
        //    }
        //}
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

    //Recursive print-to-file function
    void Shell::addToOS(Directory * parentDirectory)
    {
        for(auto& elm : parentDirectory->getElements())
        {
            if(dynamic_cast<Directory*>(elm))
            {
                Directory *childDirectory = dynamic_cast<Directory*>(elm);
                addToOS(childDirectory);
            }
            if(parentDirectory == this->root)
            {
                cout << elm->getName() << " " << elm->getPath() << endl;
            }
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

        while(input != "quit")
        {
            inputs = empty;
            this->transformInput(input);

            if(this->inputs[0] == "quit")
            {
                this->addToOS(root);
                return ;
            }
            else if(this->inputs[0] == "ls")
            {
                this->ls(this->currentDirectory);
            }
            else if(this->inputs[0] == "mkdir")
            {
                this->mkdir();
            }
            else if(this->inputs[0] == "rm")
            {
                this->rm();
            }
            else if(this->inputs[0] == "cp")
            {
                this->cp();
            }
            else if(this->inputs[0] == "link")
            {
                this->link();
            }
            else if(this->inputs[0] == "cd")
            {
                this->cd();
            }
            else if(this->inputs[0] == "cat")
            {
                this->cat();
            }
            else
            {
                throw std::invalid_argument("Wrong command! Command not found!\n");
            }
            input.clear();
        }
    }

    void Shell::ls(Directory *current)
    {
        if(this->inputs.size() == 2 && this->inputs[1] == "-R")
        {
            //this->lsRecursive(currentDirectory,isOver);
    
        }
        else if(this->inputs.size() == 1)
        {
            lsPrint(this->currentDirectory);
        }
        else
            throw invalid_argument("ls !\n");
    }
    
    //void Shell::lsRecursive(const Directory *current, bool &isOver)
    //{
    //    for(auto elm : current->getElements())
    //    {
    //        isOver = true;
    //        if(dynamic_cast<Directory *>(elm))
    //        {
    //            isOver = false;
    //            lsRecursive(dynamic_cast<Directory*>(elm),isOver);
    //            lsPrint(dynamic_cast<Directory*>(elm));
    //        }
    //        if(isOver)
    //            break;
    //    }
    //}

    void Shell::lsPrint(const Directory *current)
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
            if(current == root)
            {
                if(elm->getPath() == root->getPath() + elm->getName())
                {
                    if(dynamic_cast<Directory*>(elm))
                    {
                        cout << "D " << elm->getName() << "        " << elm->getTime() << endl;
                    }
                    else if(dynamic_cast<RegularFile*>(elm))
                    {
                        RegularFile * obj = dynamic_cast<RegularFile *>(elm);
                        cout << "F " << obj->getName() << "        " << obj->getTime() << "        " << obj->getSize() << obj << endl;
                    }
                    else if(dynamic_cast<SoftLinkedFile*>(elm))
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
            else
            {
               if(elm->getPath() == current->getPath() + "/" + elm->getName())
                {
                    if(dynamic_cast<Directory*>(elm))
                    {
                        cout << "D " << elm->getName() << "        " << elm->getTime() << endl;
                    }
                    else if(dynamic_cast<RegularFile*>(elm))
                    {
                        RegularFile * obj = dynamic_cast<RegularFile *>(elm);
                        cout << "F " << obj->getName() << "        " << obj->getTime() << "        " << obj->getSize() << obj << endl;
                    }
                    else if(dynamic_cast<SoftLinkedFile*>(elm))
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
    }


    void Shell::mkdir()
    {
        //std::cout << "(MKDIR) Current Path: " << this->currentDirectory << endl; 
        if(this->inputs.size() == 1)
        {
            throw invalid_argument("No file name given!\n");
        }
        else
        {
            for(auto elm : root->getElements())
            {
                if(currentDirectory == root)
                {
                    if(elm->getName() == inputs[1] && elm->getPath() == currentDirectory->getPath() + elm->getName())
                    {
                        throw invalid_argument("There is already an existing directory!\n");
                    }
                }
                else
                {
                    if(elm->getName() == inputs[1] && elm->getPath() == currentDirectory->getPath() + "/" + elm->getName())
                    {
                        throw invalid_argument("There is already an existing directory!\n");
                    }
                }
            }

            //Create directory on current path
            Directory *obj = new Directory;
            obj->setName(inputs[1]);

            if(currentDirectory == root)
                obj->setPath(root->getPath() + obj->getName());
            else
                obj->setPath(currentDirectory->getPath() + "/" + obj->getName());

            obj->setParent(currentDirectory);

            cout << "P:" << obj->getParent()->getName() << " " << "C:" << obj->getName() << endl;
            this->addToRoot(obj);
            
            if(currentDirectory != root)
                currentDirectory->addElements(obj);
            
            cout << obj->getPath() << endl;
        
        }
    }

    void Shell::rm()
    {
        //if(this->inputs.size() == 1)
        //{
        //    throw invalid_argument("No file name given!\n");
        //}
        //else
        //{
        //    bool flag = false;
        //    for(auto &elm : this->filesystem)
        //    {
        //        if(this->inputs[1] == elm->getName() && this->currentDirectory->getPath() == elm->getPath())
        //        {
        //            //elm delete
        //            cout << elm->getName() << " bulundu" << endl;
        //            remove("OSKeskin.txt");
        //            for(const auto &elm : this->filesystem)
        //            {
        //            }
        //            flag = true;
        //        }
        //    }
        //    if(!flag)
        //    {
        //        throw invalid_argument("File does not exist in this path!\n");
        //    }
        //}
    }

    void Shell::cp()
    {
        for(auto &elm : this->root->getElements())
        {
            if(this->inputs[2] == elm->getName() && elm->getPath() == this->currentDirectory->getPath())
            {
                RegularFile *obj = new RegularFile;
                obj = dynamic_cast<RegularFile*>(elm);
                obj->fileToVector(this->inputs[1]);
                elm = obj;

                remove("OSKeskin.txt");
                this->addToOS(currentDirectory);
                return ;
            }
        }
        // #burda napıyom ag
        RegularFile *obj = new RegularFile;
        obj->setPath(this->currentDirectory->getPath());
        obj->setName(this->inputs[2]);
        obj->fileToVector(this->inputs[1]);
        this->addToRoot(obj);
        return ;
        
    }

    void Shell::link()
    {
        for(auto &elm : this->root->getElements())
        {
            if(this->inputs[1] == elm->getName() && elm->getPath() == this->currentDirectory->getPath())
            {
                SoftLinkedFile * obj = new SoftLinkedFile;
                obj->setName(this->inputs[2]);
                obj->setPath(this->currentDirectory->getPath());
                obj->setLinkedName(elm->getName());
                obj->setPointer(dynamic_cast<RegularFile*>(elm));
                this->addToRoot(obj);

                return ;
            }
        }
    }

    void Shell::cd()
    {
        Directory* temp_parent = currentDirectory;
        if(this->inputs.size() == 1)
        {
            this->currentDirectory = this->root;
            return ;
        }
        if(this->inputs[1] == "." || this->inputs[1] == "..")
            this->cd_checkInputs(temp_parent);
        else
        {
            temp_parent = currentDirectory;
            cout << temp_parent->getName() << " " << currentDirectory->getName() << endl;
            this->cd_checkPaths();
            cout << temp_parent->getName() << " " << currentDirectory->getName() << endl;
        }
    }


    void Shell::cd_checkInputs(Directory* temp_parent)
    {
        if(this->inputs[1] == ".")
            return ;
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
                cout << "1CurDir: " << currentDirectory->getName() << endl;
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
            //cout << "(CD_CHECK_PATHS) ELM name: " << elm->getName() << "  (CD_CHECK_PATHS) ELM path: " << elm->getPath() << endl;
            //cout << "(CD_CHECK_PATHS) CHECKING : " << elm->getPath().substr(0,elm->getPath().find_last_not_of("/") + 1) << endl; 

            if(this->inputs[1] == elm->getName())
            {
                if(dynamic_cast<Directory*>(elm))
                {
                    if(currentDirectory == root)
                    {
                        if((dynamic_cast<Directory*>(elm)->getPath() == root->getPath() + dynamic_cast<Directory*>(elm)->getName()))
                        {
                            this->currentDirectory = dynamic_cast<Directory*>(elm);
                            cout << "CurDir: " << currentDirectory->getName() << endl;
                            return ;
                        }
                    }
                    else
                    {
                        if((dynamic_cast<Directory*>(elm)->getPath() == currentDirectory->getPath() + "/" + dynamic_cast<Directory*>(elm)->getName()))
                        {
                            this->currentDirectory = dynamic_cast<Directory*>(elm);
                            cout << "CurDir: " << currentDirectory->getName() << endl;
                            return ;
                        }
                    }
                }
            }
            if(this->inputs[1][0] == '/')
            {
                ////for(const auto &elm : this->paths)
                ////{
                ////    if(elm == inputs[1])
                ////    {
                ////        this->currentDirectory->setPath(inputs[1]);
                ////        return ;
                ////    }
                ////}
                //for(const auto &elm : this->root->getElements())
                //{
                //    if(dynamic_cast<Directory*>(elm))
                //    {
                //        //if(elm->getPath() == inputs[1].substr(0,inputs[1].find_last_not_of("/"))) // bu da olabilri
                //        cout << absolutePath(elm) << " - " << inputs[1] << endl;
                //        if(absolutePath(elm) == inputs[1])
                //        {
                //            this->currentDirectory = dynamic_cast<Directory*>(elm);
                //            this->currentDirectory->setPath(absolutePath(elm));
                //            cout << this->currentDirectory->getName() << " " << this->currentDirectory->getPath() << endl;
                //            return ;
                //        }
                //    }
                //}
            }
        }
    }

    void Shell::cat()
    {
        for(auto elm : this->currentDirectory->getElements())
        {
            if(this->inputs[1] == elm->getName() && elm->getPath() == currentDirectory->getPath())
            {
                elm->showContents();
                return ;
            }
        }
    }

    const string Shell::absolutePath(const File* obj) const
    {
        if(obj->getPath() == root->getPath())
            return obj->getPath() + obj->getName();
        else
            return obj->getPath() + "/" + obj->getName();
    }
}