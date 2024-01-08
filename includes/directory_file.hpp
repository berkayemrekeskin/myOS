#ifndef DIRECTORY_FILE_HPP
#define DIRECTORY_FILE_HPP

#include "../includes/files.hpp"
using namespace std;

namespace FileSystemKeskin
{
    class Directory : public File
    {
        public:
            Directory();
            
            virtual void printToSystem() override; // Overrides the virtual function
            virtual void readFromSystem(int &line) override; // Overrides the virtual function
            virtual void showContents() const override; // Overrides the virtual function

            void addElements(File *elm); // Adding elements to the vector<File*>elements
            void removeFile(File *file);

            // Setters & getters
            void setElements(const vector<File*> &other);
            void setParent(Directory* parent);
            Directory* getParent() const;
            vector<File *> getElements() const;
            
            // Iterator class
            class iterator
            {
                public:
                    iterator(vector<File*>::iterator ptr) : current(ptr) {}
                    File*& operator*() const { return *current; }
                    iterator& operator++() 
                    {
                        ++current;
                        return *this;
                    }
                    iterator operator++(int)
                    {
                        vector<File*>::iterator temp = current;
                        ++current;
                        return iterator(temp);
                    }
                    bool operator==(const iterator& oth) const { return current == oth.current; }
                    bool operator!=(const iterator& oth) const { return current != oth.current; }

                private:
                    vector<File*>::iterator current;
            };
            iterator begin() const
            {
                return iterator(this->getElements().begin());
            }
            iterator end() const
            {
                return iterator(this->getElements().end());
            }
            
        private:
            Directory * parentDirectory; // This holds the parent of the directory, helps to 'cd' command and others
            vector<File *> elements;
    };
}
    
#endif //DIRECTORY_FILE_HPP