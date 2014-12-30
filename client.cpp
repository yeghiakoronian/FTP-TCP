


#pragma comment( linker, "/defaultlib:ws2_32.lib" )
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <winsock.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>

#include <windows.h>
#include <Winbase.h>
#include <Lmcons.h>
  

#pragma once

#include <process.h>


#include <string>


using namespace std;

//user defined port number
#define REQUEST_PORT 0x7070;
#define GET "GET"
#define PUT "put"
#define LIST "list"
#define EXIT "exit"
#define DEFAULT_PATH ".\\"
//G:\My Documents\Visual Studio 2012\Projects\Project1\Project1\FILES
string ans;
int port=REQUEST_PORT;



//socket data types
SOCKET s;
SOCKADDR_IN sa;         // filled by bind
SOCKADDR_IN sa_in;      // fill with server info, IP, port



//buffer data types
char szbuffer[128];
const int BUFFER_SIZE=128;
string filenamestringfromuser;
char *buffer;

int ibufferlen=0;

int ibytessent;
int ibytesrecv=0;



//host data types
HOSTENT *hp;
HOSTENT *rp;

char localhost[11],
	remotehost[11];


//other

HANDLE test;

DWORD dwtest;

//
static int numberOfPacketsRecieved=0;
vector<string> filesInLocalDirectory()
{
	vector<string> filenames = vector<string>();
	WIN32_FIND_DATA fd;
	HANDLE ff = FindFirstFile(".\\*", &fd);
	

	if (ff != INVALID_HANDLE_VALUE)
	{ 
		
		do 
		{
			if (fd.cFileName[0] == '.') continue;
			//printf("%s",fd.cFileName);
			filenames.push_back(string(fd.cFileName));
		} 
		while (FindNextFile(ff, &fd)); 
		FindClose(ff); 
	}

	return filenames;
}

string vectorToLocalString(vector<string> vect)
{
	string toSend="";
	for (int i=0;i<vect.size();i++)
		toSend=toSend+vect.at(i);
	return toSend;
}

string listTheLocalFiles()
{
	// rearrange the files names in one string 
	vector<string> filesToDisplay=filesInLocalDirectory();
	string lineOfData=vectorToLocalString(filesToDisplay);
	//packetsData(lineOfData,s1);
	//flushBuffer();
	return lineOfData;
}


void listTheFiles(){
	HANDLE hFind;
	WIN32_FIND_DATA data;
	hFind = FindFirstFile(".\\*", &data);
	
	if (hFind != INVALID_HANDLE_VALUE) {
	do {
		if (data.cFileName[0] == '.') continue;
		printf("%s\n", data.cFileName);
	} while (FindNextFile(hFind, &data));
	FindClose(hFind);
}
}





int recievePacketsNb(SOCKET socket){

	//cout<< "ok I'm trying to recieve now"<< endl;

	if((ibytesrecv = recv(socket,szbuffer,128,0)) == SOCKET_ERROR)
		throw "Receive failed\n";
	string str=szbuffer;
	//cout<<"ACKKK VALUE="<<str<<endl;
	return atoi(str.c_str()); 

}

string toString(int num){
	ostringstream sx;
	sx<<num ;
	string number=sx.str();
	return number;
}
boolean empty(char *bu){
	for(int i=0;i<128;i++)
		if(!(bu[i]==' '))
			return false;
	return true;
}
//to empty the contents of a buffer
void flushBuffer()
{//char buff[]){
	for (int i=0;i<BUFFER_SIZE;i++)
		szbuffer[i]='\0';
}

string recievePacketsContentsk(int realLength,int numberOfPacketsRecieved,SOCKET socket){
	string  dataContent="";
	//cout<< "ok I'm trying to recieve the packets contents now"<< endl;
	cout<<"numberOfPacketsRecieved:"<<numberOfPacketsRecieved<<endl;
	for(int i=0;i<numberOfPacketsRecieved;i++)
	{
		flushBuffer();
		cout<<"i"<<i<<endl;
		if((ibytesrecv = recv(socket,szbuffer,BUFFER_SIZE,0)) == SOCKET_ERROR)
			throw "Receive failed\n";
		else
			//{ cout << "hip hip hoorah!: Successful message replied from server: " <<szbuffer<< endl;  
		{
			int value=realLength% BUFFER_SIZE;

			if((i==numberOfPacketsRecieved-1)&& (value!=0)){

				cout<<toString(value)<<endl;
				cout<<toString(realLength)<<endl;
				char *data=new char[value];

				for(int y=0;y<value;y++){
					data[y]=szbuffer[y];

					cout<<"buffer of last packet"<<szbuffer[y]<<endl;
				}
				string hi;
				cout<<data<<endl;
				hi.assign(data,value);
				dataContent=dataContent.append(hi);
				delete[] data;
				cout<<"FINAL BLOCK Packet NB:"<<i<<"  with value:"<<dataContent<<endl;
				cout<<endl<<endl;

				return dataContent;
			}else
				if(empty(szbuffer))
				{

					for(int i=0;i<BUFFER_SIZE;i++)
						dataContent=dataContent.append(" ");
				}
				else{
					dataContent=dataContent.append(szbuffer);	
					cout<<"Data Recieved in Packet NB:"<<i<<"  with value:"<<dataContent<<endl;
					cout<<endl<<endl;
				}
		}
		//flushBuffer();
	}
	return dataContent;
}
void sendPacketAck(SOCKET socket,char buff[]){
	int bufferlen = strlen(buff);
	ibytessent=0;
	if((ibytessent = send(socket,buff,bufferlen,0))==SOCKET_ERROR)
		throw "error in send in server program\n";
	//	else cout << "Echo message:" << buff << endl;  	

}


string recievePacketsContents(int numberOfPacketsRecieved,SOCKET socket)
{
	string  dataContent="";
	//cout<< "ok I'm trying to recieve the packets contents now"<< endl;

	for(int i=0;i<numberOfPacketsRecieved;i++)
	{
	//	if(i==(numberOfPacketsRecieved-1))
	//{
	//		int j=0;
	//		
	//	}
		flushBuffer();
		if((ibytesrecv = recv(socket,szbuffer,BUFFER_SIZE,0)) == SOCKET_ERROR)
			throw "Receive failed\n";
		else
			{ //cout << " message replied from server: nn packet=" <<i<<"data:"<< szbuffer<< endl; 
		if(empty(szbuffer)){cout<<"empty"<<endl;}
		dataContent=dataContent.append(szbuffer);	}
	}
	return dataContent;
}
//
string recieveDataPacket(SOCKET socket)
{
	int pack_NB=recievePacketsNb(socket);
	char ack[1];
	ack[0]='0';
	//cout<<"recieving"<<endl;
	sendPacketAck(socket,ack);
	string content=recievePacketsContents(pack_NB,socket);
	sendPacketAck(socket,ack);
	return content;
}
void recievePacketsContents(SOCKET socket){
	cout<< "ok I'm trying to recieve the packets contents now"<< endl;
	//for(int i=0;i<packetsNb;i++)

	if((ibytesrecv = recv(socket,szbuffer,128,0)) == SOCKET_ERROR)
		cout<< "Receive failed\n"<< endl;
	else
	{
		cout << "hip hip hoorah!: Successful message replied from server: " <<szbuffer<< endl;  
		numberOfPacketsRecieved=numberOfPacketsRecieved+1;
	}
}


void sendingData(SOCKET socket,char buff[]){
	int bufferlen = strlen(buff);

	if((ibytessent = send(socket,buff,bufferlen,0))==SOCKET_ERROR)
		throw "error in send in server program\n";
}

int numberOfPacket(int contentSize){

	int numberOfPackets=0;

	if(contentSize % BUFFER_SIZE==0)
		numberOfPackets=int(contentSize/BUFFER_SIZE);
	else 
		numberOfPackets=int(contentSize/BUFFER_SIZE)+1;
	return numberOfPackets;

}

void sendingNbOfPacketCtl(int numberOfPackets,SOCKET socket){

	string number=toString(numberOfPackets);
	for (int i=0;i<number.size();i++)
		szbuffer[i]=number.at(i);

	// the first message to be sent is the number of the packets to be sent after
	sendingData(socket,szbuffer);
	flushBuffer();
}
//to send the packet content to the other side
void sendingContent(string data,int numberOfPackets,SOCKET socket){
	int i=0;
	int k=0;
	int value=data.length()%BUFFER_SIZE;
	//string n=toString(value);

	for(int j=0;j<numberOfPackets;j++){		
		flushBuffer();
		k=j*BUFFER_SIZE;	
		i=0;
		string packetContent="";
		if((j==(numberOfPackets-1))&&(value==0))
		{
			while(i<BUFFER_SIZE)
			{
				szbuffer[i]=data.at(k);
				i++;
				k++;
			}	
			//	cout<<"data in buffer to send now"<<szbuffer<<endl;
			sendingData(socket,szbuffer);
		}
		else if((j==(numberOfPackets-1))&&(value!=0)){
			while(i<value)
			{
				szbuffer[i]=data.at(k);
				i++;
				k++;
			}	
			//cout<<"data in buffer to send now"<<szbuffer<<endl;
			sendingData(socket,szbuffer);
		}
		else{
			while(i<BUFFER_SIZE)
			{
				szbuffer[i]=data.at(k);
				i++;
				k++;
			}	
			//cout<<"data in buffer to send now"<<szbuffer<<endl;
			sendingData(socket,szbuffer);
		}//else
	}

}
void recieveAck(SOCKET socket,char buff[]){
	//	cout<< "ok I'm recieving ACK"<< endl;

	if((ibytesrecv = recv(socket,buff,BUFFER_SIZE,0)) == SOCKET_ERROR)
		throw "Receive failed\n";

}

// divide the data to be exchanged to packets based on the buffer size BY sending the packets total amount then send it one by one
void packetsData(string data,SOCKET socket)
{
	int dataSize=data.length();
	int numberOfPackets;
	numberOfPackets=numberOfPacket(dataSize);
	string number=toString(dataSize);
	sendingNbOfPacketCtl(numberOfPackets,socket);
	flushBuffer();
	recieveAck(socket,szbuffer);
	flushBuffer();
	sendingContent(data,numberOfPackets,socket);
	recieveAck(socket,szbuffer);
}


void sendPacketNbAck(){
	ibufferlen = strlen(szbuffer);
	ibytessent = send(s,szbuffer,ibufferlen,0);
	if (ibytessent == SOCKET_ERROR)
		throw "Send failed\n";  
	else
		cout << "the number of packets expected to be recieved = " << szbuffer<<endl;

}
//boolean found(string fileN){
//	ifstream sReader;
//	sReader.open(fileN,ios::in|ios::app);
//	cout<<"FILE NAME="<<fileN<<endl;
//	if(sReader.is_open()) {
//		cout<<"File already exist"<<endl;
//		return false;
//	}else{
//		cout<<"File is totally new"<<endl;
//		return true;}
//}
boolean found(string fileN)
{
	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFile(fileN.c_str() ,&fd);

	int count=0;
	if(hFile != INVALID_HANDLE_VALUE)
	{
	do
	{
	if('.*' != fd.cFileName[0])
	{     

	cout<<"founddd"<<endl;
	return true;
	}
	}while(TRUE == FindNextFile(hFile, &fd));
	} 

	FindClose(hFile);
	return true;

}
void createNewFile(string path,string content){
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
void deleteTheFile(string fi){


	const char* file_name = fi.c_str();
	int k= remove(file_name);
	if (k == 0) 
		cout << "File was successfully deleted"<<endl;
	else cout<<"file couldn't be deleted"<<endl;

}
boolean createFile(string fileName,string Data){
	ofstream fileWriter;
	string realPath=".\\" +fileName;
	cout<<realPath;
	if(!found(realPath)){
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

char * stringToChar(string str){
	char *x;
	x=new char[str.size()+1];
	for (int i=0; i<str.size(); i++)
	{
		x[i]=str[i];
	}
	x[str.size()]='\0';
	return x;

}



boolean stopListing(string d){
	if((d.compare(LIST)==0))
		return true;
	else return false;
}
string deleteEmptySpaces(string d){
	string str2= (" ");
	size_t found = d.find(str2);
	if (found!=std::string::npos){
		cout<<"Found @:"<<found<<endl;

		return d.substr(0,found);  
	}else
	{cout<<"Not Foung"<<endl;
	return d;
	}
}
string ifNum(string str){

	int j=0;
	while(str.at(j)!='i'){j++;}
	return str.substr(0,j);
	//toString(j);
}
int stringToInt(string str){
	return atoi(str.c_str()); 

}


//readingFileAndSending
int  getTheFileFromServer(string fileName,SOCKET socket){

	ifstream fileToRead;
	FILE *file;
	char ch;
	string fullPath=DEFAULT_PATH+fileName;
	cout<<"Full path is"<<fullPath<<endl;
	if (found(fullPath)){
		fileToRead.open(fullPath,ios::in|ios::app);
		//check that the stream could open the file
		if(fileToRead.is_open()) {


			//calculate the size
			int b=fileToRead.tellg();
			cout <<"begin"<<toString(b)<<endl;
			cout<<"OPENEDDDDDDDDDDDd"<<endl;
			fileToRead.seekg (0, fileToRead.end);
			int e = fileToRead.tellg();
			cout<<"end"<<toString(e)<<endl;
			fileToRead.seekg (0, fileToRead.beg);
			int size=e-b;

			cout<<"LENG="<<toString(size)<<endl;
			int packetsAmount=numberOfPacket(size);

			fileToRead.clear();
			fileToRead.seekg(0,fileToRead.beg);

			string number=toString(packetsAmount);
			flushBuffer();
			//send the number of packets to be sent
			cout<<"NUMBER OF PACKETS="<<number<<endl;
			sendingNbOfPacketCtl(packetsAmount,socket);
			flushBuffer();
			recieveAck(socket,szbuffer);
			flushBuffer();

			int i,k;
			i=k=0;
			char *buf;
			char ch;
			buf=new char[size-1];
			for (int i=0;i<size-1;i++)
				buf[i]=' ';
			cout<<"Buffer before sending is: "<<buf<<endl;
			char *bufsize;
			string sizestr=toString(size);
			string app="i";
			sizestr.append(app);
			cout<<"The length of the buffer of the length must be:"<<sizestr<<endl;

			bufsize=new char[sizestr.length()];

			for (int i=0;i<sizestr.length();i++)
				bufsize[i]=' ';

			for(int i=0;i<sizestr.length();i++)
				bufsize[i]=sizestr.at(i);
			cout<<"BUFFFERRRRRRRRRRRRRRRRr="<<bufsize<<endl;
			bufsize=stringToChar(sizestr);
			fileToRead.seekg(0,ios::beg);
			fileToRead.read(buf,size);
			sendingData(socket,bufsize);
			cout<<"HERE THE LENGTH OF THE FILES TO SENT"<<sizestr.length()<<endl;

			cout<<"I Read the File and it's size="<<sizestr<<endl;
			int value=size%BUFFER_SIZE;
			cout<<"DATA conntent"<<buf<<endl;
			//	   	sendingData(socket,buf);
			cout<<"HERE THE FILES TO SENT"<<buf<<endl;
			for(int j=0;j<packetsAmount;j++){		
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
					cout<<"Packet Nb="<<j<<"  -Contents are:"<<szbuffer<<endl;

				}
				else if((j==(packetsAmount-1))&&(value!=0)){
					while(i<value)
					{
						szbuffer[i]=buf[k];
						i++;
						k++;
					}	
					sendingData(socket,szbuffer);
					cout<<"Packet Nb="<<j<<"  -Contents are:"<<szbuffer<<endl;

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
					cout<<"Packet Nb="<<j<<"  -Contents are:"<<szbuffer<<endl;
				}//else
			}
			fileToRead.close();
			delete[] buf;
			return 1;
		} 
		else {
			sendingNbOfPacketCtl(2,socket);
			flushBuffer();
			recieveAck(socket,szbuffer);
			flushBuffer();

			std::cout<<"Error while opening the file"<<endl;
			return -1;
		}	   

	}else
	{

		sendingNbOfPacketCtl(2,socket);
		flushBuffer();
		recieveAck(socket,szbuffer);
		flushBuffer();

		cout << "File not found "<<fileName<< endl;

		return -1;			
	}
	return 1;

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

int  getTheFileFromClient(string fileName,SOCKET socket)
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
void ActOnDirectionPUT(string fileName,SOCKET socket){

	int i=getTheFileFromClient(fileName,socket);

	if(i==1)
		cout<<"FILE Had been sent succefully"<<endl;
	else cout<<"fILE HAD TROUBLE WHILE SENDING"<<endl;
}

string  getCurrentUserName() {

	TCHAR username[UNLEN + 1];
	DWORD size = UNLEN + 1;
	GetUserName(username, &size);
	if (GetUserName(username, &size ))
	{ 

		TCHAR *g=username;
		int size=0;
		while( (char)username[size] != '\0')
			size++;
	}
	size++;
	//cout<<(string) username<<endl;
	return (string) username; 
}

int main(void){
	char direction[128];
	string direc;
	char fileName[128];
	boolean exited=false;
	WSADATA wsadata;
	try {

		if (WSAStartup(0x0202,&wsadata)!=0){  
			cout<<"Error in starting WSAStartup()" << endl;
		} else {
			buffer="WSAStartup was successful\n";   
			WriteFile(test,buffer,sizeof(buffer),&dwtest,NULL); 

		}  
		//display current files on C
		//Display name of local host.

		gethostname(localhost,10);
		cout<<"ftp_tcp starting on host : " << localhost << endl;

		if((hp=gethostbyname(localhost)) == NULL) 
			throw "gethostbyname failed\n";

		//Ask for name of remote server

		cout << "Type name of ftp server : " << flush ;   
		cin >> remotehost ;

		if((rp=gethostbyname(remotehost)) == NULL)
			throw "remote gethostbyname failed\n";

		//Create the socket
		if((s = socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET) 
			throw "Socket failed\n";
		/* For UDP protocol replace SOCK_STREAM with SOCK_DGRAM */

		//Specify server address for client to connect to server.
		memset(&sa_in,0,sizeof(sa_in));
		memcpy(&sa_in.sin_addr,rp->h_addr,rp->h_length);
		sa_in.sin_family = rp->h_addrtype;   
		sa_in.sin_port = htons(port);

		//Display the host machine internet address

		//cout << "Connecting to remote host:";
		//cout << inet_ntoa(sa_in.sin_addr) << endl;

		//Connect Client to the server
		if (connect(s,(LPSOCKADDR)&sa_in,sizeof(sa_in)) == SOCKET_ERROR)
			throw "connect failed\n";
		//get current user name
		string userName=getCurrentUserName();
		packetsData(userName,s);
		flushBuffer();
		cout<<"\n"<<endl;
		cout<<"Files on your local machine are:"<<endl;
		listTheFiles();
		cout<<"\n\n"<<endl;
		//Ask the operation that the user want to do : put or get or List	
		string filesFromServer=recieveDataPacket(s);
		cout<<"Files Present on server:"<<endl;
		cout<<filesFromServer<<endl;
		bool filepresent=false;
		while (!filepresent)
		{
			cout<<"Type name of the file to be transferred : "<<flush; 
			cin>>fileName;
			string filenamestring=string(fileName);
			filenamestringfromuser =  filenamestring;
			if (filesFromServer.find(filenamestring)!= string::npos || listTheLocalFiles().find(filenamestring)!= string::npos)
				filepresent=true;
			else 
				cout<<"File doesnt exist neither on  your machine nor server"<<endl;
		}
		char direction[128];
		char c;
		bool directionflag=false;
		string Directionstring;
		while (!directionflag)
		{
			cout<< "Type direction of transfer : GET or PUT"<<endl;
			cin>>direction;
			for(int i=0;i<3;i++)
			{
				c=direction[i];
				direction[i]=toupper(c); 
			}

			 Directionstring=string(direction);

			if (Directionstring.compare("GET") || Directionstring.compare("PUT"))
				directionflag=true;
		}

		ibytesrecv=0; 
		ibytessent=0;
		packetsData(fileName,s);
		flushBuffer();

		packetsData(direction,s);
		flushBuffer();
		
		if(Directionstring.compare("GET")==0  && filesFromServer.find(filenamestringfromuser)!= string::npos )
		{
			
			cout<<"Send Request To   "<<remotehost<<"  Waiting ..."<< endl;
			//recieve acknowledgement that data is found
			int packValue=recievePacketsNb(s);
			cout<< "I recieved  packets"<< toString(packValue)<<endl;
			char ack[1];
			ack[0]='0';
			sendPacketAck(s,ack);
			if(packValue==0)
			{
				cout<< "File couldn't be opened";
			}
			
			
			
			
			
			
			
			
			
			
			
			else if(listTheLocalFiles().find(filenamestringfromuser)!= string::npos)
			{

				cout<<"File already exists in your directory wo you like to overwrite  Y/N?"<<endl;
				cin>>ans;
				if(ans=="Y")
				{
				flushBuffer();
				
				string filesFromServer=recievePacketsContents(packValue,s);
				//cout<<filesFromServer<<endl;
				//cout<<endl<<endl;
				//cout<<"????????????"<<endl;
				//createFile(fileN,filesFromServer);
				createFile(fileName,filesFromServer);

				cout<<"File name is "<<fileName<<endl;
				while(1){}
				}
				else 
				{
					cout<<"waiting"<<endl;
				}
			}
			
			
			else if(listTheLocalFiles().find(filenamestringfromuser)== string::npos)
			{

				
				flushBuffer();
				
				string filesFromServer=recievePacketsContents(packValue,s);
				//cout<<filesFromServer<<endl;
				//cout<<endl<<endl;
				//cout<<"????????????"<<endl;
				//createFile(fileN,filesFromServer);
				createFile(fileName,filesFromServer);

				cout<<"File name is "<<fileName<<endl;
				while(1){}
				
				
			}
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
		}
		
		if(Directionstring.compare("GET")==0  && filesFromServer.find(filenamestringfromuser)== string::npos )
		
		{
			 cout<<"File not found on server"<<endl;
			 system("pause");
		}
		if(Directionstring.compare("PUT")==0  && listTheLocalFiles().find(filenamestringfromuser)!= string::npos )
		{
			ActOnDirectionPUT(fileName,s);
			while(true){}
		}
		if(Directionstring.compare("PUT")==0  && listTheLocalFiles().find(filenamestringfromuser)== string::npos)
		{
			cout<<"CLIENT doesnt have the file"<<endl;
			system("pause");
		}
		
	}//try loop

	//Display any needed error response.

	catch (char *str) { cerr<<str<<":"<<dec<<WSAGetLastError()<<endl;}

	//close the client socket
	closesocket(s);

	/* When done uninstall winsock.dll (WSACleanup()) and exit */ 
	WSACleanup();  
	return 0;
}
