#include "config.h"

using namespace std;

bool isOneEditDistance(string s, string t);
bool is_prefix(string s, string prefix);
bool is_suffix(string s, string suffix);

int main (int argc, char* argv[]){

/**********************************************************************************************************
PHASE 1A: Server-A boot up, and be ready to listen the request
===The code for initializing, binding socket, and building network address has referenced
====================="http://www.beej.us/guide/bgc/"======================================
***********************************************************************************************************/
	int socket_servera, socket_send; //Variable for udp socket
	int pktLen, num_d=0, num_s=0; //pktLen: used in receiving udp packet / num_d=0, num_s=0: number of found definition, number of found similar words
	struct sockaddr_in aws_addr, send_addr; //network address
	socklen_t sin_size, sd_size;
	vector<string> aws_data, def_data; // to store commands, to store found results.
	map<string, string> myDict; // the dictionary to contain (word, definition)



	//Initialize the server's udp socket-------Beej's
	if((socket_servera=socket(AF_INET, SOCK_DGRAM, 0))<0){
		cout<<"Socket building error detected"<<endl;
		return 0;
	}

	//Initialize the server's network address-----Beej's
    memset(&aws_addr, 0, sizeof(aws_addr));
    aws_addr.sin_family = AF_INET;
    aws_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    aws_addr.sin_port = htons(UDP_SERVER_A);

    sin_size=sizeof(aws_addr);

    //bind the socket and the address-----Beej's
    if((bind(socket_servera,(struct sockaddr*)&aws_addr,sizeof(aws_addr)))<0){
    		cout<<"Socket binding error detected"<<endl;
        return 1;
    }


    // Initialize the aws's network address
    memset(&send_addr, 0, sizeof(send_addr));
    send_addr.sin_family = AF_INET; // force IPv4
    send_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    send_addr.sin_port = htons(UDP_AWS);
    sd_size=sizeof(send_addr);

	/*---------------------------------------------------
	 * Building the dictionary according to "backendA.txt"
	 *---------------------------------------------------*/
    fstream fin;
    fin.open("backendA.txt");
    const char * split = "::"; // used to split the word and definition
    string s;

    // read the txt file line by line
    while(getline(fin, s)){
    		char * strs = new char[s.length() + 1] ;
    		strcpy(strs, s.c_str());
    		char *p=strtok( strs, split);
    		//get key
    		string key=string(p);
    		key=key.substr(0, key.length()-1);
    		//get definition
    		p = strtok(NULL,split);
    		string value=string(p);
    		//insert to the map
    		myDict.insert(pair<string, string>(key, value));
    }
    fin.close();

    //printOut booting up message
    cout<<"The ServerA is up and running using UDP on port <"<<UDP_SERVER_A<<">."<<endl;


/*****************************************************************************************
PHASE 1B: Receive the request from the aws server, and process it
*****************************************************************************************/
    while(1){
    		char* aws_pkt=new char[BUFF_SIZE];

    		//receive the packet from the aws-----Beej's
    		if((pktLen=recvfrom(socket_servera, aws_pkt, BUFF_SIZE, 0, (struct sockaddr*)&aws_addr, &sin_size))==-1){
    			cout<<"Packet receiving error detected"<<endl;
    			return 0;
    		}


    		//read out the commends from the aws
    		split = " ";
    		char *p=strtok( aws_pkt, split);
    		while(p!=NULL){
    			aws_data.push_back(p);
    			p = strtok(NULL,split);
    		}
    		//printOut the commands message
    		cout<<"The ServerA received input <"<<aws_data[1]<<"> and operation <"<<aws_data[0]<<">."<<endl;

    		free(aws_pkt);
    		aws_pkt=NULL;

    	    //Initialize the udp socket for sending
    		if((socket_send=socket(AF_INET, SOCK_DGRAM, 0))<0){
    			cout<<"Socket building error detected"<<endl;
    			return 0;
    		}
		//if the operation is search
    		if(!aws_data[0].compare("search")){

    			// search the data line by line in the dictionary
    			map<string, string>::reverse_iterator iter;
    			for(iter = myDict.rbegin(); iter != myDict.rend(); iter++){
    				//find and add the one distance words into the vector
    				if(isOneEditDistance(aws_data[1], iter->first)){
    					def_data.push_back("#"+iter->first+"#"+iter->second);
    					num_s+=1;
    				}
    				//find and add the explanation into the vector
    				if(!iter->first.compare(aws_data[1])){
    					def_data.push_back(iter->second);
    					num_d+=1;
    				}
    			}

    			//printOut the results message on the screen
    			cout<<"The serverA has found <"<<num_d<<"> match and <"<<num_s<<"> similar words"<<endl;

    			//Send the number of packets that will be send to the aws
        		if(sendto(socket_send, to_string(def_data.size()).c_str(), BUFF_SIZE, 0, (struct sockaddr*)&send_addr, sizeof(send_addr)) == -1){
        			cout<<"Sending udp packet to aws failed"<<endl;
        			return 0;
        		}

        		//send all the results one at a time to the aws
        		for(int i=0; i<def_data.size(); i++){
				usleep(10);
        			pktLen=sendto(socket_send, def_data[i].c_str(), BUFF_SIZE, 0, (struct sockaddr*)&send_addr, sizeof(send_addr));
        		}
    		}

    		//if the operation is prefix
    		else if(!aws_data[0].compare("prefix")){

    			// search the data line by line in the dictionary
    			map<string, string>::reverse_iterator iter;
    			for(iter = myDict.rbegin(); iter != myDict.rend(); iter++){
    				//find and add the words match prefix
    				if(is_prefix(iter->first, aws_data[1])){
    					def_data.push_back(iter->first);
    				}
    			}

    			//printOut the results message on the screen
    			cout<<"The ServerA has found <"<<to_string(def_data.size())<<"> matches"<<endl;

    			//Send the number of packets that will be send to the aws
        		if(sendto(socket_send, to_string(def_data.size()).c_str(), BUFF_SIZE, 0, (struct sockaddr*)&send_addr, sizeof(send_addr)) == -1){
        			cout<<"Sending udp packet to aws failed"<<endl;
        			return 0;
        		}


        		//send all the data packets to the aws
        		for(int i=0; i<def_data.size(); i++){
				usleep(10);
        			pktLen=sendto(socket_send, def_data[i].c_str(), BUFF_SIZE, 0, (struct sockaddr*)&send_addr, sizeof(send_addr));
        		}
    		}
    		/*********************
    		 * PHASE 3: The suffix operation
    		 ********************/
    		else if(!aws_data[0].compare("suffix")){

    			// search the data line by line in the dictionary
    			map<string, string>::reverse_iterator iter;
    			for(iter = myDict.rbegin(); iter != myDict.rend(); iter++){
    				//find and add the words match prefix
    				if(is_suffix(iter->first, aws_data[1])){
    					def_data.push_back(iter->first);
    				}

    			}

    			//printOut the results message on the screen
    			cout<<"The ServerA has found <"<<to_string(def_data.size())<<"> matches"<<endl;

    			//Send the number of packets that will be send to the aws
        		if(sendto(socket_send, to_string(def_data.size()).c_str(), BUFF_SIZE, 0, (struct sockaddr*)&send_addr, sizeof(send_addr)) == -1){
        			cout<<"Sending udp packet to aws failed"<<endl;
        			return 0;
        		}

        		//send all the results to the aws
        		for(int i=0; i<def_data.size(); i++){
        			pktLen=sendto(socket_send, def_data[i].c_str(), BUFF_SIZE, 0, (struct sockaddr*)&send_addr, sizeof(send_addr));
				usleep(10);
        		}
    		}

    		//clean all the containers
    		def_data.clear();
    		aws_data.clear();
    		num_s=0;
    		num_d=0;
    		cout<<"The Server A finished sending the output to AWS"<<endl;
    		close(socket_send);
    }

    close(socket_servera);
    return 0;
}

//To decide whether or not a word is one edit distance from one another
bool isOneEditDistance(string s, string t){
	bool flag=false;
	if(s.size()!=t.size()){
		return false;
	}
	else{
		for(int i=0; i<s.size(); i++){
			if (s[i] != t[i] && !flag) {
				flag=true;
				continue;
			}
			if(s[i] != t[i]){
				return false;
			}
		}
	}

	return flag;

}

//To decide whether or not a word can match a prefix
bool is_prefix(string s, string prefix){
	if(s.size()<prefix.size() || prefix.size()==0){
		return false;
	}
	else{
		for(int i=0; i<prefix.size(); i++){
			if (s[i] != prefix[i]) {
				return false;
			}
		}
	}
	return true;
}

//To decide whether or not a word can match a suffix
bool is_suffix(string s, string suffix){
	if(s.size()<suffix.size() || suffix.size()==0){
		return false;
	}
	else{
		for(int i=suffix.size()-1; i>=0; i--){
			if (s[i+s.size()-suffix.size()] != suffix[i]) {
				return false;
			}
		}
	}
	return true;
}
