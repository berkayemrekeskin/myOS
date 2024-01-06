#ifndef DIRECTORY_FILE_HPP
#define DIRECTORY_FILE_HPP

#include "files.hpp"
using namespace std;

namespace FileSystemKeskin
{
    class Directory : public File
    {
        public:
            Directory();
            Directory(string name, string path, string type);
            Directory(const Directory &oth);
            Directory & operator=(const Directory &oth);
            virtual ~Directory();
            
            virtual void printToSystem() override;
            virtual void readFromSystem(int &line) override;
            virtual void showContents() const override;

            //void fileToVector(strinag filePath);
            void addElements(File *elm);
            vector<File *> getElements() const;
            void setElements(const vector<File*> &other);
            void setParent(Directory* parent);
            Directory* getParent() const;
            //void removeElements();

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
            Directory * parentDirectory;
            vector<File *> elements;

    };
}
    
#endif //DIRECTORY_FILE_HPP