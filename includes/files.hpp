#ifndef FILES_HPP
#define FILES_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <stdexcept>
using namespace std;

/*  
    NOTE FOR MY IMPLEMENTATION:
        - In the pdf file, it says that some functions should be pure virtual and
        overridden in the other classes. I done that but my functions names are not 
        same with the commands. For example, my showContents function handles the 
        'cat' command but i named them in my Shell class.
*/

namespace FileSystemKeskin
{
    class File
    {
        public: 

            File();
            File(string _type);
            virtual ~File();    

            virtual void printToSystem(); // Printing to the OSKeskin.txt file
            virtual void readFromSystem(int &line) = 0; // Reading from the OSKeskin.txt file
            virtual void showContents() const = 0; // Pure virtual 'cat' function.
            void splitLine(const string &input, vector<string> &vect); // Helper for reading & getting inputs

            // Getters & setters
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

    // Templated searching function for elements.
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