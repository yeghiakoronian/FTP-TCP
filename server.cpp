//    SERVER TCP PROGRAM
// revised and tidied up by
// J.W. Atwood
// 1999 June 30
// There is still some leftover trash in this code.

/* send and receive codes between client and server */
/* This is your basic WINSOCK shell */
#pragma comment( linker, "/defaultlib:ws2_32.lib" )
  
#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <winsock.h>
#include <iostream>
#include <windows.h>
#include <string>
#include <strsafe.h>
#include <vector>
#include <sstream>
#include <fstream>
#pragma comment(lib,"ws2_32.lib")
//#include <unistd.h>
using namespace std;
//#include <dirent.h>

//port data types

#define REQUEST_PORT 0x7070
#define GET "GET"
#define PUT "PUT"
#define LIST "list"
#define EXIT "exit"

const int BUFFER_SIZE=128;
int port=REQUEST_PORT;

//socket data types
SOCKET s;

SOCKET s1;
SOCKADDR_IN sa;      // filled by bind
SOCKADDR_IN sa1;     // fill with server info, IP, port
union {struct sockaddr generic;struct sockaddr_in ca_in;}ca;

int calen=sizeof(ca); 

//buffer data types
static	char szbuffer[BUFFER_SIZE];

char *buffer;
int ibufferlen;
int ibytesrecv;

int ibytessent;

//host data types
char localhost[11];

HOSTENT *hp;

//wait variables
int nsa1;
int r,infds=1, outfds=0;
struct timeval timeout;
const struct timeval *tp=&timeout;

fd_set readfds;

//others
HANDLE test;

DWORD dwtest;
WIN32_FIND_DATA file_data;

//DIR *directory;
struct dirent *ent;
//WIN32_FIND_DATA file_data;
char *myDir;
struct stat myStat;

//lists the files in a dirctory
/////////////////////////////////////////////////////////
vector<string> filesInDirectory()
{
	vector<string> filenames = vector<string>();
	WIN32_FIND_DATA fd;
	HANDLE ff = FindFirstFile(".\\*", &fd);
	

	if (ff != INVALID_HANDLE_VALUE)
	{ 
		
		do 
		{
			if (fd.cFileName[0] == '.') continue;
			printf("%s\n",fd.cFileName);
			filenames.push_back(string(fd.cFileName));
		} 
		while (FindNextFile(ff, &fd)); 
		FindClose(ff); 
	}

	return filenames;
}
////////////////////////////////////////////////////////////////////////////
////to create one string to be sent based on the vector that we have 
string vectorToString(vector<string> vect)
{
	string toSend="";
	for (int i=0;i<vect.size();i++)
		toSend=toSend+vect.at(i)+"\n";
	return toSend;
}


//to empty the contents of a buffer
void flushBuffer()
{
	for (int i=0;i<BUFFER_SIZE;i++)
		szbuffer[i]='\0';
}
//Number of packets expected to be recieved
int recievePacketsNb(SOCKET socket)
{
	if((ibytesrecv = recv(socket,szbuffer,128,0)) == SOCKET_ERROR)
		throw "Receive failed\n";

	string str=szbuffer;
	flushBuffer();
	return atoi(str.c_str()); 
}
//loop to recieve the content of the packets
string recievePacketsContents(int numberOfPacketsRecieved,SOCKET socket)
{
	string  dataContent="";

	for(int i=0;i<numberOfPacketsRecieved;i++)
	{
		flushBuffer();
		if((ibytesrecv = recv(socket,szbuffer,BUFFER_SIZE,0)) == SOCKET_ERROR)
			throw "Receive failed\n";
		else
			dataContent=dataContent+szbuffer;	
	}
	return dataContent;
}
//send packet acknowledgement
void sendPacketAck(SOCKET socket,char buff[])
{
	int bufferlen = strlen(buff);
	ibytessent=0;
	if((ibytessent = send(socket,"0",2,0))==SOCKET_ERROR)
		throw "error in send in server program\n";	

}
//does the whole work
string recieveDataPacket(SOCKET socket)
{
	int pack_NB=recievePacketsNb(socket);
	char ack[]={'1'};
	sendPacketAck(socket,ack);

	string content=recievePacketsContents(pack_NB,socket);
	flushBuffer();
	sendPacketAck(socket,ack);
	return content;
}
//To send data saved in the buffer	
void sendingData(SOCKET socket,char buff[])
{
	int bufferlen = strlen(szbuffer);
	if((ibytessent = send(socket,szbuffer,bufferlen,0))==SOCKET_ERROR)
		throw "error in send in server program\n";
	//else
		//cout<<"sent"<<endl;
}

void recievingData(SOCKET socket,char buff[],char comment[])
{
	if((ibytesrecv = recv(s1,szbuffer,BUFFER_SIZE,0)) == SOCKET_ERROR)
		cout<< "Receive error in server program\n"<<endl;
	else
		cout << comment << ":  "<<szbuffer <<"   ";
}

//get the number of packets
int numberOfPacket(int contentSize)
{
	int numberOfPackets=0;

	if(contentSize % BUFFER_SIZE==0)
		numberOfPackets=int(contentSize/BUFFER_SIZE);
	else 
		numberOfPackets=int(contentSize/BUFFER_SIZE)+1;
	return numberOfPackets;

}
//sending the number of packets expected
void sendingNbOfPacketCtl(int numberOfPackets,SOCKET socket)
{
	int y = numberOfPackets;
	ostringstream sx;
	sx<<numberOfPackets ;
	string number=sx.str();
	//cout<<"number of packs y is :"<<y<<endl;
	//cout<<"string number =is :"<<number<<endl;
	//cout<<"number.size() :"<<number.size()<<endl;
	for (int i=0;i<number.size();i++)
		szbuffer[i]=number.at(i);

	// the first message to be sent is the number of the packets to be sent after
	sendingData(socket,szbuffer);
	flushBuffer();
	//	recievingData(s1,szbuffer);
}
//send the packets
void sendingContentTitle(string data,int numberOfPackets,SOCKET socket)
{
	int i=0;
	int k=0;
	//cout<<"DATA IS"<<data<<endl;
	//cout<<"DATA LENGTH IS"<<data.length()<<endl;
	//cout<<"number of packs is"<<numberOfPackets<<endl;

	int value=data.length()%BUFFER_SIZE;
	//cout<<"value IS"<<value<<endl;
	ostringstream x;
	x<<value ;
	string n=x.str();
	for(int j=0;j<numberOfPackets;j++)
	{		
		flushBuffer();
		k=j*BUFFER_SIZE;	
		i=0;
		if((j==(numberOfPackets-1))&&(value==0))
		{
			while(i<BUFFER_SIZE)
			{
				szbuffer[i]=data.at(k);
				i++;
				k++;
			}	
			sendingData(socket,szbuffer);
		}
		else if((j==(numberOfPackets-1))&&(value!=0)){
			while(i<value)
			{
				szbuffer[i]=data.at(k);
				i++;
				k++;
			}	

			sendingData(socket,szbuffer);

		}
		else{
			while(i<BUFFER_SIZE)
			{
				szbuffer[i]=data.at(k);
				i++;
				k++;
			}	
			sendingData(socket,szbuffer);
		}//else
	}

}
//acknowledgement recieving
void recieveAck(SOCKET socket,char buff[])
{

	if((ibytesrecv = recv(socket,buff,BUFFER_SIZE,0)) == SOCKET_ERROR)
		throw "Receive failed\n";

}
string recieveData(SOCKET socket)
{
	int numberOfPacketsRcvdFN= recievePacketsNb(socket);
	return recievePacketsContents(numberOfPacketsRcvdFN,s1);	
}

//Divide the string data to fit into packets

void packetsData(string data,SOCKET socket)
{

	int numberOfPackets;
	numberOfPackets=numberOfPacket(data.length());
	sendingNbOfPacketCtl(numberOfPackets,socket);
	flushBuffer();
	recieveAck(socket,szbuffer);
	flushBuffer();
	sendingContentTitle(data,numberOfPackets,socket);
	recieveAck(socket,szbuffer);
}
//to check if the file is in the directory
boolean found(string fileN)
{
	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFile(fileN.c_str() ,&fd);
	int count=0;
	if(hFile != INVALID_HANDLE_VALUE){
		do{
			if('.*' != fd.cFileName[0]){     
				cout<<"it's just founddd"<<endl;
				return true;
			}
		  }while(TRUE == FindNextFile(hFile, &fd));
	} 
	FindClose(hFile);
	return true;
}
string toString(int num)
{
	ostringstream sx;
	sx<<num ;
	string number=sx.str();
	return number;
}
char * stringToChar(string str)
{
	char *x;
	x=new char[str.size()+1];
	for (int i=0; i<str.size(); i++)
	{
		x[i]=str[i];
	}
	x[str.size()]='\0';
	return x;

}
void chunk(int packetsAmount,int value,char *buf,SOCKET socket)
{

	int k,i;
	k=i=0;
	for(int j=0;j<packetsAmount;j++)
	{		
		//cout << "number of packets yp be sent   :"<< numberOfPackets <<endl;
		flushBuffer();
		k=j*BUFFER_SIZE;	
		i=0;
		//string packetContent="";
		if((j==(packetsAmount-1))&&(value==0))
		{
			while(i<BUFFER_SIZE)
			{
				szbuffer[i]=buf[k];
				i++;
				k++;
			}	
			sendingData(socket,szbuffer);

			cout<<"Packet Nb="<<toString(j)<<"  -Contents are:"<<szbuffer<<endl<<endl;

		}
		else if((j==(packetsAmount-1))&&(value!=0)){
		/*	char * bu=new char[value];
			while(i<value)
			{
				bu[i]=buf[k];
				i++;
				k++;
			}	
			sendingData(socket,bu);
			string hi;
			hi.assign(bu,value);
			std::cout<<"Packet Nb="<<toString(j)<<"  -Contents are:"<<hi<<endl<<endl;
			delete[] bu;*/
			while(i<value)
			{
				szbuffer[i]=buf[k];
				i++;
				k++;
			}	

			sendingData(socket,szbuffer);

		}
		else{
			while(i<BUFFER_SIZE)
			{
				szbuffer[i]=buf[k];
				//cout<<szbuffer[i]<<endl;
				i++;
				k++;
			}
			sendingData(socket,szbuffer);
			std::cout<<"Packet Nb="<<toString(j)<<"  -Contents are:"<<szbuffer<<endl<<endl;
		}
	}
}
//readingFileAndSending
int  getTheFileFromServer(string fileName,SOCKET socket)
{
	char *buf;
	string file= ".\\" + fileName;
	char *filenamechar=stringToChar(file);
	
	FILE * pFile;

	
    if (found(fileName))
    {
		pFile=fopen( filenamechar ,"r");
		if (pFile!=NULL)
	  {
		//calculate the size
		
		int b=ftell(pFile);
		cout <<"begin"<<toString(b)<<endl;
		fseek(pFile, 0, SEEK_END);

		int e = ftell(pFile);
		cout<<"end"<<toString(e)<<endl;
		int size=e-b;
		cout<<"LENG="<<toString(size)<<endl;
		rewind (pFile);

		int packetsAmount=numberOfPacket(size);

		string number=toString(packetsAmount);
		flushBuffer();
		//send the number of packets to be sent
		cout<<"NUMBER OF PACKETS="<<number<<endl;
		sendingNbOfPacketCtl(packetsAmount,socket);
		flushBuffer();
		recieveAck(socket,szbuffer);
		flushBuffer();

		buf=new char[size-1];

		for (int i=0;i<size-1;i++)
			buf[i]='\0';

	//	cout<<"Buffer before sending is: "<<buf<<endl;
	//	char *bufsize;
	//	string sizestr=toString(size);
		//string app="i";
	//	sizestr.append(app);
	//	cout<<"The length of the buffer of the length must be:"<<sizestr<<endl;

	//	bufsize=new char[sizestr.length()];

	//	for (int i=0;i<sizestr.length();i++)
	//		bufsize[i]='\0';

	//	for(int i=0;i<sizestr.length();i++)
		//	bufsize[i]=sizestr.at(i);
	//	cout<<"BUFFFERRRRRRRRRRRRRRRRr="<<bufsize<<endl;
	//	bufsize=stringToChar(sizestr);
	
		fread (buf,1,size,pFile);

		//sendingData(socket,bufsize);
		//cout<<"HERE THE LENGTH OF THE FILES TO SENT"<<sizestr.length()<<endl;
		//  		   
	//	cout<<"I Read the File and it's size="<<sizestr<<endl;


		long value=size%BUFFER_SIZE;
		cout<<"DATA conntent"<<buf<<endl;
		
		cout<<"HERE THE FILES TO SENT"<<buf<<endl<<endl;
		cout<<"VALUE="<<value<<endl;
		chunk(packetsAmount,value,buf,socket);
		fclose (pFile);
	}
	}
	
	else {
		sendingNbOfPacketCtl(2,socket);
		flushBuffer();
		recieveAck(socket,szbuffer);
		flushBuffer();

		std::cout<<"Error while opening the file"<<endl;
		return -1;
    }
 
	/*			
	}else
	{

	sendingNbOfPacketCtl(2,socket);
	flushBuffer();
	recieveAck(socket,szbuffer);
	flushBuffer();

	cout << "File not found "<<fileName<< endl;

	return -1;			
	}*/
	return 1;

}
void ActOnDirectionGet(string fileName,SOCKET socket)
{
	int i=getTheFileFromServer(fileName,socket);

	if(i==1)
		cout<<"FILE Had been sent succefully"<<endl;
	else cout<<"fILE HAD TROUBLE WHILE SENDING"<<endl;
}



boolean stopListing(string d)
{
	if((d.compare(LIST)==0))
		return true;
	else return false;
}
string deleteEmptySpaces(string d)
{
	string str2= (" ");
	size_t found = d.find(str2);
	if (found!=std::string::npos){
		return d.substr(0,found);  
	}else
		return d;
}

//ActOnDirectionPUT(fileName,s1){}
string ifNum(string str)
{

	int j=0;
	while(str.at(j)!='i'){j++;}
	return str.substr(0,j);
	//toString(j);
}
int stringToInt(string str)
{
	return atoi(str.c_str()); 

}
void createNewFile(string path,string content)
{
	const char *buf;
	std::cout<<"The path is : "<<path<<endl;
	buf=new char[content.length()];
	buf=content.c_str();
	/*		FILE *fp=fopen("c:\D\t.pdf","w");
	int k=strlen(buf);
	fwrite(buf,k,1, fp);
	fclose(fp);
	*/
	ofstream fileWriter(path,ios::app);
	fileWriter.write(buf,content.length());

	fileWriter.close();
}
void deleteTheFile(string fi)
{
	const char* file_name = fi.c_str();
	int k= remove(file_name);
	if (k == 0) 
		cout << "File was successfully deleted"<<endl;
	else cout<<"file couldn't be deleted"<<endl;

}

boolean createFile(string fileName,string Data)
{
	ofstream fileWriter;
	string realPath=".\\"+fileName;
	//G:\My Documents\Visual Studio 2012\Projects\ClientNew\ClientNew\Debug
	if(found(realPath)){
		cout<<"DATA To be added is :"<<Data<<endl<<endl;;
		createNewFile(realPath,Data);

		cout<<"OKKK U ve Done it"<<endl;
	}else{
		cout<<"Next Step"<<endl;
		deleteTheFile(realPath);
		cout<<"DATA To be added is :"<<Data<<endl<<endl;;

		createNewFile(realPath,Data);
	}

	return false;
}
/////////////////////////////////////////////////////////////////////////////
void listTheFiles()
{
	// rearrange the files names in one string 
	vector<string> filesToDisplay=filesInDirectory();
	string lineOfData=vectorToString(filesToDisplay);
	packetsData(lineOfData,s1);
	flushBuffer();
}
////////////////////////////////////////////////////////////////////////////////////


int main(void){


	WSADATA wsadata;
	try{        		 
		if (WSAStartup(0x0202,&wsadata)!=0){  
			cout<<"Error in starting WSAStartup()\n";
		}
		else{
			buffer="WSAStartup was suuccessful\n";   
			WriteFile(test,buffer,sizeof(buffer),&dwtest,NULL); 
		}  

		//Display info of local hostfindfi

		gethostname(localhost,10);
		
		cout<<"ftpd_tcp starting at host: ["<<localhost<< "]"<<endl;
		cout<<"waiting to be contacted for tansferring files.."<<endl;

		if((hp=gethostbyname(localhost)) == NULL) {
			cout << "gethostbyname() cannot get local host info?"
				<< WSAGetLastError() << endl; 
			exit(1);
		}
		//Create the server socket
		if((s = socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET) 
			throw "can't initialize socket";
		// For UDP protocol replace SOCK_STREAM with SOCK_DGRAM 

		//Fill-in Server Port and Address info.
		sa.sin_family = AF_INET;
		sa.sin_port = htons(port);
		sa.sin_addr.s_addr = htonl(INADDR_ANY);

		//Bind the server port

		if (bind(s,(LPSOCKADDR)&sa,sizeof(sa)) == SOCKET_ERROR)
			throw "can't bind the socket";

		//Successfull bind, now listen for client requests.

		if(listen(s,10) == SOCKET_ERROR)
			throw "couldn't  set up listen on socket";
		//else cout << "Listen was successful" << endl;

		/*It's a handle that identifies the open file; it's generally called a file descriptor, hence the name fd.
		When you open the file, the operating system creates some resources that are needed to access it. These are
		stored in some kind of data structure (perhaps a simple array) that uses an integer as a key; the call to open
		returns that integer so that when you pass it read, the operating system can use it to find the resources it needs.*/

		FD_ZERO(&readfds);

		//wait loop

		while(1)

		{

			FD_SET(s,&readfds);  //always check the listener

			if(!(outfds=select(infds,&readfds,NULL,NULL,tp))) {}

			else if (outfds == SOCKET_ERROR) throw "failure in Select";

			else if (FD_ISSET(s,&readfds))  cout << "got a connection request" << endl; 

			//Found a connection request, try to accept. 

			if((s1=accept(s,&ca.generic,&calen))==INVALID_SOCKET)
				throw "Couldn't accept connection\n";

			//Connection request accepted.
			cout<<"accepted connection from "<<inet_ntoa(ca.ca_in.sin_addr)<<":"
				<<hex<<htons(ca.ca_in.sin_port)<<endl;
			string userName=recieveDataPacket(s1);
			
			userName=deleteEmptySpaces(userName);
			flushBuffer();
			//cout<<"yeghia"<<recieveDataPacket(s1)<<endl;
			//cout<<"USER NAME  "<<userName<<endl;
			//while the user asks for listing the data
			cout<<"List of Files on Server"<<endl;
			listTheFiles();

			while(1){

				//start with recieving the direction packet to know what to do
				string fileName=recieveDataPacket(s1);
				
				flushBuffer();

				string direction=recieveDataPacket(s1);
				flushBuffer();


				/*	while(stopListing(direction)){
				cout<<"here I'm"<<endl;
				listTheFiles();	
				}
				if(direction.compare(EXIT)==0){
				closesocket(s1);
				}
				*/if(direction.compare(GET)==0){
					//cout<< "REcieved Direction  is "<<direction<<endl;	
					//cout<<endl<<endl;
					/*	string fileName=recieveDataPacket(s1);
					cout<< "REcieved file name is "<<fileName<<endl;
					flushBuffer();			
					*/	ActOnDirectionGet(fileName,s1);

				}

			}


			/*else
			if(direction.compare(PUT)==0){

			cout<< "REcieved Direction  is "<<direction<<endl;	
			cout<<endl<<endl;
			//ActOnDirectionPUT(fileName,s1);
			cout<<"Not Already Asked"<<endl;
			cout<<"I'm waiting for data from Client"<<endl;
			//	char fileName[128];
			cout<< "REcieved Direction  is "<<direction<<endl;	
			cout<<endl<<endl;
			string fileName=recieveDataPacket(s1);
			cout<< "REcieved file name is "<<fileName<<endl;
			flushBuffer();			

			//recieve acknowledgement that data is found
			int packValue=recievePacketsNb(s);
			cout<< "I recieved  packets"<< toString(packValue);
			char ack[1];
			ack[0]='0';
			sendPacketAck(s,ack);
			if(packValue==0){cout<< "File couldn't be opened";}
			else if(packValue==2){cout<<"File doesn't exist in the directory";}
			else {
			flushBuffer();
			char buf[128];

			string size="";
			string dataContent="";
			//buf=new char[128];
			if((ibytesrecv = recv(s,buf,128,0)) == SOCKET_ERROR)
			throw "Receive failed\n";
			else
			//{ cout << "hip hip hoorah!: Successful message replied from server: " <<szbuffer<< endl;  
			{
			size=buf;	
			cout<<"LENGTH OF THE FILE TO BE RECIEVED :"<<size<<endl;
			}
			string r=ifNum(size);
			cout<<"REAL ="<<r<<endl;

			int toRead=stringToInt(r);

			string filesFromServer=recievePacketsContents(packValue,s);
			cout<<filesFromServer<<endl;
			cout<<endl<<endl;
			cout<<"????????????"<<endl;
			//createFile(fileN,filesFromServer);
			createFile(fileName,dataContent);

			cout<<"File name is "<<fileName<<endl;
			}

			} 			

			//sendRecivedFileNameAck()
			/*
			string str2= ("\\");
			ifstream fileToRead;
			size_t found = fileName.find(str2);
			if (found!=std::string::npos)
			{
			//	search in default directory
			std::cout << "first 'needle' found at: " << found << '\n';
			fileToRead.open(str);
			int size=fileToRead.tellg();
			int packetsAmount=numberOfPacket(size);
			ostringstream sx;
			sx<<packetsAmount ;
			string number=sx.str();
			flushBuffer();
			for (int i=0;i<number.size();i++)
			szbuffer[i]=number.at(i);




			recieveDirection();
			sendRecieveDirectionAck();
			// create the packets to send the files names in it

			// the first message to be sent is the number of the packets to be sent after

			cout<< "packet content"<<szbuffer<<endl;

			sendingData(s1,szbuffer);
			recievingData(s1,szbuffer);

			fileToRead.close();
			}else
			{
			//search in the specified path 
			}
			// send the file to the client





			//Send to Client the received message (echo it back).
			ibufferlen = strlen(szbuffer);

			if((ibytessent = send(s1,szbuffer,ibufferlen,0))==SOCKET_ERROR)
			throw "error in send in server program\n";
			else cout << "Echo message:" << szbuffer << endl;  */
		}//wait loop

	} //try loop

	//Display needed error message.

	catch(char* str) { cerr<<str<<WSAGetLastError()<<endl;}

	//close Client socket
	closesocket(s1);		

	//close server socket
	closesocket(s);

	/* When done uninstall winsock.dll (WSACleanup()) and exit */ 
	WSACleanup();
	return 0;
}