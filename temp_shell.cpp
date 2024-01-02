#include "files.hpp"
#include "shell.hpp"
using namespace std;
using namespace FileSystemKeskin;

namespace ShellKeskin
{

    string Shell::rootPath = "/";

    Shell::Shell() : currentPath("/") {}
    Shell::~Shell() 
    {
        for(auto elm : this->filesystem)
        {
            delete elm;
        }
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
                for(auto &elm : this->filesystem)
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
        if(this->filesystem.empty())
        {
            Directory *obj = new Directory;
            obj->setName(".");
            obj->setPath("/");
            obj->printToSystem();
            this->addToFileSystem(obj);
            this->addToPath(obj);
        }
        for(auto &folders : this->filesystem)
        {
            for(const auto &elements : this->filesystem)
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
        os.close();
        this->checkInput();
    }
    void Shell::addToFileSystem(File *elm)
    {
        if(elm->getType() == "ft_regular")
        {
            RegularFile * regularPtr = dynamic_cast<RegularFile*>(elm);
            //current pathi directory olarak tuttuktan sonra kontrol edip ona ekle
            this->filesystem.push_back(regularPtr);
        }
        else if(elm->getType() == "ft_linked")
        {
            SoftLinkedFile * linkedPtr = dynamic_cast<SoftLinkedFile*>(elm);
            //current pathi directory olarak tuttuktan sonra kontrol edip ona ekle
            this->filesystem.push_back(linkedPtr);
        }
        else if(elm->getType() == "ft_directory")
        {
            Directory * directoryPtr = dynamic_cast<Directory*>(elm);
            this->filesystem.push_back(directoryPtr);
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
    void Shell::addToOS() const
    {
        ofstream os("OSKeskin.txt");
        if(!os.is_open())
        {
            throw invalid_argument("OS cannot open!\n");
        }
        for(auto elm : this->filesystem)
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
                return ;
            if(this->inputs[0] == "ls")
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
                throw std::invalid_argument("Wrong command! Command not found!\n");
            input.clear();
            inputs.clear();
        }
        
    }
    void Shell::ls()
    {
        //cout << "(LS) (1) Current Path: " << this->currentPath << endl; 

        //Check if '-R' used
        if(this->inputs.size() == 2 && this->inputs[1] == "-R")
        {
            /*File * t_currentPath;
            for(auto elm : this->filesystem)
            {
                if(currentPath == elm->getPath())
                {
                   t_currentPath = lsRecursive();
                   cout << t_currentPath->getName() << endl;
                }
            }*/
        }
        else if(this->inputs.size() == 1)
        {
            for(auto elm : this->filesystem)
            {
                //cout << "(LS) Element Name: " << elm->getName() << " (LS) Element Path: " << elm->getPath() << endl;
                if(currentPath == elm->getPath())
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
        else
        {
            throw invalid_argument("Wrong parameter!\n");
        }
    }
    /*File * Shell::lsRecursive()
    {
        File * obj;
        for(int i = 0; i < this->filesystem.size(); i++)
            {
                obj = filesystem[i];
                if(currentPath == obj->getPath())
                {
                   lsRecursive()
                }
            }
        return obj; 
    }*/
    void Shell::mkdir()
    {
        //std::cout << "(MKDIR) Current Path: " << this->currentPath << endl; 
        if(this->inputs.size() == 1)
        {
            throw invalid_argument("No file name given!\n");
        }
        else
        {
            for(auto elm : filesystem)
            {
                if(this->inputs[1] == elm->getName() && currentPath == elm->getPath()) //If names and paths are same
                {
                    throw invalid_argument("There is already an existing directory!\n");
                }
            }

            //Create directory on current path
            Directory *obj = new Directory;
            obj->setName(inputs[1]);
            obj->setPath(this->currentPath);
            obj->printToSystem();
            this->addToFileSystem(obj);
            //cout << "(MKDIR) Current Path: " << obj->getPath() << endl;

            Directory *current = new Directory;
            current->setName(".");
            if(currentPath != "/")
                current->setPath(currentPath + "/" + this->inputs[1]);
            else
                current->setPath(currentPath + this->inputs[1]);
            current->printToSystem();
            this->addToFileSystem(current);
            //cout << "(MKDIR) Created directory inside Path: " << current->getPath() << endl;

            Directory *father = new Directory;
            father->setName("..");
            if(currentPath != "/")
                father->setPath(currentPath + "/" + this->inputs[1]);
            else
                father->setPath(currentPath + this->inputs[1]);
            father->printToSystem();
            //cout << "(MKDIR) Father Path: " << father->getPath() << endl;
            this->addToFileSystem(father);

            obj->addElements(current);
            obj->addElements(father);
            
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
            for(auto &elm : this->filesystem)
            {
                if(this->inputs[1] == elm->getName() && this->currentPath == elm->getPath())
                {
                    //elm delete
                    cout << elm->getName() << " bulundu" << endl;
                    remove("OSKeskin.txt");
                    for(const auto &elm : this->filesystem)
                    {
                        elm->printToSystem();
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
        for(auto &elm : this->filesystem)
        {
            if(this->inputs[2] == elm->getName() && elm->getPath() == this->currentPath)
            {
                RegularFile *obj = new RegularFile;
                obj = dynamic_cast<RegularFile*>(elm);
                obj->fileToVector(this->inputs[1]);
                elm = obj;

                remove("OSKeskin.txt");
                this->addToOS();
                return ;
            }
        }

        RegularFile *obj = new RegularFile;
        obj->setPath(this->currentPath);
        obj->setName(this->inputs[2]);
        obj->fileToVector(this->inputs[1]);
        this->addToFileSystem(obj);
        return ;
        
    }
    void Shell::link()
    {
        for(auto &elm : this->filesystem)
        {
            if(this->inputs[1] == elm->getName() && elm->getPath() == this->currentPath)
            {
                //RegularFile *temp = new RegularFile;
                //temp = dynamic_cast<RegularFile *>(elm); // #Şunlarda error check yap
                SoftLinkedFile * obj = new SoftLinkedFile;
                obj->setName(this->inputs[2]);
                obj->setPath(this->currentPath);
                obj->setLinkedName(elm->getName());
                obj->setPointer(dynamic_cast<RegularFile*>(elm));
                obj->printToSystem();
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
            this->currentPath = this->rootPath;
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
            if(this->currentPath == "/")
            {
                //cout << "(CD_CHECK_INPUTS) (1) .. :  " << endl;
                return ;
            }
            else if(this->currentPath.find_last_of("/") == 0)
            {
                //cout << "(CD_CHECK_INPUTS) (2) .. :  " << this->currentPath.find_last_of("/") <<  " " << this->currentPath.substr(0,this->currentPath.find_last_not_of("/") + 1) << endl;
                this->currentPath = "/";
            }
            else
            {
                //cout << "(CD_CHECK_INPUTS) (3) .. :  " << this->currentPath.find_last_of("/") <<  " " << this->currentPath.substr(0,this->currentPath.find_last_of("/")) << endl;
                this->currentPath = this->currentPath.substr(0, this->currentPath.find_last_of("/")); 
            }
        }
        return ;
    }
    void Shell::cd_checkPaths()
    {
        for(auto elm : this->filesystem)
        {
            //cout << "(CD_CHECK_PATHS) ELM name: " << elm->getName() << "  (CD_CHECK_PATHS) ELM path: " << elm->getPath() << endl;
            //cout << "(CD_CHECK_PATHS) CHECKING : " << elm->getPath().substr(0,elm->getPath().find_last_not_of("/") + 1) << endl; 
            if(this->inputs[1][0] == '/')
            {
                for(const auto &elm : this->paths)
                {
                    if(elm == inputs[1])
                    {
                        this->currentPath = inputs[1];
                        return ;
                    }
                }
            }
            if(this->inputs[1] == elm->getName() && elm->getType() == "ft_directory")
            {
                if(elm->getPath() == "/" && elm->getPath() == this->currentPath)
                {
                    //cout << "(CD_CHECKP_ATHS - CURRENT PATH) (1): " << this->currentPath << endl;
                    this->currentPath += elm->getName();
                    //cout << "(CD_CHECKP_ATHS - CURRENT PATH) (1) NEW: " << this->currentPath << endl;
                    return ;

                }
                else if(elm->getPath() != "/" && elm->getPath().substr(0,elm->getPath().find_last_not_of("/") + 1) == this->currentPath)
                {
                    //cout << "(CD_CHECKP_ATHS - CURRENT PATH) (2): " << this->currentPath << endl;
                    this->currentPath += "/" + elm->getName(); //#şunu fonksiyona çevir
                    //cout << "(CD_CHECKP_ATHS - CURRENT PATH) (2) NEW: " << this->currentPath << endl;
                    return ;
                }
            }
        }
    }
    bool Shell::isDirectory()
    {
        cout << "(isDirectory) Name of dir: " << this->inputs[1] << endl;
        for(auto elm : this->filesystem)
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
        for(auto elm : this->filesystem)
        {
            if(this->inputs[1] == elm->getName() && elm->getPath() == this->currentPath)
            {
                elm->showContents();
            }
        }
    }
}
 