#include "config.h"


using namespace std;

int main (int argc, char* argv[]){

/*****************************************************************************************
PHASE 1A: Monitor boot up, and be ready to send the request
===The code for initializing, binding socket, and building network address has referenced
====================="http://www.beej.us/guide/bgc/"======================================
*****************************************************************************************/
	int sockfd_monitor; //variable for client socket
	struct sockaddr_in aws_addr; //remote server net address
	vector<string> match_data, def_data, simi_data; // vectors for store data
	string commend, input; // the type of operation, the input word
	int length_similar, length_def, length_match;// number of similar words, number of definitions, number of matches

	//Initialize the client's tcp socket-----Beej's
	if((sockfd_monitor=socket(AF_INET, SOCK_STREAM, 0))<0){
		cout<<"Socket building error detected"<<endl;
		return 0;
	}

	//Initialize the remote aws's network address-----Beej's
	memset(&aws_addr, 0, sizeof(aws_addr));
    aws_addr.sin_family = AF_INET;
    aws_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    aws_addr.sin_port = htons(TCP_AWS_M);

    //Connect to the server-----Beej's
    if(connect(sockfd_monitor, (struct sockaddr*)&aws_addr, sizeof(aws_addr)) < 0){
    		cout << "Connection error detected" << endl;
    		return 0;
    }

    //printOut the booting up message
    cout << "The Monitor is up and running." << endl;

/****************************************************************************************
PHASE 2: Receive the results given by the aws server and display it on the screen
****************************************************************************************/
    while(1){

    		//receive the type of operation
    		char* as_data=new char[BUFF_SIZE];
    		recv(sockfd_monitor, as_data, BUFF_SIZE, 0);
    		commend=string(as_data);
    		free(as_data);
    		as_data=NULL;

    		if(!commend.compare("search")){
    			//receive the client's input word from aws
    			char* in_data=new char[BUFF_SIZE];
    			recv(sockfd_monitor, in_data, BUFF_SIZE, 0);
    			input=string(in_data);
    			free(in_data);
    			in_data=NULL;

    			//receive the number of definition
    			char* nd_data=new char[BUFF_SIZE];
    			recv(sockfd_monitor, nd_data, BUFF_SIZE, 0);
    			length_def=atoi(nd_data);
    			free(nd_data);
    			nd_data=NULL;

    			if(length_def==0){
    				cout<<"Found no matches for <"<<input<<">"<<endl;
    			}
    			else{
    				//if there is a definition, then we receive and display it
    				char* df_data=new char[BUFF_SIZE];
    				recv(sockfd_monitor, df_data, BUFF_SIZE, 0);
    				def_data.push_back(string(df_data));
    				free(df_data);
    				df_data=NULL;
    				cout<<"Found a match for <"<<input<<">:"<<endl;
    				cout<<"<"<<def_data[0]<<">"<<endl;
    			}

    			//receive the number of one distance words
    			char* ns_data=new char[BUFF_SIZE];
    			recv(sockfd_monitor, ns_data, BUFF_SIZE, 0);
    			length_similar=atoi(ns_data);
    			free(ns_data);
    			ns_data=NULL;

    			if(length_similar==0){
    				cout<<"Found no one edit distance word for <"<<input<<">"<<endl;
    			}
    			else{
    				//if there is a similar word, then we receive this word
    				char* sw_data=new char[BUFF_SIZE];
    				recv(sockfd_monitor, sw_data, BUFF_SIZE, 0);
    				simi_data.push_back(string(sw_data));
    				free(sw_data);
    				sw_data=NULL;
    				//then we receive the definition of this word
    				char* sd_data=new char[BUFF_SIZE];
    				recv(sockfd_monitor, sd_data, BUFF_SIZE, 0);
    				simi_data.push_back(string(sd_data));
    				free(sd_data);
    				sd_data=NULL;

    				cout<<"One edit distance match is <"<<simi_data[0]<<">:"<<endl;
    				cout<<"<"<<simi_data[1]<<">"<<endl;
    			}


    		}
    		else if((!commend.compare("prefix")) || (!commend.compare("suffix"))){
    			//receive the client's input word from aws
    			char* in_data=new char[BUFF_SIZE];
    			recv(sockfd_monitor, in_data, BUFF_SIZE, 0);
    			input=string(in_data);
    			free(in_data);
    			in_data=NULL;

    			//receive the number of matches
    			char* nm_data=new char[BUFF_SIZE];
    			recv(sockfd_monitor, nm_data, BUFF_SIZE, 0);
    			length_match=atoi(nm_data);
    			free(nm_data);
    			nm_data=NULL;

    			if(length_match==0){
    				cout<<"Found no matches for <"<<input<<">"<<endl;
    			}
    			else{
    				//receive the matched words
    				for(int i=0; i<length_match; i++){
    					char* m_data=new char[BUFF_SIZE];
    					recv(sockfd_monitor, m_data, BUFF_SIZE, 0);
    					match_data.push_back(string(m_data));
    					free(m_data);
    					m_data=NULL;
    				}
    				cout<<"Found "<<"<"<<match_data.size()<<">"<<" matches for <"<<input<<">:"<<endl;
    				//print out all the match results
    				for(int i=0; i<length_match; i++){
    					cout<<"<"<<match_data[i]<<">"<<endl;
    				}
    			}
    		}

    		//clean all the containers
    		length_similar=0;
    		length_def=0;
		length_match=0;

		match_data.clear();
		simi_data.clear();
		def_data.clear();
    }

    close(sockfd_monitor);
    return 0;
}
