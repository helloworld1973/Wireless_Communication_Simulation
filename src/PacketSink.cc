#include <omnetpp.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <cmath>
#include <ctgmath>

using namespace omnetpp;

class PacketSink : public cSimpleModule
{
public:
    PacketSink();
    ~PacketSink();

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    std::string filename;
};

Define_Module(PacketSink);

PacketSink::PacketSink()
    : cSimpleModule()
{

}

PacketSink::~PacketSink()
{
    //std::string filename = "Data_Received.txt";
    FILE * filePointerToWrite = fopen("Data_Received.txt", "a");
    if (filePointerToWrite == NULL) return;

    int nodeXPosition = getParentModule()->par("nodeXPosition");
    int nodeYPosition = getParentModule()->par("nodeYPosition");

    int nodeIdentifier = getParentModule()->par("nodeIdentifier");

    fprintf(filePointerToWrite, "ReceiverNode #           NumOfMessage Received         Position(X.Y)\n");
    fprintf(filePointerToWrite, "%d,                      %d,                           %d,%d\n",
            nodeIdentifier, numOfPacketsReceived, nodeXPosition, nodeYPosition);

    fclose(filePointerToWrite);

    FILE * MessageLogFilePointer = fopen("Message_PacketSink.txt", "a");

    for(int i = 0; i < numOfPacketsReceived; i++){
        if(numOfPacketsReceived >= circBuffSize) break;
        AppMessage *appMsg = sinkBuffer[i];
	}
    fclose(MessageLogFilePointer);
}

void PacketSink::initialize()
{
    circBuffSize = 10000;
    writeIndex = 0;
    numOfPacketsReceived = 0;
    FILE * MessageLogFilePointer = fopen("Message_PacketSink.txt", "a");
    if (MessageLogFilePointer != NULL)
    {
		fprintf(MessageLogFilePointer, "NumOfMessage Received      msgSize      timeStamp      SenderID      sequenceNumber\n");
        fclose(MessageLogFilePointer);
    }
    filename = par("filename").str();
}

void PacketSink::handleMessage(cMessage *msg)
{
    numOfPacketsReceived++;
    if (dynamic_cast<AppMessage *>(msg))
    {
        AppMessage *appMsg = static_cast<AppMessage *>(msg);
        if (writeIndex < circBuffSize)
		{
            sinkBuffer[writeIndex] = appMsg;
        }
        else
		{
			writeIndex = 0;
		}
        writeIndex++;
        delete msg;
    }

    else
    {
        delete msg;
    }
}
