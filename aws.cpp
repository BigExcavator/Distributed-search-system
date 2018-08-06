#include "config.h"

using namespace std;

int main (int argc, char* argv[]){

	int sockfd_client, sockfd_monitor, client_in, monitor_in; //variables for tcp socket connection
	int sockfd_udp_aws, sockfd_udp_recv; //variables for udp socket connection
	int isThereSimilarWorda=0, isThereSimilarWordb=0, isThereSimilarWordc=0; // variables for whether or not there is similar words
	socklen_t sin_size, sa_size, sb_size, sc_size, min_size, ser_in_size;
	struct sockaddr_in aws_c_addr, aws_m_addr, aws_udp_addr, servera_addr, serverb_addr, serverc_addr, client_in_addr, monitor_in_addr;// network address
	vector<string> clientData, servera_data, serverb_data, serverc_data, def_data, similar_data, match_data,similar_data_def;// store the results

/*****************************************************************************************
PHASE 1A: Server boot up, and begin to listen up the client's information
===The code for initializing, binding socket, and building network address has referenced
====================="http://www.beej.us/guide/bgc/"======================================
*****************************************************************************************/
	//Initialize the server's tcp socket for client-----Beej's
	if((sockfd_client =  socket(AF_INET, SOCK_STREAM, 0))<0){
		cout<<"Socket building error detected"<<endl;
		return 0;
	}

	//Initialize the server's tcp socket for monitor-----Beej's
	if((sockfd_monitor =  socket(AF_INET, SOCK_STREAM, 0))<0){
		cout<<"Socket building error detected"<<endl;
		return 0;
	}


	//Initialize the aws's network address-----Beej's
	memset(&aws_c_addr, 0, sizeof(aws_c_addr));
	aws_c_addr.sin_family = AF_INET;
	aws_c_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	aws_c_addr.sin_port = htons(TCP_AWS_C);

	memset(&aws_m_addr, 0, sizeof(aws_m_addr));
	aws_m_addr.sin_family = AF_INET;
	aws_m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	aws_m_addr.sin_port = htons(TCP_AWS_M);

    //bind socket to server's network address-----Beej's
 	if (bind(sockfd_client, (struct sockaddr *) &aws_c_addr, sizeof(aws_c_addr)) < 0){
 		cout << "Socket binding error detected for client" << endl;
 		return 0;
 	}

 	if (bind(sockfd_monitor, (struct sockaddr *) &aws_m_addr, sizeof(aws_m_addr)) < 0){
 		cout << "Socket binding error detected for monitor" << endl;
 		return 0;
 	}

 	//begin to listen to client-----Beej's
	if (listen(sockfd_client, 15) < 0){
		cout << "socket listening error detected for client" << endl;
		return 0;
	}

	//begin to listen to monitor-----Beej's
	if (listen(sockfd_monitor, 15) < 0){
		cout << "socket listening error detected for monitor" << endl;
		return 0;
	}


	//Initialize the aws's udp socket for receiving
	if((sockfd_udp_recv=socket(AF_INET, SOCK_DGRAM, 0))<0){
		cout << "socket building error detected" << endl;
		return 0;
	}

	// Initialize the aws's udp address
    memset(&aws_udp_addr, 0, sizeof(aws_udp_addr));
    aws_udp_addr.sin_family = AF_INET;
    aws_udp_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    aws_udp_addr.sin_port = htons(UDP_AWS);
    ser_in_size=sizeof(aws_udp_addr);

    //bind the aws' udp socket and the address-----Beej's
    if((bind(sockfd_udp_recv,(struct sockaddr*)&aws_udp_addr,sizeof(aws_udp_addr)))<0){
    		cout<<"Socket binding error detected 11"<<endl;
        return 1;
    }


    // Initialize the servera's network address
    memset(&servera_addr, 0, sizeof(servera_addr));
    servera_addr.sin_family = AF_INET; // force IPv4
    servera_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servera_addr.sin_port = htons(UDP_SERVER_A);
    sa_size=sizeof(servera_addr);

    // Initialize the serverb's network address
    memset(&serverb_addr, 0, sizeof(serverb_addr));
    serverb_addr.sin_family = AF_INET; // force IPv4
    serverb_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverb_addr.sin_port = htons(UDP_SERVER_B);
    sb_size=sizeof(serverb_addr);

    // Initialize the serverc's network address
    memset(&serverc_addr, 0, sizeof(serverc_addr));
    serverc_addr.sin_family = AF_INET; // force IPv4
    serverc_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverc_addr.sin_port = htons(UDP_SERVER_C);
    sc_size=sizeof(serverc_addr);

    //printOut the booting up message
	cout<<"The AWS is up and running."<<endl;

/*****************************************************************************************
PHASE 1B: Receive the request from the client, and send request to the three backend server
*****************************************************************************************/

	sin_size=sizeof(client_in_addr);
	min_size=sizeof(monitor_in_addr);
	const char* split="#";
	int monitor_setup=0; //flag for whether or not the monitor is connected

	while(1){

		client_in=0;
		int def_len;

		//accept the client's connection request
		if((client_in=accept(sockfd_client, (struct sockaddr *)&client_in_addr, &sin_size))<0){
			cout << " Socket accepting error detected for client" << endl;
			continue;
		}

		//accept the monitor's connection request
		if(monitor_setup==0){
			if((monitor_in=accept(sockfd_monitor, (struct sockaddr *)&monitor_in_addr, &min_size))<0){
				cout << " Socket accepting error detected for monitor" << endl;
				continue;
			}
			monitor_setup=1; //After the first connection, the monitor will always on, and the aws will not keeping preparing to listen the monitor
		}

		//receive the commend from the client
		for(int i=0; i<2; i++){
			char* in_data=new char[BUFF_SIZE];
			recv(client_in, in_data, BUFF_SIZE, 0);
			clientData.push_back(string(in_data));
			free(in_data);
			in_data=NULL;
		}

		//printOut the command messages
		cout << "The AWS received input=<"<<clientData[1]<<"> and function=<"<<clientData[0]<<"> from the client using TCP over port <";
		cout<<TCP_AWS_C<<">"<<endl;

		string backend=clientData[0]+" "+clientData[1]; // store the command in a stirng to send to the backend servers

		//Initialize the aws's udp socket for sending
		if((sockfd_udp_aws=socket(AF_INET, SOCK_DGRAM, 0))<0){
			cout << "socket building error detected" << endl;
			return 0;
		}
		/*****************************
		 * Send request to backend A and get results
		 ******************************/
		if(sendto(sockfd_udp_aws, backend.c_str(), BUFF_SIZE, 0,(struct sockaddr*)&servera_addr,sizeof(servera_addr)) == -1){
			cout<<"Sending udp packet to servera failed"<<endl;
			return 0;
		}
		//printOut send to A message
		cout<<"The AWS sent <"<<clientData[1]<<"> and <"<<clientData[0]<<"> to Backend-Server A"<<endl;


		//get how many packets will be send by the backend server
		char* a_data=new char[BUFF_SIZE];
		if(recvfrom(sockfd_udp_recv, a_data, BUFF_SIZE, 0, (struct sockaddr*)&aws_udp_addr, &ser_in_size)<0){
			cout<<"Receiving udp packet to servera failed"<<endl;
			return 0;
		}
		def_len=atoi(a_data);
		free(a_data);
		a_data=NULL;

		//receive packets from A
		for(int i=0; i<def_len; i++){
			char* a_data=new char[BUFF_SIZE];
			recvfrom(sockfd_udp_recv, a_data, BUFF_SIZE, 0, (struct sockaddr*)&aws_udp_addr, &ser_in_size);
			servera_data.push_back(string(a_data));
			free(a_data);
			a_data=NULL;
		}

		//if the operation is search
		if(!clientData[0].compare("search")){

			//to divide the definition of the given word and the similar words apart
			for(int i=0; i<servera_data.size(); i++){
				if(servera_data[i][0]=='#'){
					isThereSimilarWorda=1;
					string word_def=servera_data[i].substr(1, servera_data[i].length());
					char * strs = new char[word_def.length() + 1] ;
					strcpy(strs, word_def.c_str());
					char* p=strtok( strs, split);
					similar_data.push_back(string(p));
					p = strtok(NULL,split);
					similar_data_def.push_back(string(p));
					free(strs);
					strs=NULL;
				}
				else{
					def_data.push_back(servera_data[i]);
				}
			}
		}
		//if the operation is prefix/suffix, put the matched in another vector
		else{
			for(int i=0; i<servera_data.size(); i++){
				match_data.push_back(servera_data[i]);
			}
		}

		/*****************************
		 * Send request to backend B and get results
		 ******************************/
		if(sendto(sockfd_udp_aws, backend.c_str(), BUFF_SIZE, 0,(struct sockaddr*)&serverb_addr,sizeof(serverb_addr)) == -1){
			cout<<"Sending udp packet to serverb failed"<<endl;
			return 0;
		}

		//printOut send to B message
		cout<<"The AWS sent <"<<clientData[1]<<"> and <"<<clientData[0]<<"> to Backend-Server B"<<endl;


		//get how many packets will be send by the backend server
		char* b_data=new char[BUFF_SIZE];
		if(recvfrom(sockfd_udp_recv, b_data, BUFF_SIZE, 0, (struct sockaddr*)&aws_udp_addr, &ser_in_size)<0){
			cout<<"Receiving udp packet to serverb failed"<<endl;
			return 0;
		}
		def_len=atoi(b_data);
		free(b_data);
		b_data=NULL;

		//receive packets from B
		for(int i=0; i<def_len; i++){
			char* b_data=new char[BUFF_SIZE];
			recvfrom(sockfd_udp_recv, b_data, BUFF_SIZE, 0, (struct sockaddr*)&aws_udp_addr, &ser_in_size);
			serverb_data.push_back(string(b_data));
			free(b_data);
			b_data=NULL;
		}

		//if the operation is search
		if(!clientData[0].compare("search")){
			//divide the definition of the given word and similar words apart
			for(int i=0; i<serverb_data.size(); i++){
				if(serverb_data[i][0]=='#'){
					isThereSimilarWordb=1;
					string word_def=serverb_data[i].substr(1, serverb_data[i].length());
					char * strs = new char[word_def.length() + 1] ;
					strcpy(strs, word_def.c_str());
					char* p=strtok( strs, split);
					similar_data.push_back(string(p));
					p = strtok(NULL,split);
					similar_data_def.push_back(string(p));
					free(strs);
					strs=NULL;
				}
				else{
					def_data.push_back(serverb_data[i]);
				}
			}
		}
		// if the operation is prefix/suffix, put the results into another vector
		else{
			for(int i=0; i<serverb_data.size(); i++){
				match_data.push_back(serverb_data[i]);
			}
		}

		/*****************************
		 * Send request to backend C and get results
		 ******************************/
		if(sendto(sockfd_udp_aws, backend.c_str(), BUFF_SIZE, 0,(struct sockaddr*)&serverc_addr,sizeof(serverc_addr)) == -1){
			cout<<"Sending udp packet to serverc failed"<<endl;
			return 0;
		}

		//printOut send to B message
		cout<<"The AWS sent <"<<clientData[1]<<"> and <"<<clientData[0]<<"> to Backend-Server C"<<endl;


		//get how many packets will be send by the backend server
		char* c_data=new char[BUFF_SIZE];
		if(recvfrom(sockfd_udp_recv, c_data, BUFF_SIZE, 0, (struct sockaddr*)&aws_udp_addr, &ser_in_size)<0){
			cout<<"Receiving udp packet to serverc failed"<<endl;
			return 0;
		}
		def_len=atoi(c_data);
		free(c_data);
		c_data=NULL;

		//receive packets from C
		for(int i=0; i<def_len; i++){
			char* c_data=new char[BUFF_SIZE];
			recvfrom(sockfd_udp_recv, c_data, BUFF_SIZE, 0, (struct sockaddr*)&aws_udp_addr, &ser_in_size);
			serverc_data.push_back(string(c_data));
			free(c_data);
			c_data=NULL;
		}

		//if the operation is search
		if(!clientData[0].compare("search")){
			//divide the definition of the given word and similar words apart
			for(int i=0; i<serverc_data.size(); i++){
				if(serverc_data[i][0]=='#'){
					isThereSimilarWordc=1;
					string word_def=serverc_data[i].substr(1, serverc_data[i].length());
					char * strs = new char[word_def.length() + 1] ;
					strcpy(strs, word_def.c_str());
					char* p=strtok( strs, split);
					similar_data.push_back(string(p));
					p = strtok(NULL,split);
					similar_data_def.push_back(string(p));
					free(strs);
					strs=NULL;

				}
				else{
					def_data.push_back(serverc_data[i]);
				}
			}
		}
		// if the operation is prefix/suffix, put the results into another vector
		else{
			for(int i=0; i<serverc_data.size(); i++){
				match_data.push_back(serverc_data[i]);
			}
		}

		//printOut all the receiving messages
		if(!clientData[0].compare("search")){
			cout<<"The AWS received <"<<isThereSimilarWorda<<"> similar words from Backend-Server <A> using UDP over port <"<<UDP_AWS<<">"<<endl;
			cout<<"The AWS received <"<<isThereSimilarWordb<<"> similar words from Backend-Server <B> using UDP over port <"<<UDP_AWS<<">"<<endl;
			cout<<"The AWS received <"<<isThereSimilarWordc<<"> similar words from Backend-Server <C> using UDP over port <"<<UDP_AWS<<">"<<endl;
		}
		else{
			cout<<"The AWS received <"<<servera_data.size()<<"> matches from Backend-Server <A> using UDP over port <";
			cout<<UDP_AWS<<">"<<endl;
			cout<<"The AWS received <"<<serverb_data.size()<<"> matches from Backend-Server <B> using UDP over port <";
			cout<<UDP_AWS<<">"<<endl;
			cout<<"The AWS received <"<<serverc_data.size()<<"> matches from Backend-Server <C> using UDP over port <";
			cout<<UDP_AWS<<">"<<endl;
		}

/*****************************************************************************************
PHASE 2: Send all the collected results back to client and monitor
*****************************************************************************************/

		/*******************************
		* Send the results to the client
		********************************/

		if(!clientData[0].compare("search")){
			//send the number of definition to the client
			send(client_in, std::to_string(def_data.size()).c_str(), BUFF_SIZE, 0);

			//if there is a defination, then send it
			if(def_data.size()!=0){
				send(client_in, def_data[0].c_str(), BUFF_SIZE, 0);
				cout<<"The AWS sent <"<<"1"<<"> matches to client."<<endl;
			}
			else{
				cout<<"The AWS sent <"<<"0"<<"> matches to client."<<endl;
			}
		}
		else{

			//send the number of matches to the client
			send(client_in, to_string(match_data.size()).c_str(), BUFF_SIZE, 0);

			//if there are matches, then send it
			if(match_data.size()!=0){
				for(int i=0; i<match_data.size(); i++){
					send(client_in, match_data[i].c_str(), BUFF_SIZE, 0);
				}
				cout<<"The AWS sent <"<<match_data.size()<<"> matches to client."<<endl;
			}
			else{
				cout<<"The AWS sent <"<<match_data.size()<<"> matches to client."<<endl;
			}
		}


		/*******************************
		* Send the results to the monitor
		********************************/

		//Send the type of operation to the monitor
		send(monitor_in, clientData[0].c_str(), BUFF_SIZE, 0);

		if(!clientData[0].compare("search")){
			//send the input word into monitor
			send(monitor_in, clientData[1].c_str(), BUFF_SIZE, 0);
			//send the number of definition to the monitor
			send(monitor_in, to_string(def_data.size()).c_str(), BUFF_SIZE, 0);
			//if there is a definition, then send it to the monitor
			if(def_data.size()!=0){
				send(monitor_in, def_data[0].c_str(), BUFF_SIZE, 0);
			}

			//send the number of similar words to the monitor
			send(monitor_in, to_string(similar_data.size()).c_str(), BUFF_SIZE, 0);
			//send the similar word and definition to the monitor
			if(similar_data.size()!=0){
					send(monitor_in, similar_data[0].c_str(), BUFF_SIZE, 0);
					send(monitor_in, similar_data_def[0].c_str(), BUFF_SIZE, 0);
			}

			//printOut sending information according to the true situation
			if(def_data.size()==0 && similar_data.size()!=0){
				cout<<"The AWS sent <"<<similar_data[0]<<"> to the monitor via TCP port <"<<TCP_AWS_M<<">"<<endl;
			}
			else if(def_data.size()!=0 && similar_data.size()==0){
				cout<<"The AWS sent <"<<clientData[1]<<"> to the monitor via TCP port <"<<TCP_AWS_M<<">"<<endl;
			}
			else if(def_data.size()!=0 && similar_data.size()!=0){
				cout<<"The AWS sent <"<<clientData[1]<<"> and <"<<similar_data[0]<<"> to the monitor via TCP port <"<<TCP_AWS_M<<">"<<endl;
			}
			else{
				cout<<"The AWS sent no word to the monitor via TCP port <"<<TCP_AWS_M<<">"<<endl;
			}

		}
		else{
			//send the input word into monitor
			send(monitor_in, clientData[1].c_str(), BUFF_SIZE, 0);

			//send the number of matches to the monitor
			send(monitor_in, to_string(match_data.size()).c_str(), BUFF_SIZE, 0);

			if(match_data.size()!=0){
				for(int i=0; i<match_data.size(); i++){
					send(monitor_in, match_data[i].c_str(), BUFF_SIZE, 0);
				}
				cout<<"The AWS sent <"<<match_data.size()<<"> matches to the monitor via TCP port <"<<TCP_AWS_M<<">"<<endl;
			}
			else{
				cout<<"The AWS sent <"<<match_data.size()<<"> matches to the monitor via TCP port <"<<TCP_AWS_M<<">"<<endl;
			}

		}

		//Clean the data inside the containers
		clientData.clear();
		servera_data.clear();
		serverb_data.clear();
		serverc_data.clear();
		def_data.clear();
		similar_data.clear();
		match_data.clear();
		similar_data_def.clear();
		isThereSimilarWorda=0;
		isThereSimilarWordb=0;
		isThereSimilarWordc=0;

		//close the child socket for the client
		close(client_in);
		close(sockfd_udp_aws);
	}

	//close all the existed sockets
	close(monitor_in);
	close(sockfd_client);
	close(sockfd_monitor);
	close(sockfd_udp_recv);
	return 0;

}
