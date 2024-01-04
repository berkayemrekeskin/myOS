#ifndef SHELL_HPP
#define SHELL_HPP


#include "files.hpp"
using namespace std;
using namespace FileSystemKeskin;

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
            void addToOS(Directory *path);
            void addToOSRecursive(Directory *path);
            void addToRoot(File* obj);
            void addToFolders();
            //void removeElement();

            //Helpers
            const string absolutePath(const File *obj) const;
            //Commands
            void ls(Directory *current); // #pathleri kontrol et, cur.path ile aynıysa isimleri printle
            void lsRecursive(const Directory *path, bool &isOver); // #currentpathden pathVectorün son elemanına kadar(en uzunu) her pathe gelince elemanlarını printle (path ismiyle birlikte)
            void lsPrint(const Directory *path);            
            void recursivePrint(const Directory *current);
            void mkdir();   // #yeni bir directory obj oluştur, filesysteme pushla, myOS yazdır
            void rm(); // #adı girilen fileı filesystemde bul, filesystemden sil, filesystemi en baştan myOS yazdır silinmiş haliyle yazılsın
            void cp(); // #file adını al, regularFile obj oluştur, fileTOVector kullan, filesysteme pushla, myOS yazdır
            void link(); // #file adını al, filesystemde regFileı bul, linkedFile obj oluştur, linkedFile obj pointerıyla regFile pointle, filesysteme pushla, myOS yazdır 
            void cd(); //directory name al, pathVectorde varsa ve doğru komut girdiyse (.. etc) current pathi değiştir
            void cd_checkInputs();
            void cd_checkPaths();
            void cat(); // eleman yazdır

            //Iterator Class
            class Iterator
            {
                public:
                    Iterator(File *fptr) : ptr(fptr) {}
                    //...
                private:
                    File * ptr;
            };

        private:
            Directory * currentDirectory; // #Anlık pathi tutar
            Directory * root;
            vector<string> inputs;
    };

}




#endif