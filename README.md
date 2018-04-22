# ELF-Virus by Shail Shah
This repository contains code for a simple ELF Virus that is capable of executing something malicious, infecting other ELF files, and executing the infected files normally in the foreground.

### Getting Started 
1. Clone the repository: 
`git clone https://github.com/shailrshah/ELF-Virus.git`

2. Go inside the repository: 
`cd ELF-Virus`

3. Change permission for the test: 
`chmod 755 test/virusCheck.sh`

4. Run the test: 
`test/virusCheck.sh`

5. View and edit the source code: 
`vim src/virus.c`

6. Compile the source code:
`gcc -o virus src/virus.c`

7. Run the virus:
`./virus`

### What is an ELF Virus? 
An ELF Virus is a malicious piece of code that targets ELF (Executable and Linkable Format) executeables. It infects these executables in such a way that after being affected, these executables carry out some things invisible to the user. If the ELF Virus is able to get root access, it can cause significant damage to the system.

### How it works
1. Something bad is executed. 
2. An ELF executeable that has not yet been infected is searched for in the current working directory. If found, the executable is infected, by adding the virus code before the execuatble, and a magic number or a signature at the end of the executable.
3. If the object file that is being executed is the original virus file itself, a magic number or a signature is added at the end of the file. Otherwise, the original code of the executable is executed.

