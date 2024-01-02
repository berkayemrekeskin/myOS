#ifndef FILES_HPP
#define FILES_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono> // #belki cpp11 le olmayabilir kontrol et
using namespace std;

/*TODO*/
/*

*/

namespace FileSystemKeskin
{

    class File
    {
        public: 

            File(string _type);
            File(string name, string path, string type);
            virtual ~File();    

            virtual void printToSystem(); //Saving to the OS
            virtual void readFromSystem(int &line) = 0; //Reading from the OS
            virtual void showContents() const = 0; //Pure virtual 'cat' function helper
            void splitLine(const string &input, vector<string> &vect);

            const string getName() const;
            const string getPath() const;
            const string getType() const;
            const string getTime() const;
            void setName(const string &name);
            void setPath(const string &path);
            void setType(const string &type);
            void setTime(const string &time);

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
            string name;
            string path;
            string type;
            string time;
    };

    class RegularFile : public File
    {
        public:
            RegularFile();
            RegularFile(string name, string path, string type);
            RegularFile(const RegularFile &oth);
            RegularFile & operator=(const RegularFile &oth);
            virtual ~RegularFile();

            virtual void printToSystem() override;
            virtual void readFromSystem(int &line) override;
            virtual void showContents() const override;

            void fileToVector(string filePath);
            const size_t getSize() const;

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
            vector<string> data; //#datayı line line al (if data[i][data[i].length()-1] == '$' end reading)
    };
    class SoftLinkedFile : public File
    {
        public:
            SoftLinkedFile();
            SoftLinkedFile(string name, string path, string type, string linkedName);
            SoftLinkedFile(const SoftLinkedFile &oth);
            SoftLinkedFile & operator=(const SoftLinkedFile &oth);
            virtual ~SoftLinkedFile();

            virtual void printToSystem() override;
            virtual void readFromSystem(int &line) override;
            virtual void showContents() const override;

            void setPointer(RegularFile *obj);
            void setLinkedName(const string &name);
            RegularFile * getPointer() const;
            const string getLinkedName() const;

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
            string linkedFileName;
            RegularFile *linkedRegular;
    };

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
            vector<File *> elements;
    };

    template<typename T> 
    bool isIn(const T& element, const vector<T> vect)
    {
        for(auto elm : vect)
        {
            if(elm == element)
                return true;
        }
        return false;
    }

}

#endif //FILES.HPP