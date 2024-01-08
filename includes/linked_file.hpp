#ifndef LINKED_FILE_HPP
#define LINKED_FILE_HPP

#include "../includes/files.hpp"
#include "../includes/regular_file.hpp"
using namespace std;

namespace FileSystemKeskin
{
    class SoftLinkedFile : public File
    {
        public:
            SoftLinkedFile();

            virtual void printToSystem() override; // Overrides the virtual function
            virtual void readFromSystem(int &line) override; // Overrides the virtual function
            virtual void showContents() const override; // Overrides the virtual function

            //Setters & getters
            void setPointer(RegularFile *obj);
            void setLinkedPath(const string &path);
            RegularFile * getPointer() const;
            const string getLinkedPath() const;

            //Iterator Class
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
                return iterator(linkedRegular->getData().begin());
            }
            const iterator end() const
            {
                return iterator(linkedRegular->getData().end());
            }

        private:
            RegularFile *linkedRegular; // I hold a RegularFile pointer to point the file has been linked.
            string linkedFilePath;  // Holding the path of the linked file makes the link unique
    };
}

#endif //LINKED_FILE_HPP