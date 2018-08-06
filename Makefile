############################################################
# To compile all cpp files in the project
#
# To run the executable files using make commend
#
# @ Author: Ruohan Sun     
# EE450. Session 1 
############################################################

all:	
	g++ -std=c++11 -o client client.cpp 
	g++ -std=c++11 -o aws aws.cpp 
	g++ -std=c++11 -o monitor monitor.cpp 
	g++ -std=c++11 -o serverA serverA.cpp 
	g++ -std=c++11 -o serverB serverB.cpp 
	g++ -std=c++11 -o serverC serverC.cpp 
	

.PHONY: serverA
serverA: 
	./serverA

.PHONY: serverB
serverB: 
	./serverB
	

.PHONY: serverC	
serverC: 
	./serverC
	
.PHONY: aws
aws:	 
	./aws
	
.PHONY: monitor
monitor:	 
	./monitor

.PHONY: client
client:	
	./client
	