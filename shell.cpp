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
        this->checkInput(); // Get & check input
        
    }

    void Shell::addToFolders()
    {
        for(const auto &folder : this->root->getElements())
        {
            if(dynamic_cast<Directory *>(folder))
                if(folder->getPath() == root->getPath() + folder->getName()) 
                    dynamic_cast<Directory *>(folder)->setParent(root); 
        }

        for(const auto &folder : this->root->getElements())
        {
            for(const auto &element : this->root->getElements())
            {
                if(dynamic_cast<Directory *>(folder))   // Find directory
                {
                    cout << element->getPath() << " " << folder->getPath() << endl;
                    if(element->getPath() == folder->getPath()) // Check their paths
                    {
                        if(dynamic_cast<Directory*>(element))   // Check if the element is a directory
                            dynamic_cast<Directory*>(element)->setParent(dynamic_cast<Directory*>(folder)); // Set it's parent if it is directory
                        Directory *obj = dynamic_cast<Directory *>(folder);
                        obj->addElements(element);
                    } 
                }
            }
        }
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
        //cout << "(LS) (1) Current Path: " << this->currentDirectory << endl; 
        //Check if '-R' used
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
            if(current->getPath() + elm->getName() == elm->getPath())
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
                if(this->inputs[1] == elm->getName() && currentDirectory->getPath() == elm->getPath()) //If names and paths are same
                    throw invalid_argument("There is already an existing directory!\n");

            //Create directory on current path
            Directory *obj = new Directory;
            obj->setName(inputs[1]);
            if(currentDirectory == root)
                obj->setPath(absolutePath(currentDirectory) + obj->getName());
            else
                obj->setPath(absolutePath(currentDirectory));

            this->addToRoot(obj);
            this->addToFolders();

            cout << "(MKDIR) objpath: " << obj->getPath() << endl;
            cout << "Current path name: " << currentDirectory->getPath() << " " << currentDirectory->getName() << endl; 

//. ve .. oluşturma sadece shell başlarken ve mkdir yapınca oluşturulup shell kapanınca yok olacak ekilde yap, printlerken if bloğunda yazdır.
            
            //ŞUNLARI SİL AG
            //Directory *current = new Directory;
            //current->setName(".");
            //if(currentDirectory->getPath() != "/")
            //    current->setPath(currentDirectory->getPath() + "/" + this->inputs[1]);
            //else
            //    current->setPath(currentDirectory->getPath() + this->inputs[1]);
            //this->addToFileSystem(current);
            //this->addToRoot(current);
//
            ////cout << "(MKDIR) Created directory inside Path: " << current->getPath() << endl;
//
            //Directory *father = new Directory;
            //father->setName("..");
            //if(currentDirectory->getPath() != "/")
            //    father->setPath(currentDirectory->getPath() + "/" + this->inputs[1]);
            //else
            //    father->setPath(currentDirectory->getPath() + this->inputs[1]);
            ////cout << "(MKDIR) Father Path: " << father->getPath() << endl;
            //this->addToFileSystem(father);
            //this->addToRoot(father);
//
//
            //obj->addElements(current);
            //obj->addElements(father);

            //for(auto elm : this->root->getElements())
            //{
            //    cout << elm->getName() << " " << elm->getPath() << endl;
            //}
            
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
                //RegularFile *temp = new RegularFile;
                //temp = dynamic_cast<RegularFile *>(elm); // #Şunlarda error check yap
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
        //cout << this->inputs[1][0] << endl;
        //cout << this->inputs[1] << endl;
        if(this->inputs.size() == 1)
        {
            this->currentDirectory = this->root;
            return ;
        }
        if(this->inputs[1] == "." || this->inputs[1] == "..")
            this->cd_checkInputs();
        else
            this->cd_checkPaths();
    }

    // #parentDirectory kullan
    void Shell::cd_checkInputs()
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
                this->currentDirectory = root;
                return;
            }
            for(const auto& elm : this->root->getElements())
            {
                if(dynamic_cast<Directory*>(elm))
                {
                    if(dynamic_cast<Directory*>(elm) == this->currentDirectory->getParent())
                    {
                        this->currentDirectory = dynamic_cast<Directory*>(elm);
                        return;
                    }
                }
            }
        }

        //cout << "NOLİYA1" << endl;
        //if(this->inputs[1] == ".")
        //{
        //    cout << "NOLİYA2" << endl;
        //    return ;
        //}
        //else if(this->inputs[1] == "..")
        //{
        //    cout << "NOLİYA3" << endl;
        //    if(this->currentDirectory == this->root)
        //    {
        //        cout << "NOLİYA4" << endl;
        //        throw runtime_error("You are in the root path!\n");
        //        return ;
        //    }
        //    else if(this->currentDirectory->getParent()->getPath() == this->root->getPath())
        //    {
        //        cout << "NOLİYA5" << endl;
        //        cout << currentDirectory->getName() << endl;
        //        cout << currentDirectory->getParent()->getName() << endl;
        //        this->currentDirectory = this->root;
        //        return ;
        //    }
        //    else
        //    {
        //        for(const auto &elm : this->root->getElements())
        //        {
        //             cout << "NOLİYA6" << endl;
//
        //            if(dynamic_cast<Directory*>(elm))
        //            {
        //                cout << "NOLİYA7" << endl;
//
        //                if(dynamic_cast<Directory*>(elm)->getParent() == this->currentDirectory->getParent())
        //                {
        //                    cout << "NOLİYA8" << endl;
        //                    cout << currentDirectory->getName() << endl;
        //                    cout << currentDirectory->getParent()->getName() << endl;
        //                    Directory *ptr = dynamic_cast<Directory*>(elm);
        //                    this->currentDirectory = this->currentDirectory->getParent();
        //                    return ;
        //                }
        //            }
        //        }
        //    }
        //}
        //return ;
        return ;
    }


    void Shell::cd_checkPaths()
    {
        for(auto elm : this->root->getElements())
        {
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
            if(this->inputs[1] == elm->getName() && dynamic_cast<Directory*>(elm))
            {
                if(elm->getPath() == currentDirectory->getPath())
                {
                    cout << this->currentDirectory->getPath() << " " << this->currentDirectory->getName() << endl;
                    this->currentDirectory = dynamic_cast<Directory*>(elm);
                    cout << this->currentDirectory->getPath() << " " << this->currentDirectory->getName() << endl;
                    return ;
                }
                else
                {
                    throw invalid_argument("Error!\n");
                }
            }
        }
        return ;
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