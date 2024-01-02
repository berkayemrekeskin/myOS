#include "files.hpp"
#include "shell.hpp"
using namespace std;
using namespace FileSystemKeskin;


//TODO : HER BOKU DEĞİŞTİR
namespace ShellKeskin
{
    string Shell::rootPath = "/";

    Shell::Shell() 
    {
        filesystem.setName("Filesystem Directory");
        filesystem.setPath("/");
        root->setName("Root");
        root->setPath("/");
        currentPath = &filesystem;
    }
    Shell::~Shell() 
    {
        for(auto elm : this->filesystem.getElements())
        {
            delete elm;
        }
        delete currentPath;
        delete root;
    }
    void Shell::startOS()
    {
        int lineCounter = 0;
        int oldLineCounter = 0;
        int totalLine = 0;
        string line;
        vector<string> subStrings;

        ifstream os("OSKeskin.txt"); //Count the total number of lines in OS
        if(!os.is_open())
        {
            //cout << "ishere" << endl;
            throw invalid_argument("OS cannot open!\n");
        }
        while(getline(os,line))
        {
            totalLine++;
        }
        os.close();

        os.open("OSKeskin.txt");
        if(!os.is_open())
        {
            throw invalid_argument("OS cannot open!\n");
        }
        while(lineCounter <= totalLine-1)
        {
            while(oldLineCounter < lineCounter)
            {
                string temp;
                getline(os,line);
                line.clear();
                oldLineCounter++;
            }

            line.clear();
            getline(os,line);
            subStrings.clear();

            this->splitLine(line,subStrings);
            
            if(subStrings[0] == "ft_regular")
            {
                RegularFile * obj = new RegularFile;
                obj->readFromSystem(lineCounter);
                this->addToFileSystem(obj);
                this->addToPath(obj);
                //#BUNU DİREKT FONKSİYON YAP
            }
            else if(subStrings[0] == "ft_linked")
            {
                SoftLinkedFile *obj = new SoftLinkedFile;
                obj->readFromSystem(lineCounter);
                for(auto &elm : this->filesystem.getElements())
                {
                    if(elm->getName() == obj->getLinkedName() && elm->getPath() == obj->getPath())
                    {
                        obj->setPointer(dynamic_cast<RegularFile*>(elm));
                    }
                }
                this->addToFileSystem(obj);
                this->addToPath(obj);
                
            }
            else if(subStrings[0] == "ft_directory")
            {
                Directory *obj = new Directory;
                obj->readFromSystem(lineCounter);
                this->addToFileSystem(obj);
                this->addToPath(obj);
            }
            else
            {
                throw invalid_argument("File type unknown!\n");
            }
            lineCounter++;
            oldLineCounter++;
        }
        if(this->filesystem.getElements().empty())
        {
            Directory *obj = new Directory;
            obj->setName(".");
            obj->setPath("/");
            this->addToFileSystem(obj);
        }

        for(auto &folders : this->filesystem.getElements())
        {
            for(const auto &elements : this->filesystem.getElements())
            {
                if(dynamic_cast<Directory *>(folders))
                {
                    if(folders->getPath() == "/")
                    {
                        if(elements->getPath() == "/" + folders->getName())
                        {
                            Directory *obj = dynamic_cast<Directory *>(folders);
                            obj->addElements(elements);
                        }
                    }
                    else
                    {
                        if(elements->getPath() == (folders->getPath() + "/" + folders->getName()))
                        {
                            Directory *obj = dynamic_cast<Directory *>(folders);
                            obj->addElements(elements);
                        }
                    }
                }
            }
        }
        for(auto elm : this->currentPath->getElements())
        {
            cout << elm->getName() << " ";
        }
        cout << endl;

        os.close();
        this->checkInput();
    }
    void Shell::addToFileSystem(File *elm)
    {
        if(elm->getType() == "ft_regular")
        {
            RegularFile * regularPtr = dynamic_cast<RegularFile*>(elm);
            //current pathi directory olarak tuttuktan sonra kontrol edip ona ekle
            this->filesystem.getElements().push_back(regularPtr);
        }
        else if(elm->getType() == "ft_linked")
        {
            SoftLinkedFile * linkedPtr = dynamic_cast<SoftLinkedFile*>(elm);
            //current pathi directory olarak tuttuktan sonra kontrol edip ona ekle
            this->filesystem.getElements().push_back(linkedPtr);
        }
        else if(elm->getType() == "ft_directory")
        {
            Directory * directoryPtr = dynamic_cast<Directory*>(elm);
            this->filesystem.getElements().push_back(directoryPtr);
        }
        else
        {
            throw invalid_argument("Invalid file type!\n");
        }
    }
    void Shell::addToPath(const File* obj)
    {
        if(!this->paths.empty())
        {
            if(!isIn(obj->getPath(),this->paths))
            {
                this->paths.push_back(obj->getPath());
            }
        }
        else
        {
            this->paths.push_back(obj->getPath());
        }
    }
    void Shell::addToOS(Directory * current)
    {
        ofstream os("OSKeskin.txt");
        if(!os.is_open())
        {
            throw invalid_argument("OS cannot open!\n");
        }
        addToOSRecursive(current);
    }
    void Shell::addToOSRecursive(Directory *current)
    {
        bool flag = false;
        for(auto elm : current->getElements())
        {
            flag = true;
            elm->printToSystem();
            if(dynamic_cast<Directory *>(elm))
            {
                addToOSRecursive(dynamic_cast<Directory *>(elm));
            }
        }
        if(flag == true)
            return ;
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
    void Shell::splitLine(const string &input, vector<string> &vect) //#Hata olabilir (düzelttim glb currentword != " ")
    {
        stringstream ss(input);
        string word;
        while(ss >> word)
        {
            vect.push_back(word);
        }
    }
    void Shell::checkInput()
    {
        string input;
        while(true)
        {
            this->transformInput(input); //inputs are in the inputs vector now
            if(input == "quit")
            {
                this->addToOS(root);
                return ;
            }
            if(this->inputs[0] == "ls")
                this->ls(this->currentPath);
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
            inputs.clear();
        }
        
    }
    void Shell::ls(Directory *current)
    {

        //cout << "(LS) (1) Current Path: " << this->currentPath << endl; 
        //Check if '-R' used
        if(this->inputs.size() == 2 && this->inputs[1] == "-R")
        {
            bool isOver = false;
            if(dynamic_cast<Directory *>(current))
            {
                this->lsRecursive(current,isOver);
            }
        }
        else if(this->inputs.size() == 1)
        {
            for(auto elm : this->filesystem.getElements())
                current->addElements(elm);
            lsPrint(current);
        }
        else
            throw invalid_argument("ls !\n");
    }
    void Shell::lsRecursive(const Directory *current, bool &isOver)
    {
        for(auto elm : current->getElements())
        {
            isOver = true;
            if(dynamic_cast<Directory *>(elm))
            {
                isOver = false;
                lsRecursive(dynamic_cast<Directory*>(elm),isOver);
                lsPrint(dynamic_cast<Directory*>(elm));
            }
            if(isOver)
                break;
        }
    }
    void Shell::lsPrint(const Directory *current)
    {
        if(!current->getElements().empty())
        {
            for(auto elm : current->getElements())
            {
                if(current->getPath() == elm->getPath())
                {
                    if(elm->getType() == "ft_directory")
                    {
                        cout << "D " << elm->getName() << "        " << elm->getTime() << endl;
                    }
                    else if(elm->getType() == "ft_regular")
                    {
                        RegularFile * obj = new RegularFile;
                        obj = dynamic_cast<RegularFile *>(elm);
                        cout << "F " << obj->getName() << "        " << obj->getTime() << "        " << obj->getSize() << endl;
                    }
                    else if(elm->getType() == "ft_linked")
                    {
                        SoftLinkedFile * obj;
                        obj = dynamic_cast<SoftLinkedFile *>(elm);
                        cout << "F " << obj->getName() << "        " << obj->getTime() << "        " << obj->getPointer()->getSize() << endl;
                    }
                    
                }
            }
        }
    }
    void Shell::mkdir()
    {
        //std::cout << "(MKDIR) Current Path: " << this->currentPath << endl; 
        if(this->inputs.size() == 1)
        {
            throw invalid_argument("No file name given!\n");
        }
        else
        {
            for(auto elm : filesystem.getElements())
            {
                if(this->inputs[1] == elm->getName() && currentPath->getPath() == elm->getPath()) //If names and paths are same
                {
                    throw invalid_argument("There is already an existing directory!\n");
                }
            } 

            //Create directory on current path
            Directory *obj = new Directory;
            obj->setName(inputs[1]);
            obj->setPath(this->currentPath->getPath());
            this->addToFileSystem(obj);
            //cout << "(MKDIR) Current Path: " << obj->getPath() << endl;

            Directory *current = new Directory;
            current->setName(".");
            if(currentPath->getPath() != "/")
                current->setPath(currentPath->getPath() + "/" + this->inputs[1]);
            else
                current->setPath(currentPath->getPath() + this->inputs[1]);
            this->addToFileSystem(current);
            //cout << "(MKDIR) Created directory inside Path: " << current->getPath() << endl;

            Directory *father = new Directory;
            father->setName("..");
            if(currentPath->getPath() != "/")
                father->setPath(currentPath->getPath() + "/" + this->inputs[1]);
            else
                father->setPath(currentPath->getPath() + this->inputs[1]);
            //cout << "(MKDIR) Father Path: " << father->getPath() << endl;
            this->addToFileSystem(father);

            obj->addElements(current);
            obj->addElements(father);

            for(auto elm : this->filesystem.getElements())
            {
                cout << elm->getName() << " " << elm->getPath() << endl;
            }
            
        }
    }
    void Shell::rm()
    {
        if(this->inputs.size() == 1)
        {
            throw invalid_argument("No file name given!\n");
        }
        else
        {
            bool flag = false;
            for(auto &elm : this->filesystem.getElements())
            {
                if(this->inputs[1] == elm->getName() && this->currentPath->getPath() == elm->getPath())
                {
                    //elm delete
                    cout << elm->getName() << " bulundu" << endl;
                    remove("OSKeskin.txt");
                    for(const auto &elm : this->filesystem.getElements())
                    {
                    }
                    flag = true;
                }
            }
            if(!flag)
            {
                throw invalid_argument("File does not exist in this path!\n");
            }
        }
    }
    void Shell::cp()
    {
        for(auto &elm : this->filesystem.getElements())
        {
            if(this->inputs[2] == elm->getName() && elm->getPath() == this->currentPath->getPath())
            {
                RegularFile *obj = new RegularFile;
                obj = dynamic_cast<RegularFile*>(elm);
                obj->fileToVector(this->inputs[1]);
                elm = obj;

                remove("OSKeskin.txt");
                this->addToOS(currentPath);
                return ;
            }
        }

        RegularFile *obj = new RegularFile;
        obj->setPath(this->currentPath->getPath());
        obj->setName(this->inputs[2]);
        obj->fileToVector(this->inputs[1]);
        this->addToFileSystem(obj);
        return ;
        
    }
    void Shell::link()
    {
        for(auto &elm : this->filesystem.getElements())
        {
            if(this->inputs[1] == elm->getName() && elm->getPath() == this->currentPath->getPath())
            {
                //RegularFile *temp = new RegularFile;
                //temp = dynamic_cast<RegularFile *>(elm); // #Şunlarda error check yap
                SoftLinkedFile * obj = new SoftLinkedFile;
                obj->setName(this->inputs[2]);
                obj->setPath(this->currentPath->getPath());
                obj->setLinkedName(elm->getName());
                obj->setPointer(dynamic_cast<RegularFile*>(elm));
                this->addToFileSystem(obj);

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
            this->currentPath->setPath(this->rootPath);
            return ;
        }
        if(this->inputs[1] == "." || this->inputs[1] == "..")
        {
            this->cd_checkInputs();
        }
        else
        {
            this->cd_checkPaths();
        }
    }
    void Shell::cd_checkInputs()
    {
        if(this->inputs[1] == ".")
        {
            return ;
        }
        else if(this->inputs[1] == "..")
        {
            if(this->currentPath->getPath() == "/")
            {
                //cout << "(CD_CHECK_INPUTS) (1) .. :  " << endl;
                return ;
            }
            else if(this->currentPath->getPath().find_last_of("/") == 0)
            {
                //cout << "(CD_CHECK_INPUTS) (2) .. :  " << this->currentPath.find_last_of("/") <<  " " << this->currentPath.substr(0,this->currentPath.find_last_not_of("/") + 1) << endl;
                this->currentPath->setPath("/");
            }
            else
            {
                //cout << "(CD_CHECK_INPUTS) (3) .. :  " << this->currentPath.find_last_of("/") <<  " " << this->currentPath.substr(0,this->currentPath.find_last_of("/")) << endl;
                this->currentPath->setPath(this->currentPath->getPath().substr(0, this->currentPath->getPath().find_last_of("/"))); 
            }
        }
        return ;
    }
    void Shell::cd_checkPaths()
    {
        for(auto elm : this->filesystem.getElements())
        {
            //cout << "(CD_CHECK_PATHS) ELM name: " << elm->getName() << "  (CD_CHECK_PATHS) ELM path: " << elm->getPath() << endl;
            //cout << "(CD_CHECK_PATHS) CHECKING : " << elm->getPath().substr(0,elm->getPath().find_last_not_of("/") + 1) << endl; 
            if(this->inputs[1][0] == '/')
            {
                for(const auto &elm : this->paths)
                {
                    if(elm == inputs[1])
                    {
                        this->currentPath->setPath(inputs[1]);
                        return ;
                    }
                }
            }
            if(this->inputs[1] == elm->getName() && elm->getType() == "ft_directory")
            {
                if(elm->getPath() == "/" && elm->getPath() == this->currentPath->getPath())
                {
                    cout << "(CD_CHECKP_ATHS - CURRENT PATH) (1): " << this->currentPath->getPath() << endl;
                    this->currentPath->setPath(this->currentPath->getPath() + elm->getName());
                    cout << "(CD_CHECKP_ATHS - CURRENT PATH) (1) NEW: " << this->currentPath->getPath() << endl;
                    return ;

                }
                else if(elm->getPath() != "/" && elm->getPath().substr(0,elm->getPath().find_last_not_of("/") + 1) == this->currentPath->getPath())
                {
                    cout << "(CD_CHECKP_ATHS - CURRENT PATH) (2): " << this->currentPath->getPath() << endl;
                    this->currentPath->setPath(this->currentPath->getPath() + "/" + elm->getName()); //#şunu fonksiyona çevir
                    cout << "(CD_CHECKP_ATHS - CURRENT PATH) (2) NEW: " << this->currentPath->getPath() << endl;
                    return ;
                }
            }
        }
    }
    bool Shell::isDirectory()
    {
        cout << "(isDirectory) Name of dir: " << this->inputs[1] << endl;
        for(auto elm : this->filesystem.getElements())
        {
            if(this->inputs[1] == elm->getName() && elm->getType() == "ft_directory")
            {
                return true;
            }
        }
        return false;
    }
    void Shell::cat()
    {
        for(auto elm : this->filesystem.getElements())
        {
            if(this->inputs[1] == elm->getName() && elm->getPath() == this->currentPath->getPath())
            {
                elm->showContents();
            }
        }
    }
}
  