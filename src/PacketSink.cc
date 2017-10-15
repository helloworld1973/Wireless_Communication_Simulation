#include <omnetpp.h>
#include <string>
#include <iostream>
#include <cmath>
#include <ctgmath>
#include <stdio.h>
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

    std::string fileName;
    int numOfPackets;
    FILE * filePointer;
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
	fileName=par("fileName").str();
	char* fileNameChar=(char*)fileName.data();
    numOfPackets = 0;
    filePointer = fopen(fileNameChar, "a");
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
    	fprintf(filePointer, "%d,      %f,      %f,      %d,      %d,      %d\n",
             numOfPackets, timeStamp.dbl() ,
             appMsg->getTimeStamp(),
             appMsg->getSenderId(),
             appMsg->getSequenceNumber(),
             appMsg->getMsgSize());
     
      numOfPackets++;
      delete msg;
    }
}
