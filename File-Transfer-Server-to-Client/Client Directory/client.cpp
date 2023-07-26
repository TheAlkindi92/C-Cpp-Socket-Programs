#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sstream>
#include <ctime>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
using namespace std;


//Client side
int main(int argc, char *argv[])
{
    //we need port number, from command line to connect to server 

    if(argc != 2)
    {
        cerr << "Usage: ip_address port" << endl; exit(0); 
    }
    
    
     //grab the IP address and port number 
    char *serverIp = (char*)"127.0.0.1"; 
    int port = atoi(argv[1]); 


    //create a message buffer 
    char msg[1500]; 



    //setup a socket and connection tools 
    struct hostent* host = gethostbyname(serverIp); 
    sockaddr_in sendSockAddr;   

    bzero((char*)&sendSockAddr, sizeof(sendSockAddr)); 
    sendSockAddr.sin_family = AF_INET; 
    sendSockAddr.sin_addr.s_addr = 
        inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    //try to connect...
    int status = connect(clientSd,
                         (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));


    if(status < 0)
    {
        cout<<"Error connecting to socket!"<<endl;
        return -1;
    }
    cout << "Connected to the server!\n" << endl;


    

    int bytesRead, bytesWritten = 0;
    int UCID, PASSCODE;
    string data;
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);
    int op = 1, pchk = 1;


//************** Socket Communication Started here  ***************

    while(1)
    {

        // **************  Block to receive UCID from User and send it to server and receive it from the Server ***************


        if(op == 1){
            cout<<"Enter Your UCID "<<endl;
             cout << ">";
        getline(cin, data);


        cout << "Your UCID is :: " + data<<endl;
            UCID = stoi(data);

            memset(&msg, 0, sizeof(msg));//clear the buffer
        strcpy(msg, data.c_str());
       

        if(data == "exit")
        {
            send(clientSd, (char*)&msg, strlen(msg), 0);
            break;
        }
        bytesWritten += send(clientSd, (char*)&msg, strlen(msg), 0);
        cout << "Awaiting server response..." << endl;
        memset(&msg, 0, sizeof(msg));//clear the buffer
        bytesRead += recv(clientSd, (char*)&msg, sizeof(msg), 0);
        cout << "Local Time Send by Server: " << msg << endl;

          if(pchk == 1){

            char s1[2];
            s1[0] = msg[17];
            s1[1] = msg[18];
            int sec = stoi(s1);
            
            PASSCODE = UCID+sec;
            cout<<"\n\nGenerating Passcode for Server......\n";
            //sleep(5);
            cout << "The PASSCODE for Server is : " << PASSCODE<<endl;
            pchk++;
        }
            op++;

        }
        // **********  Block to Send Passcode to Server and Receive content of file from server *****************


        else if(op == 2){

            std::string tmp = std::to_string(PASSCODE);
            char const *num_char = tmp.c_str();

            memset(&msg, 0, sizeof(msg));//clear the buffer
            strcpy(msg, num_char);
            op++;
            cout << "\n\nSending Passcode to server....."<<endl;

            bytesWritten += send(clientSd, (char*)&msg, strlen(msg), 0);
            cout << "Awaiting server response..." << endl;
            memset(&msg, 0, sizeof(msg));//clear the buffer
            bytesRead += recv(clientSd, (char*)&msg, sizeof(msg), 0);


            FILE *outfile = std::fopen("Clientdata.txt", "w");
            std::string fileContent = msg;
            std::fprintf(outfile, "%s\n", fileContent.c_str());
            std::fclose(outfile);

            cout << "\nThe Content of File send by Server :: " << msg << endl;
            op++;
            break;

        }else{
             memset(&msg, 0, sizeof(msg));//clear the buffer
            bytesRead += recv(clientSd, (char*)&msg, sizeof(msg), 0);

        }
        
    }



    //we need to close the socket descriptors after we're all done


    gettimeofday(&end1, NULL);
    close(clientSd);
    cout << "\n\n********Session********\n" << endl;
    cout << "Bytes written: " << bytesWritten << 
    " Bytes read: " << bytesRead << endl;
    cout << "Elapsed time: " << (end1.tv_sec- start1.tv_sec) 
      << " secs" << endl;
    cout << "Connection closed\n\n" << endl;
    return 0;    
}