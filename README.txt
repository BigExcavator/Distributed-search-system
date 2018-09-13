Name: Ruohan Sun

Summary:
The system includes one client side, three server sides, one management side. After a request coming in, the search job will be done simultaneously by the three server sides and results will be integrated by the management side. The system supports lookup based on prefix & suffix in large scale at quick time.

The funciton include:
	* Find the matching definition of a given word and display the definition on client and monitor screen.
	* Find the one-edit-distance words and display one of them on the monitor screen.
	* Find the matching words for a given prefix and display all of them on client and monitor screen.
	* Find the matching words for a given suffix and display all of them on client and monitor screen.
	* All the message prompt by my project is corresponding to the screen message given by the project guide book.
	* My project can deal with input errors. If the user give illegal commands, the client will terminate
		 and give some message to guide the user to give legal commands.
	* My project will give error message when some error happens, including connection, sending, receiving and so forth. 
	* My project can deal with different order of opening client and monitor. You can open client firstly or monitor 	   firstly.
	
For each file:

	* client.cpp
		- Read the commands given by the user.
		- if the commands are illegal, the client will terminate and give relative message in order to guide the user 			to give legal commands.
		- Create the socket and make the TCP connection with the aws.
		- Send the commands to aws using TCP.
		- Receive the results from aws using TCP.
		- Show the results on the screen.
		- Close the socket when terminate.
	
	* aws.cpp
		- Create a TCP socket to serve client and bind it to it's own network address, then begin listening
		- Create a TCP socket to serve monitor and bind it to it's own network address, then begin listening
		- Create a UDP socket to send message to the backend servers.
		- Begin looping to accept the coming TCP connection.
		- Receive the commands given by the client.
		- Send the commands to the three backend server and collect feed back results from the backend server using 		      UDP.
		- Send the results back to the client and monitor using TCP.
		- Close all the socket when terminate.
	
	* serverA.cpp
		- Create a UDP socket and bind it to it's own network address.
		- Add all the unique pairs of (word, definition) from "backendA.txt" into a map.
		- Begin looping to deal with the coming UDP packets.
		- Search for the definition and one-edit-distance words, or to find the prefix/suffix matches.
		- Send the results back to the aws using UDP.
		- Close the socket when terminate.
		
	* serverB.cpp
		- Almost the same as serverA.
		  It will add all the unique pairs of (word, definition) from "backendB.txt" into a map.
		  
	* serverC.cpp
		- Almost the same as serverA.
		  It will add all the unique pairs of (word, definition) from "backendC.txt" into a map.
		  
	* monitor.cpp
		- Make the TCP connection with the aws.
		- Begin looping to listen the coming message from aws.
		- Display the results on the screen.
		- Close the socket when terminate.
	
	* config.h
		 store all the included header files, ports number, and constants of the project
		
 Information format

	* client to aws
		- using a loop to send each command (eg. first loop send "search", second loop send "Hulk")
	
	* aws to client
		- using a loop to send all results, and the client will store them in a vector
		
	* aws to monitor
	    - using a loop to send all results, and the monitor will store them in a vector
	    
	* aws to backend server
		- send a string containing commands message to the backend server, and the backend server will recover 			information from it
	
	* backend server to aws
		- using a loop to send results to the aws, and the aws will store them in a vector.
		- for the one-edit-distance words and it's definition, it will be sent by a string beginning with "#", and the 		       aws will distinguish it.

  

