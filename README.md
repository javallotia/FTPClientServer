# FTPClientServer
Application to perform various FTP Client Server protocols 
## Implimented Protocols
### USER - Identifies the User name with the entered login credentials.
### PASS - Identifies the User password with the entered Password.
### LIST - lists all the directories and files in the spicified path.
### PWD - Retrives the current working directory.
### CDUP - Moves to the parent directory.
### CWD - Changes the current working directory to the specified path.
### MKD - created a directory in the specified path.
### RMD - Remove the specified directory and its contents in the path.
### DELE - Removes the specified file in the path.
### RNFR - Sets the specified directory from the path to be moved or renamed.
### RNTO - Renames or Moves the set directory to the specified path. *Only works when used directly after RNFR command!
### STOR - Sends and stores a file to the server.
### RETR - Retrives and stores a file from the server to the client.
### APPE - Sends and appends a file to the server if it exists, otherwise stores the new file.
### ABOR - Aborts any on going transactions between the server and client and logs out the user.
### REIN - Reinitilizes the connection and logs out the user.
### NOOP - The server replies with an Okay message.
### QUIT - Safely terminates the connection between the server and client.

This project was moved from the university server to GitHub to make it publically visible.
