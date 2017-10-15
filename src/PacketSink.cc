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
    virtual void finish();
    virtual void handleMessage(cMessage *msg);

    std::string fileName;
    char* fileNameChar;
    FILE * filePointer;
    int numOfPackets;

};
Define_Module(PacketSink);


PacketSink::PacketSink()
{
}

PacketSink::~PacketSink()
{  

}
void PacketSink::finish()
{
    fclose(filePointer);
}

void PacketSink::initialize()
{
	fileName=par("fileName").stdstringValue();
	fileNameChar=(char*)fileName.data();
    numOfPackets = 0;
    filePointer= fopen(fileNameChar, "a");
    if (filePointer != NULL)
    {
        fprintf(filePointer, "Index      ReceivedTimeStamp      GeneratedTimeStamp      SenderID      sequenceNumber      msgSize\n");
        //fclose(filePointer);
    }
    //fclose(filePointer);
}


void PacketSink::handleMessage(cMessage *msg)
{
    if (check_and_cast<AppMessage *>(msg))
    {
    	AppMessage *appMsg = static_cast<AppMessage *>(msg);
    	simtime_t timeStampReceive = simTime();//current simulation time
    	simtime_t timeStampSend=appMsg->getTimeStamp();
        int sendId=appMsg->getSenderId();
        int seqNum=appMsg->getSequenceNumber();
        int msgSize=appMsg->getMsgSize();

        //FILE * filePointer= fopen(fileNameChar, "a");
    	fprintf(filePointer,"%d,      %f,      %f,      %d,      %d,      %d\n",
                      numOfPackets, timeStampReceive.dbl(), timeStampSend.dbl(), sendId, seqNum, msgSize);

        numOfPackets++;
        delete msg;
        //fclose(filePointer);
    }
}
