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
    int numOfPacketsReceived;

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


    FILE * filePointerToWrite = fopen("SinkReceivedPacketsNum.txt", "a");
    if (filePointerToWrite == NULL) return;

    int nodeXPosition = getParentModule()->par("nodeXPosition");
    int nodeYPosition = getParentModule()->par("nodeYPosition");
    int nodeIdentifier = getParentModule()->par("nodeIdentifier");


    fprintf(filePointerToWrite, "ReceiverNode           NumOfPacketsReceived         Position\n");
    fprintf(filePointerToWrite, "%d,                     %d,                           %d,%d\n",
            nodeIdentifier, numOfPacketsReceived, nodeXPosition, nodeYPosition);

    fclose(filePointerToWrite);

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

    }

}


void PacketSink::handleMessage(cMessage *msg)
{
    if (check_and_cast<AppMessage *>(msg))
    {
        numOfPacketsReceived++;
        AppMessage *appMsg = static_cast<AppMessage *>(msg);
        simtime_t timeStampReceive = simTime();//current simulation time
        simtime_t timeStampSend=appMsg->getTimeStamp();
        int sendId=appMsg->getSenderId();
        int seqNum=appMsg->getSequenceNumber();
        int msgSize=appMsg->getMsgSize();

        fprintf(filePointer,"%d,      %f,      %f,      %d,      %d,      %d\n",
                numOfPackets, timeStampReceive.dbl(), timeStampSend.dbl(), sendId, seqNum, msgSize);

        numOfPackets++;
        delete msg;

    }
}
