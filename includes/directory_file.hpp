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
            
            virtual void printToSystem() override;
            virtual void readFromSystem(int &line) override;
            virtual void showContents() const override;

            void addElements(File *elm);
            vector<File *> getElements() const;
            void setElements(const vector<File*> &other);
            void setParent(Directory* parent);
            Directory* getParent() const;

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
            Directory * parentDirectory;
            vector<File *> elements;

    };
}
    
#endif //DIRECTORY_FILE_HPP