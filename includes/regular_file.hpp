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
            RegularFile(string name, string path, string type);

            virtual void printToSystem() override;
            virtual void readFromSystem(int &line) override;
            virtual void showContents() const override;

            void fileToVector(string filePath);
            size_t getSize() const;
            void setData(const vector<string> &data);
            vector<string> getData() const;
            
            //Iterator Class
            /*
            class iterator
            {
                public:
                    iterator(string* ptr) : current(ptr) {}
                    string& operator*() const { return *current; }
                    iterator& operator++() 
                    {
                        ++current;
                        return *this;
                    }
                    iterator operator++(int)
                    {
                        string* temp = current;
                        ++current;
                        return iterator(temp);
                    }
                    bool operator==(const iterator& oth) const { return current == oth.current; }
                    bool operator!=(const iterator& oth) const { return current != oth.current; }
                private:
                    string* current;
            };
            iterator begin() const
            {
                return iterator(data.begin());
            }
            iterator end() const
            {
                return iterator(data.end());
            }
            */
        private:
            size_t size;
            vector<string> data;
    };
}

#endif