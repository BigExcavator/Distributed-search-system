#include "config.h"


using namespace std;

int main (int argc, char* argv[]){

/*****************************************************************************************
PHASE 1A: Client boot up, and be ready to send the request
===The code for initializing, binding socket, and building network address has referenced
====================="http://www.beej.us/guide/bgc/"======================================
*****************************************************************************************/

	//if the number of commands is wrong, terminate.
	if(argc!=3){
		cout<<"The input should obey the format: ./client <function> <input>."<<endl;
		cout<<"Please try again."<<endl;
		return 0;
	}

	int sockfd_client; //variable for client socket
	int dataLen; //the length of data sent or received
	struct sockaddr_in aws_addr; //remote server network address
	string comd1, comd2; // store the user's commands
	vector<string> dataSend, matchData; // the vectors for storing commands, storing results

	//read user's commend, if illegal, terminate
	comd1=argv[1];
	if((comd1.compare("search")) && (comd1.compare("prefix")) && (comd1.compare("suffix"))){
		cout<<"The first commend should be search or prefix or suffix"<<endl;
		cout<<"Please try again."<<endl;
		return 0;
	}

	dataSend.push_back(comd1);

	//read user's commend, if illegal, terminate
	comd2=argv[2];
	if(comd2.length()>27){
		cout<<"The length of the words should be no longer than 27"<<endl;
		cout<<"Please try again."<<endl;
		return 0;
	}
	dataSend.push_back(comd2);

	//Initialize the client's tcp socket-----Beej's
	if((sockfd_client=socket(AF_INET, SOCK_STREAM, 0))<0){
		cout<<"Socket building error detected"<<endl;
		return 0;
	}

	//Initialize the remote aws's network address-----Beej's
	memset(&aws_addr, 0, sizeof(aws_addr));
    aws_addr.sin_family = AF_INET;
    aws_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    aws_addr.sin_port = htons(TCP_AWS_C);

    //Connect to the aws server-----Beej's
    if(connect(sockfd_client, (struct sockaddr*)&aws_addr, sizeof(aws_addr)) < 0){
    		cout << "Connection error detected" << endl;
    		return 0;
    }

    //printOut the booting up information
    cout << "The client is up and running." << endl;

/*****************************************************************************************
PHASE 1B: Client send the request to the aws server over tcp connection
*****************************************************************************************/
    for(int i=0; i<dataSend.size(); i++){
    		dataLen=send(sockfd_client, dataSend[i].c_str(), BUFF_SIZE, 0);
    }

    //printOut the command sending finished message
    cout << "The client sent <"<<comd2<<"> and <"<<comd1<<"> to AWS." << endl;

/*****************************************************************************************
PHASE 2: Client receive the results given by the aws server
*****************************************************************************************/

    //if the operation is search
    if(!comd1.compare("search")){

    		//get how many definition found
    		int num_def;
    		char* aws_data=new char[BUFF_SIZE];
    		recv(sockfd_client, aws_data, BUFF_SIZE, 0);
    		num_def=atoi(aws_data);
    		free(aws_data);
    		aws_data=NULL;

    		//printOut the founded definition
    		if(num_def!=0){
    			char* d_data=new char[BUFF_SIZE];
    			cout<<"Found a match for <"<<comd2<<">:"<<endl;
    			recv(sockfd_client, d_data, BUFF_SIZE, 0);
    			cout<<"<"<<string(d_data)<<">"<<endl;
    			free(d_data);
    			d_data=NULL;
    		}
    		else{
    			cout<<"Found no matches for <"<<comd2<<">"<<endl;
    		}
    }
    //if the operation is prefix/suffix
    else{
    		//get how may matched found
    		int num_match;
    		char* m_num_data=new char[BUFF_SIZE];
    		recv(sockfd_client, m_num_data, BUFF_SIZE, 0);
    		num_match=atoi(m_num_data);
    		free(m_num_data);
    		m_num_data=NULL;

    		//printOut the founded matches
    		if(num_match!=0){
    			for(int i=0; i<num_match; i++){
    				char* m_data=new char[BUFF_SIZE];
    				recv(sockfd_client, m_data, BUFF_SIZE, 0);
    				matchData.push_back(string(m_data));
    				free(m_data);
    				m_data=NULL;
    			}
    			cout<<"Found <"<<num_match<<"> matches for <"<<comd2<<">:"<<endl;
    			for(int i=0; i<num_match; i++){
    				cout<<"<"<<matchData[i]<<">"<<endl;
    			}
    		}
    		else{
    			cout<<"Found no matches for <"<<comd2<<">"<<endl;
    		}

    }

    //close the socket and clean	the container
    close(sockfd_client);
    matchData.clear();

    return 0;
}
