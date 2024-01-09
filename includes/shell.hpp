#ifndef SHELL_HPP
#define SHELL_HPP

#include "../includes/files.hpp"
#include "../includes/directory_file.hpp"
#include "../includes/regular_file.hpp"
#include "../includes/linked_file.hpp"

using namespace std;
using namespace FileSystemKeskin;

namespace ShellKeskin
{
    class Shell
    {
        public:

            Shell();
            ~Shell();

            void startOS();

            // Input functions
            void getInput(string &input);
            void transformInput(string &input);
            void splitLine(const string &input, vector<string> &vect);
            void checkInput();
            // Element add & delete
            void addToOS();
            void addToFiles(File* obj);
            void addToFolders();
            void assignParents();
            // Helpers
            const string absolutePath(const File *obj, const Directory* current) const;
            // Commands & command helpers
                // LS & LS -R 
                void ls();
                void lsRecursive(const Directory *current);
                void lsPrint(const Directory *current);            
                // MKDIR & RMDIR
                void mkdir();
                void removeFromFiles(File* file, Directory* parentDir);
                void rmdir(File *file, Directory* parentDir);
                // RM FILES
                void rm();
                // CP FILES & DIRECTORIES
                void cp();
                void cp_directory(Directory* newDirectory, Directory* current);
                void link_recursive(SoftLinkedFile* linked,Directory* current);
                // LINK FILES
                void link();
                // CD DIRECTORIES
                void cd();
                void cd_checkInputs(const Directory *temp_parent);
                void cd_checkPaths();
                // CAT FILES & DIRECTORIES
                void cat();

                void check(Directory* current, Directory* totalDirectory); 
        private:
            Directory* files; // Holds all the files in the OSKeskin.txt
            Directory* currentDirectory; // Holds the current directory
            Directory* rootDirectory; // Holds the root directory
            size_t fileSize; // Size of the OSKeskin
            vector<string> inputs; // Holds the inputs of commands
            const static size_t MAX_SIZE; // Maximum size which is 10MB
    };

}

#endif