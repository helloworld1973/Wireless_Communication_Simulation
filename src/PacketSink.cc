#include <omnetpp.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace omnetpp;

#include "AppMessage_m.h"

class PacketSink : public cSimpleModule
{
public:
    PacketSink();
    ~PacketSink();

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    std::string filename;
    int numOfPackets;
};
Define_Module(PacketSink);


PacketSink::PacketSink()
{
}

PacketSink::~PacketSink()
{  
	 fclose(filePointer);     
}

void PacketSink::initialize()
{
	  filename = par("filename").str();
    numOfPackets = 0;
    FILE * filePointer = fopen(filename, "a");
    if (filePointer != NULL)
    {
        fprintf(filePointer, "Index      ReceivedTimeStamp      GeneratedTimeStamp      SenderID      sequenceNumber      msgSize\n");
        //fclose(filePointer);
    }
}


void PacketSink::handleMessage(cMessage *msg)
{
    if (check_and_cast<AppMessage *>(msg))
    {
    	AppMessage *appMsg = static_cast<AppMessage *>(msg);
    	simtime_t timeStamp = simTime();//current simulation time
    	FILE * filePointer = fopen(filename, "a");
    	fprintf(filePointer, "%d,      %f,      %f,      %d,      %d,      %d\n",
             numOfPackets, timeStamp.dbl() , SIMTIME_DBL(appMsg->timeStamp), appMsg->senderId, appMsg->sequenceNumber, appMsg->msgSize);
     
      numOfPackets++;
      delete msg;
    }
}
