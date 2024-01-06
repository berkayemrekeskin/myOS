#ifndef SHELL_HPP
#define SHELL_HPP

#include "files.hpp"
#include "directory_file.hpp"

using namespace std;
using namespace FileSystemKeskin;

#define MAX_SIZE = 100000

namespace ShellKeskin
{
    class Shell
    {
        public:
            Shell();
            ~Shell();
            void startOS(); // Read all files from txt file and save.
            //Input functions
            void getInput(string &input);
            void transformInput(string &input);
            void splitLine(const string &input, vector<string> &vect);
            void checkInput();
            //Element add & delete
            void addToOS();
            void addToFiles(File* obj);
            void addToFolders();
            void assignParents();
            //void removeElement();

            //Helpers
            const string absolutePath(const File *obj, const Directory* current) const;
            //Commands
            void ls(const Directory *current); // #pathleri kontrol et, cur.path ile aynıysa isimleri printle
            void lsRecursive(const Directory *current); // #currentpathden pathVectorün son elemanına kadar(en uzunu) her pathe gelince elemanlarını printle (path ismiyle birlikte)
            void lsPrint(const Directory *current);            
            void mkdir();   // #yeni bir directory obj oluştur, filesysteme pushla, myOS yazdır
            void rmdir(Directory* removed); // if mkdir gets input that os has, then delete that folder with its elements recurively
            void rm(); // #adı girilen fileı filesystemde bul, filesystemden sil, filesystemi en baştan myOS yazdır silinmiş haliyle yazılsın
            void cp(); // #file adını al, regularFile obj oluştur, fileTOVector kullan, filesysteme pushla, myOS yazdır
            void cp_directory(Directory* newDirectory,Directory* current);
            void link(); // #file adını al, filesystemde regFileı bul, linkedFile obj oluştur, linkedFile obj pointerıyla regFile pointle, filesysteme pushla, myOS yazdır 
            void cd(); //directory name al, pathVectorde varsa ve doğru komut girdiyse (.. etc) current pathi değiştir
            void cd_checkInputs(const Directory *temp_parent);
            void cd_checkPaths();
            void cat(); // eleman yazdır


        private:
            Directory* files;
            Directory* currentDirectory; // #Anlık pathi tutar
            Directory* rootDirectory;
            size_t fileSize;
            vector<string> inputs;
    };

}

#endif