#ifndef REGULAR_FILE_HPP
#define REGULAR_FILE_HPP

#include "../includes/files.hpp"
using namespace std;

namespace FileSystemKeskin
{
    class RegularFile : public File
    {
        public:
            RegularFile();

            virtual void printToSystem() override; // Overrides the virtual function
            virtual void readFromSystem(int &line) override; // Overrides the virtual function
            virtual void showContents() const override; // Overrides the virtual function
            void fileToVector(string filePath); // Reads from the OSKeskin.txt & adds to the data

            // Getters & setters
            size_t getSize() const;
            vector<string> getData() const;
            void setData(const vector<string> &data);
            
            // Iterator Class
            class iterator
            {
                public:
                    iterator(vector<string>::iterator ptr) : current(ptr) {}
                    string& operator*() const { return *current; }
                    iterator& operator++() 
                    {
                        ++current;
                        return *this;
                    }
                    iterator operator++(int)
                    {
                        vector<string>::iterator temp = current;
                        ++current;
                        return iterator(temp);
                    }
                    bool operator==(const iterator& oth) const { return current == oth.current; }
                    bool operator!=(const iterator& oth) const { return current != oth.current; }

                private:
                    vector<string>::iterator current;
            };
            const iterator begin() const
            {
                return iterator(this->getData().begin());
            }
            const iterator end() const
            {
                return iterator(this->getData().end());
            }
            
        private:
            size_t size; // Holds the size of the data, i used this to calculate the 10MB size
            vector<string> data;
    };
}

#endif