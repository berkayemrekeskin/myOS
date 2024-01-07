#ifndef FILES_HPP
#define FILES_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
using namespace std;

namespace FileSystemKeskin
{
    class File
    {
        public: 

            File(string _type);
            File(string name, string path, string type);
            virtual ~File() = 0;    

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

        private:
            string name;
            string path;
            string type;
            string time;
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