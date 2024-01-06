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
            class iterator
            {
                public:
                    iterator(File* ptr) : current(ptr) {}
                    File& operator*() const { return *current; }
                    iterator& operator++() 
                    {
                        ++current;
                        return *this;
                    }
                    iterator operator++(int)
                    {
                        File* temp = current;
                        ++current;
                        return iterator(temp);
                    }
                    bool operator==(const iterator& oth) const { return current == oth.current; }
                    bool operator!=(const iterator& oth) const { return current != oth.current; }

                private:
                    File* current;
            };
            const iterator begin() const
            {
                return iterator(elements[0]);
            }
            const iterator end() const
            {
                return iterator(elements[elements.size()]);
            }

        private:
            Directory * parentDirectory;
            vector<File *> elements;

    };
}
    
#endif //DIRECTORY_FILE_HPP