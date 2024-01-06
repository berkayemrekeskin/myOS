#ifndef REGULAR_FILE_HPP
#define REGULAR_FILE_HPP

#include "files.hpp"
using namespace std;

namespace FileSystemKeskin
{
    class RegularFile : public File
    {
        public:
            RegularFile();
            RegularFile(string name, string path, string type);


            virtual void printToSystem() override;
            virtual void readFromSystem(int &line) override;
            virtual void showContents() const override;

            void fileToVector(string filePath);
            const size_t getSize() const;
            void setData(const vector<string> &data);
            const vector<string> getData() const;
            
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
            size_t size;
            vector<string> data;
    };
}

#endif