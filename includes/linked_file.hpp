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

            virtual void printToSystem() override;
            virtual void readFromSystem(int &line) override;
            virtual void showContents() const override;

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
            RegularFile *linkedRegular;
            string linkedFilePath;
    };
}

#endif //LINKED_FILE_HPP