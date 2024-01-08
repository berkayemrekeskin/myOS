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
            
            virtual void printToSystem() override;
            virtual void readFromSystem(int &line) override;
            virtual void showContents() const override;

            void fileToVector(string filePath);

            size_t getSize() const;
            vector<string> getData() const;
            void setData(const vector<string> &data);
            
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
                return iterator(this->getData().begin());
            }
            const iterator end() const
            {
                return iterator(this->getData().end());
            }
            
        private:
            size_t size;
            vector<string> data;
    };
}

#endif