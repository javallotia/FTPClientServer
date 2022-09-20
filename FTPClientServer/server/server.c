#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>    
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <libgen.h>
// #define PORT 12345

void listcmd(char buffer[]) {
    DIR *obj;
    struct dirent *dir;
    obj = opendir(".");
    strcat(buffer , "\nList of Directories and Files \t:\t\n");
    if(obj) {
        while( (dir = readdir(obj) ) != NULL ) {
            strcat(buffer , dir->d_name );
            strcat(buffer , "\n");
        }
    }
}

void pwdcmd(char buffer[]) {
    char pwd[PATH_MAX];
    if (getcwd(pwd, sizeof(pwd)) != NULL) {
        strcat(buffer , "\nCurrent Working Directory\t:\t");
        strcat(buffer , pwd);
        strcat(buffer , "\n");
    } 
    else {
        strcpy(buffer , "\nCouldn't Fetch Current Working Directory!!!\n");
        return;
    }
}

void cwdcmd(char buffer[] , char temp[]) {
    if( temp[3] != ' ') {
        strcat(buffer , "\n504 	Invalid Parameter!!!\n");
        return;
    }
    int len = strlen(temp);
    len -= 4;
    char path[len];
    strncpy(path , temp + 4 , len);
    path[len] = '\0';
    if (chdir(path) != 0)  {
        strcat(buffer , "\nNo Such File or Directory\n");
        return;
    }  
}
void cdupcmd(char buffer[]) {
    char pwd[PATH_MAX];
    getcwd(pwd, sizeof(pwd));
    if (pwd == NULL) {
        strcpy(buffer , "\nCouldn't Fetch Current Working Directory!!!\n");
        return;
    } 
    else {
        strcpy(pwd, dirname(pwd));
        if (chdir(pwd) != 0)  {
            strcat(buffer , "\nNo Parent Directory available!!!\n");
            return;
        }
        else {
            strcat(buffer , "\nCurrent Working Directory \t:\t");
            strcat(buffer , pwd);
            strcat(buffer , "\n");
        }
    }
}

void mkdcmd(char buffer[] , char temp[]) {
    if( temp[3] != ' ') {
        strcat(buffer , "\n504 	Invalid Parameter!!!\n");
        return;
    }
    int len = strlen(temp);
    len -= 4;
    char path[len];
    int i , j;
    for(i = 0; i < len; i++ ) {
        if(temp[ i + 4 ] == ' ') {
            strcat(buffer ,"\nInvalid Path\n");
            return;
        }
        if(temp[i+4] == '/' && i != len){
            j = i;
        }
        path[i] = temp[i+4];
    }
    path[j] = '\0';
    char dirName[len - j];
    strncpy(dirName , temp + 5 + j  , len - j);
    if (chdir(path) != 0)  {
        strcat(buffer , "\nNo Such File or Directory!!!\n");
        return;
    }
    // pwdcmd(buffer);
    if (mkdir(dirName , 0777) == -1)  {
        strcat(buffer , "\nError while Creating Directory!!!\n"); 
        return;
    }
    else {
        strcat(buffer , "\nSuccessfully Created Directory \t:\t");
        strcat(buffer , dirName); 
        strcat(buffer , "\n");
        //pwdcmd(buffer);
    }
}

void removeDirectory(const char path[] , char buffer[]) {
    size_t pathlen;
    char *fullpath;
    DIR *dir;
    struct stat statpath, statentry;
    struct dirent *entry;
    stat(path, &statpath);
    if (S_ISDIR(statpath.st_mode) == 0) {
        strcat(buffer , "\nNo Such Directory!!!\n");
        return;
    }
    if ((dir = opendir(path)) == NULL) {
        strcat(buffer , "\nCouldn't Open The Directory\n");
        return;
    }
    pathlen = strlen(path);
    // for each entry in Directory
    while ((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
            continue;
        }
        fullpath = calloc(pathlen + strlen(entry->d_name) + 1, sizeof(char));
        strcpy(fullpath, path);
        strcat(fullpath, "/");
        strcat(fullpath, entry->d_name);
        stat(fullpath, &statentry);
        if (S_ISDIR(statentry.st_mode) != 0) {
            removeDirectory(fullpath , buffer);
            continue;
        }
        if (unlink(fullpath) == 0) {
            strcat(buffer , "\nFile Removed from the Directory\t\t:\t");
            strcat(buffer , fullpath);
        }
        else {
            strcat(buffer , "\nCouldn't remove File from the Directory\t:\t");
            strcat(buffer , fullpath);
            //return;
        }
    }
    if (rmdir(path) == 0) {
        strcat(buffer , "\nDirectory Removed\t:\t");
        strcat(buffer , path);
    }
    else {
        strcat(buffer , "\nCan't remove Directory\t:\t");
        strcat(buffer , fullpath);
        //return;
    }
    closedir(dir);
}
void rmdcmd(char buffer[] , char temp[]) {
    if( temp[3] != ' ') {
        strcat(buffer , "\n504 	Invalid Parameter!!!\n");
        return;
    }
    int len = strlen(temp);
    len -= 4;
    char path[len];
    strncpy(path , temp + 4 , len);
    path[len] = '\0';
    
    removeDirectory(path , buffer);
    
}
void removeFile(const char path[] , char buffer[]) {
    char *fullpath;
    struct stat statpath;
    stat(path, &statpath);
    if (S_ISREG(statpath.st_mode) == 0) {
        strcat(buffer , "\nNo Such File in the Directory!!!\n");
        return;
    }
    strcpy(fullpath, path);
    if (remove(fullpath) == 0) {
        strcat(buffer , "\nFile Removed\t:\t");
        strcat(buffer , fullpath);
    }
    else {
        strcat(buffer , "\nCouldn't Remove File\t:\t");
        strcat(buffer , fullpath);
        //return;
    }
}
void delecmd(char buffer[] , char temp[] ) {
    int len = strlen(temp);
    len -= 5;
    char path[len];
    strncpy(path , temp + 5 , len);
    path[len] = '\0';
    
    removeFile(path , buffer);
}
char OldPath[1024], NewPath[1024];
int prevcmd;
void rnfrcmd(char buffer[] , char temp[]) {
    struct stat sb;
    int len = strlen(temp) - 5;
    strncpy(OldPath , temp + 5, len);
    stat(OldPath, &sb);
    if (sb.st_mode & S_IFREG) {
        strcat(buffer , "\nProvided File Path \t:\t");
        strcat(buffer , OldPath);
        strcat(buffer , "\n");
        return;
    }
    else {
        strcat(buffer , "\nFile does not exist \t:\t");
        strcat(buffer , OldPath);
        strcat(buffer , "\n");
    }
    return;
}
void rntocmd(char buffer[], char temp[]) {
    struct stat sb1, sb2;
    int len = strlen(temp) - 5;
    strncpy(NewPath, temp + 5, len);
    stat(NewPath, &sb1);
    if (sb1.st_mode & S_IFREG) {
        strcat(buffer , "\n");
        strcat(buffer , NewPath);
        strcat(buffer , "\n\t:\t File already Exist in the location!!!");
        return;
    }
    else {
        stat(OldPath, &sb2);
        if (sb2.st_mode & S_IFREG) {
            if (rename(OldPath , NewPath) < 0) {
                strcat(buffer , "\nRename Error \t:\t");
                strcat(buffer , OldPath);
                strcat(buffer , "\n");
                return;
            }
            else{
                strcat(buffer , "\nRename Successful\n");
                strcat(buffer , "\nOld File path \t:\t");
                strcat(buffer , OldPath);
                strcat(buffer , "\n");
                strcat(buffer , "\nNew File path \t:\t");
                strcat(buffer , NewPath);
                strcat(buffer , "\n");
                return;
            }
        }
    } 
}
char fileName[50];
void storcmd(char buffer[] , char temp[]) {
    int len = strlen(temp) - 5;    
    strncpy(fileName , temp + 5 , len);
    fileName[len] = '\0';
    FILE *fPtr;
    fPtr = fopen(fileName , "w");
    if (fPtr == NULL) {
        strcat(buffer , fileName);
        strcat(buffer , "\t:\tFile Does Not Exist!!!\n");
        return;
    }
    strcpy(buffer , "\n225 	Store Data connection open!!!\n");
    fclose(fPtr);
}

void appecmd(char buffer[] , char temp[]) {
    int len = strlen(temp) - 5;
    
    strncpy(fileName , temp + 5 , len);
    fileName[len] = '\0';
    FILE *fPtr;
    fPtr = fopen(fileName , "a");
    if (fPtr == NULL) {
        strcat(buffer , fileName);
        strcat(buffer , "\t:\t File Does Not Exist!!!\n");
        return;
    }
    strcpy(buffer , "\n225 	Append Data connection open!!!\n");
    fclose(fPtr);
}

void retrcmd(char buffer[] , char temp[] ) {
    int len = strlen(temp) - 5;
    strncpy(fileName , temp + 5 , len);
    fileName[len] = '\0';
    FILE *fPtr;
    fPtr = fopen(fileName , "r");
    if (fPtr == NULL) {
        strcat(buffer , fileName);
        strcat(buffer , "\t:\t File Does Not Exist!!!\n");
        return;
    }
    strcpy(buffer , "\n225 	Fetch Data connection open!!!\n");
    fclose(fPtr);
}

int find(char* str1) {
    if(str1[0] == 'U') {
        if(str1[1] == 'S' && str1[2] == 'E' && str1[3] == 'R') {
            return 1;
        }
        return 0;
    }
    else if(str1[0] == 'P') {
        if(str1[1] == 'A' && str1[2] == 'S' && str1[3] == 'S') {
            return 2;
        }
        else if(str1[1] == 'W' && str1[2] == 'D') {
            return 6;
        }
        else {
            return 0;
        }
    }
    else if(str1[0] == 'M') {
        if(str1[1] == 'K' && str1[2] == 'D') {
            return 3;
        }
        return 0;
    }
    else if(str1[0] == 'C') {
        if(str1[1] == 'W' && str1[2] == 'D') {
            return 4;
        }
        else if(str1[1] == 'D' && str1[2] == 'U' && str1[3] == 'P') {
            return 12;
        }
        return 0;
    }
    else if(str1[0] == 'R') {
        if(str1[1] == 'E' && str1[2] == 'T' && str1[3] == 'R') {
            return 7;
        }
        else if(str1[1] == 'M' && str1[2] == 'D') {
            return 5;
        }
        else if(str1[1] == 'E' && str1[2] == 'I' && str1[3] == 'N') {
            return 19;
        }
        else if(str1[1] == 'N' && str1[2] == 'F' && str1[3] == 'R') {
            return 17;
        }
        else if(str1[1] == 'N' && str1[2] == 'T' && str1[3] == 'O') {
            return 18;
        }
        return 0;
    }
    else if(str1[0] == 'S') {
        if(str1[1] == 'T' && str1[2] == 'O' && str1[3] == 'R') {
            return 8;
        }
        return 0;
    }
    else if(str1[0] == 'L') {
        if(str1[1] == 'I' && str1[2] == 'S' && str1[3] == 'T') {
            return 9;
        }
        return 0;
    }
    else if(str1[0] == 'A') {
        if(str1[1] == 'B' && str1[2] == 'O' && str1[3] == 'R') {
            return 10;
        }
        if(str1[1] == 'P' && str1[2] == 'P' && str1[3] == 'E') {
            return 13;
        }
        return 0;
    }
    else if(str1[0] == 'D') {
        if(str1[1] == 'E' && str1[2] == 'L' && str1[3] == 'E') {
            return 14;
        }
    }
    else if(str1[0] == 'N') {
        if(str1[1] == 'O' && str1[2] == 'O' && str1[3] == 'P') {
            return 16;
        }
    }
    else if(str1[0] == 'Q') {
        if(str1[1] == 'U' && str1[2] == 'I' && str1[3] == 'T') {
            return 11; 
        }
        return 0;
    }
    return 0;
}

int flag = 0;
int indexDB = 0;
void callFunction(int returnValue, char buffer[]) {
    char temp[500] = "";
    strcpy(temp , buffer);
    strcpy(buffer , "\n200	Command okay\n");
    if(returnValue == 1) {
        prevcmd = returnValue;
        if( flag == 1 || flag == 2) {
            strcpy(buffer , "\n503 	Invalid Commands!!!\n");
            return;
        }
        if( temp[4] != ' ') {
            strcpy(buffer , "\n503 	Invalid Commands!!!\n");
            return;
        }
        char userID[10];
        strncpy(userID , temp + 5 , 9);
        static const char filename[] = "idPass.txt";
        FILE *file = fopen ( filename, "r" );
        char line [ 100 ];
        char id[10];
        while ( fgets ( line, sizeof line, file ) != NULL ) {
            strncpy(id , line , 9);
            id[9] = '\0';
            indexDB++;
            if ( strcmp( id , userID ) == 0 ) {
                flag = 1;
                break;
            }
        }
        fclose ( file );
        if ( flag == 1) {
            strcat(buffer , "\n331 	User Name okay, need Password!!!\n");
        }
        else {
            strcpy(buffer , "\nUser Name not Found In DATABASE!!!\n");
            indexDB = 0;
            return;
        }
    }
    else if ( returnValue == 2) {
        prevcmd = returnValue;
        if( flag == 2 || flag == 0) {
            strcpy(buffer , "\n503 	Invalid Commands!!!\n");
            return;
        }
        if( temp[4] != ' ') {
            strcpy(buffer , "\n503 	Invalid Commands!!!\n");
            return;
        }
        char userPass[9];
        strncpy(userPass , temp + 5 , 8);
        static const char filename[] = "idPass.txt";
        FILE *file = fopen ( filename, "r" );
        char line [ 100 ];
        char pass[9];
        int count = 1;
        while ( fgets ( line, sizeof line, file ) != NULL ) {
            if( count == indexDB ) {
                strncpy(pass , line + 10 , 8);
                pass[8] = '\0';
                if ( strcmp( pass , userPass ) == 0 ) {
                    flag = 2;
                }
                break;
            }
            count += 1;
        }
        fclose ( file );
        if ( flag == 2) {
            strcat(buffer , "\nSuccessfully Connected!!!\n");
            return;
        }
        else {
            strcpy(buffer , "\nPort does not exist!!!\n");
            return;
        }
    }
    else if ( returnValue == 3) {
        prevcmd = returnValue;
        if(flag == 0) {
            strcat(buffer , "\n530 	User Not logged in!!!\n");
            return;
        }
        if(flag == 1) {
            strcat(buffer , "\n331 	User Name okay, need Password!!!\n");
            return;
        }
        mkdcmd(buffer , temp);
    }
    else if ( returnValue == 4) {
        if(flag == 0) {
            strcat(buffer , "\n530 	User Not logged in!!!\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n331 	User Name okay, need Password!!!\n");
            return;
        }
        pwdcmd(buffer);
        cwdcmd(buffer, temp);
        pwdcmd(buffer);
    }
    else if ( returnValue == 5) {
        prevcmd = returnValue;
        if(flag == 0) {
            strcat(buffer , "\n530 	User Not logged in!!!\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n331 	User Name okay, need Password!!!\n");
            return;
        }
        rmdcmd(buffer , temp);
    }
    else if ( returnValue == 6) {
        prevcmd = returnValue;
        if(flag == 0) {
            strcat(buffer , "\n530 	User Not logged in!!!\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n331 	User Name okay, need Password!!!\n");
            return;
        }
        if( temp[3] == ' ') {
            strcat(buffer , "\n504 	Invalid Parameter!!!\n");
            return;
        }
        if( strlen(temp) > 3) {
            strcpy(buffer , "\n503 	Invalid Commands!!!\n");
            return;
        }
        pwdcmd(buffer);
    }
    else if ( returnValue == 7) {
        prevcmd = returnValue;
        if(flag == 0) {
            strcat(buffer , "\n530 	User Not logged in!!!\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n331 	User Name okay, need Password!!!\n");
            return;
        }
         if( temp[4] != ' ') {
            strcpy(buffer , "\n503 	Invalid Commands!!!\n");
            return;
        }
        retrcmd(buffer , temp);
    }
    else if ( returnValue == 8) {
        prevcmd = returnValue;
        if(flag == 0) {
            strcat(buffer , "\n530 	User Not logged in!!!\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n331 	User Name okay, need Password!!!\n");
            return;
        }
        if( temp[4] != ' ') {
            strcpy(buffer , "\n503 	Invalid Commands!!!\n");
            return;
        }
        storcmd(buffer , temp);
    }
    else if ( returnValue == 9) {
        prevcmd = returnValue;
        if(flag == 0) {
            strcat(buffer , "\n530 	User Not logged in!!!\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n331 	User Name okay, need Password!!!\n");
            return;
        }
        if( temp[4] == ' ') {
            strcat(buffer , "\n504 	Invalid Parameter!!!\n");
            return;
        }
        if( strlen(temp) > 4) {
            strcpy(buffer , "\n503 	Invalid Commands!!!\n");
            return;
        }
        listcmd(buffer);
    }
    else if ( returnValue == 10) {
        prevcmd = returnValue;
        if(flag == 0) {
            strcat(buffer , "\n530 	User Not logged in!!!\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n331 	User Name okay, need Password!!!\n");
            return;
        }
        if( temp[4] == ' ') {
            strcat(buffer , "\n504 	Invalid Parameter!!!\n");
            return;
        }
        if( strlen(temp) > 4) {
            strcat(buffer , "\n503 	Invalid Commands!!!\n");
            return;
        }
        flag = 0;
        strcat(buffer , "\n551 	Requested action aborted: page type unknown. - User Logged Out\n");
        indexDB = 0;
    }
    else if ( returnValue == 11) {
        prevcmd = returnValue;
        strcpy(buffer ,  "\nConnection Terminated!!!\n");
        return;
    }
    else if ( returnValue == 12) {
        prevcmd = returnValue;
        if(flag == 0) {
            strcat(buffer , "\n530 	User Not logged in!!!\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n331 	User Name okay, need Password!!!\n");
            return;
        }
        if( temp[4] == ' ') {
            strcat(buffer , "\n504 	Invalid Parameter!!!\n");
            return;
        }
        if( strlen(temp) > 4) {
            strcpy(buffer , "\n503 	Invalid Commands!!!\n");
            return;
        }
        cdupcmd(buffer);
        pwdcmd(buffer);
    }
    else if ( returnValue == 13) {
        prevcmd = returnValue;
        if(flag == 0) {
            strcat(buffer , "\n530 	User Not logged in!!!\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n331 	User Name okay, need Password!!!\n");
            return;
        }
        if( temp[4] != ' ') {
            strcpy(buffer , "\n503 	Invalid Commands!!!\n");
            return;
        }
        appecmd(buffer , temp);
    }
    else if ( returnValue == 14) {
        prevcmd = returnValue;
        if(flag == 0) {
            strcat(buffer , "\n530 	User Not logged in!!!\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n331 	User Name okay, need Password!!!\n");
            return;
        }
        delecmd(buffer , temp);
    }
    else if ( returnValue == 15) {
        prevcmd = returnValue;
        if(flag == 2) {
            flag = 0;
            strcpy(buffer ,  "\nUser Logged Out Successfully!!!\n");
        }
        else {
            strcpy(buffer ,  "\nUser not logged in!!!\n");
        }
        return;
    }
    else if ( returnValue == 16) {
        prevcmd = returnValue;
        strcpy(buffer ,  "\nOkay\n");
        return;
    }
    else if ( returnValue == 17) {
        prevcmd = returnValue;
        if(flag == 0) {
            strcat(buffer , "\n530 	User Not logged in!!!\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n331 	User Name okay, need Password!!!\n");
            return;
        }
        rnfrcmd(buffer , temp);
    }
    else if ( returnValue == 18) {
        if(prevcmd != 17) {
            strcat(buffer, "\n\t:\t RNTO can only be used after using the RNFR command!!!");
            return;
        }
        prevcmd = returnValue;
        if(flag == 0) {
            strcat(buffer , "\n530 	User Not logged in!!!\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n331 	User Name okay, need Password!!!\n");
            return;
        }
        rntocmd(buffer , temp);
    }
    else if( returnValue == 19 ) {
        prevcmd = returnValue;
        if(flag == 2) {
            flag = 0;
            indexDB = 0;
            strcpy(buffer ,  "\nUser Logged Out Successfully!!!\n");
        }
        else {
            strcpy(buffer ,  "\nUser not logged in!!!\n");
        }
        return;
    }

}
// char sigbuffer[1000] = "";
// void myhandler(int) {
//     strcpy(sigbuffer, "\nConnection Terminated!!!\n");
//     // printf("%s", sigbuffer);
// }

void mainloop(int acceptOutput) {
    char buffer[10000] = "";
    char temp[10000] = " ";
    while(1) {
        if( strcmp(temp , "\n225 	Store Data connection open!!!\n") == 0) {
            FILE *fPtr;
            fPtr = fopen(fileName, "w"); 
            if (fPtr == NULL) { 
                printf("Cannot open file %s \n", fileName);  
            }
            else {
                bzero(buffer, sizeof(buffer));
                while (1) {
                    read(acceptOutput , buffer , sizeof(buffer));
                    fputs(buffer , fPtr);
                    if ( strlen(buffer) - 1 != sizeof(buffer) ) {
                        break;
                    }
                    bzero(buffer, sizeof(buffer));
                }
                printf("\nFile Received Successfully!!!\n");
                fclose(fPtr);
                bzero(temp , sizeof(temp));
            }
        }
        if( strcmp(temp , "\n225 	Append Data connection open!!!\n") == 0) {
            FILE *fPtr;
            fPtr = fopen(fileName, "a"); 
            if (fPtr == NULL) { 
                printf("Cannot open file %s \n", fileName);  
            }
            else {
                bzero(buffer, sizeof(buffer));
                while (1) {
                    read(acceptOutput , buffer , sizeof(buffer));
                    fputs(buffer , fPtr);
                    if ( strlen(buffer) - 1 != sizeof(buffer) ) {
                        break;
                    }
                    bzero(buffer, sizeof(buffer));
                }
                printf("\nFile Received Successfully!!!\n");
                fclose(fPtr);
                bzero(temp , sizeof(temp));
            }
        }
        bzero(buffer, sizeof(buffer));
        read(acceptOutput , buffer , sizeof(buffer));
        // if(strcmp(sigbuffer, "\nConnection Terminated!!!\n") == 0) {
        //     strcpy(buffer, "QUIT");
        // }
        printf("\nMessage Received\t:\t%s\n" , buffer);
        int returnValue = find( buffer );
        if( returnValue == 0 ) {
            strcpy(buffer , "502 	Invalid Command!!!\n");
        }
        else {
            callFunction(returnValue , buffer);
        }
        write(acceptOutput , buffer , strlen(buffer));
        if( strcmp( "\nConnection Terminated!!!\n" , buffer) == 0) {
            //sleep(3);
            printf("%s" , buffer);
            exit(0);
        }
        printf("%s" , buffer);
        strcpy(temp , buffer);


        if ( strcmp(temp , "\n225 	Fetch Data connection open!!!\n") == 0 ) {
            printf("\nFile Sending In Process\n");
            sleep(2);
            FILE *fPtr;
            fPtr = fopen(fileName, "r"); 
            if (fPtr == NULL) { 
                printf("Cannot open file\t:\t%s \n", fileName); 
            }
            else {
                char ch;
                bzero(buffer, sizeof(buffer));
                int i = 0;
                while(1) {
                    while ( ( (ch = fgetc(fPtr)) != EOF ) && ( strlen(buffer) - 1 != sizeof(buffer)) ) {
                        buffer[i] = ch;
                        i++;
                    }
                    buffer[i] = '\0';
                    if ( ch == EOF) {
                        write(acceptOutput , buffer, strlen(buffer));
                        break;
                    }
                    if ( strlen(buffer) == sizeof(buffer) ) {
                        write(acceptOutput , buffer, strlen(buffer));
                        bzero(buffer, sizeof(buffer));
                        i = 0;
                    }
                }
                printf("\nFile Sent Successfully!!!\n");
                fclose(fPtr);
            }
        }
    }
}

int main(int argc, char *argv[]) {
   int sockFD , bindOutput , acceptOutput , clientLen, portNumber;
    struct sockaddr_in servaddr,cliaddr;
    sockFD = socket(AF_INET,SOCK_STREAM,0);
    if( sockFD < 0 ) {
        printf("\nSocket Error!!!\n");
        exit(0);
    }
    else {
        printf("\nSocket is created!!!\n");
    }
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    sscanf(argv[1], "%d", &portNumber);
    servaddr.sin_port=htons((uint16_t)portNumber);
    bindOutput = bind(sockFD,(struct sockaddr *)&servaddr, sizeof(servaddr) );
    if( bindOutput < 0 ) {
        printf("Cant bind!!!\n");
        exit(0);
    }
    else {
        printf("Bind Successful!!!\n");
    }
    listen(sockFD , 5 );
    while(1) {
        clientLen = sizeof(cliaddr);
        acceptOutput = accept(sockFD,(struct sockaddr *)&cliaddr , &clientLen);
        if(acceptOutput < 0) {
            printf("Couldn't accept Client!!!\n");
            exit(0);
        }
        else {
            printf("Client Connection Request Accepted!!!\n");
        }
        pid_t pid;
        if( (pid = fork()) == 0) {
            // struct sigaction act;
            // act.sa_handler = myhandler;
            // sigaction(SIGINT, &act, NULL);
            mainloop(acceptOutput);
            close(acceptOutput);
            close(sockFD);
            exit(0);
        }
        close(acceptOutput);
    }
    return 0;
}
