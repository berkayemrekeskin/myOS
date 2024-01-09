Name & Surname: Berkay Emre KESKIN
Class : CSE241
Homework 2

In this homework, i generally try to write according to the pdf file. Below this text, i will introduce my code and algorithm.

    Commands & their use:   

        -ls & ls -R = Lists the contents of the current directory, ls -R lists recursively.
        -mkdir = My mkdir makes and removes directories. If there is a directory created and the input name is same with that directory,
    i remove that directory. If the input is unique, than it creates a directory. (as the pdf told)
        -rm = removes a Regular or Linked file.
        -cp = My cp copies files inside & outside of the OS, also it can copy directories and their elements inside the OS.
        -link = makes a link with a Regular file
        -cd = Changes the current directory with the given path
        -cat = Prints the data inside the files.
        -quit = Quits from the shell

    My Notes:
        
        PDF tolds that cat should be in the File abstract class, i made it in the abstract class but it's name
    is showContents. I have some pure virtual functions in my abstract File class to do file read & write.
        My Shell class handles all the commands accordingly to the PDF file. I wanted to make the names same with
    the commands so cat function's name is showContents.
        If OSKeskin.txt does not open, please delete that txt becuse it may become corrupted. My program can open it's own 
    OSKeskin.txt file if there is none.
    