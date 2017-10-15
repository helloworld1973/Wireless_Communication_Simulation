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
    virtual void handleMessage(cMessage *msg);

    std::string fileName;
    //std::deque<AppMessage *> macBuffer;
    AppMessage *sinkBuffer[10000];
    int writeIndex;
    int circBuffSize;

};
Define_Module(PacketSink);


PacketSink::PacketSink()
{
}

PacketSink::~PacketSink()
{

    //Number of Packets transmitted VS number of Packets received
    //PacketSink fileName doesnt make sense to be
    //used here
    //std::string fileName = "Data_Received.txt";
    FILE * filePointerToWrite = fopen("Data_Received.txt", "a");
    if (filePointerToWrite == NULL) return;

    // retrieve node position in the network (from parents)
    int nodeXPosition = getParentModule()->par("nodeXPosition");
    int nodeYPosition = getParentModule()->par("nodeYPosition");

    // retrieve node identifier from parent
    int nodeIdentifier = getParentModule()->par("nodeIdentifier");


    fprintf(filePointerToWrite, "ReceiverNode #           NumOfMessage Received         Position(X.Y)\n");
    fprintf(filePointerToWrite, "%d,                      %d,                           %d,%d\n",
            nodeIdentifier, numOfPacketsReceived, nodeXPosition, nodeYPosition);

    fclose(filePointerToWrite);

    //Start writing our message into log files
    FILE * MessageLogFilePointer = fopen("Message_PacketSink.txt", "a");

    for(int i = 0; i < numOfPacketsReceived; i++){
        if(numOfPacketsReceived >= circBuffSize) break;
        AppMessage *appMsg = sinkBuffer[i];
        //we can perfect the formatting later...
            //fprintf(MessageLogFilePointer, "%d,                  %d,       %f,        %d,        %d\n",
                   //numOfPacketsReceived, appMsg->msgSize, SIMTIME_DBL(appMsg->timeStamp), appMsg->senderId, appMsg->sequenceNumber);
    }
    fclose(MessageLogFilePointer);


}

void PacketSink::initialize()
{
    //save memory for heavy-loaded testing
    circBuffSize = 10000;
    writeIndex = 0;
    numOfPacketsReceived = 0;
    FILE * MessageLogFilePointer = fopen("Message_PacketSink.txt", "a");
    if (MessageLogFilePointer != NULL)
    {

            fprintf(MessageLogFilePointer, "NumOfMessage Received      msgSize      timeStamp      SenderID      sequenceNumber\n");
            fclose(MessageLogFilePointer);
    }
    // take parameters
    fileName = par("fileName").str();
}

void PacketSink::handleMessage(cMessage *msg)
{


    if (dynamic_cast<AppMessage *>(msg))
    {
        //File pointer CAN NOT be handled here
        AppMessage *appMsg = static_cast<AppMessage *>(msg);

        if (writeIndex < circBuffSize){
            sinkBuffer[writeIndex] = appMsg;
        }
        else{
            //Circulate back to the start
            writeIndex = 0;
            }

        writeIndex++;
        delete msg;
    }

    else
    {
        //It should never reach here
        delete msg;
    }
}
