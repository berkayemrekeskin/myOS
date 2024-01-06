#ifndef LINKED_FILE_HPP
#define LINKED_FILE_HPP

#include "files.hpp"
#include "regular_file.hpp"
using namespace std;

namespace FileSystemKeskin
{
    class SoftLinkedFile : public File
    {
        public:
            SoftLinkedFile();
            SoftLinkedFile(string name, string path, string type, string linkedName);


            virtual void printToSystem() override;
            virtual void readFromSystem(int &line) override;
            virtual void showContents() const override;

            void setPointer(RegularFile *obj);
            void setLinkedPath(const string &path);
            RegularFile * getPointer() const;
            const string getLinkedPath() const;

            //Iterator Class
            class Iterator
            {
                public:
                    Iterator(string *fptr) : ptr(fptr) {}
                    //...
                private:
                    string * ptr;
            };
            
        private:
            RegularFile *linkedRegular;
            string linkedFilePath;
    };
}

#endif //LINKED_FILE_HPP