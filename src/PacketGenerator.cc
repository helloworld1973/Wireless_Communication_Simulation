#include <omnetpp.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace omnetpp;

#include "AppMessage_m.h"

class PacketGenerator : public cSimpleModule
{
    public:
        PacketGenerator();
        ~PacketGenerator();
        int numOfPacketsGenerated;

    protected:
        double iatDistribution;
        int messageSize;

        int seqno;
        int senderId;

        virtual void finish();
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        AppMessage *generateMessage();
};
Define_Module(PacketGenerator);

PacketGenerator::PacketGenerator()
{
}

PacketGenerator::~PacketGenerator()
{
}


void PacketGenerator::finish()
{
    //#exp2
    FILE * filePointerToWrite = fopen("PacketGeneratorDataNum.txt", "a");
    if (filePointerToWrite == NULL) return;
    int nodeXPosition = getParentModule()->par("nodeXPosition");
    int nodeYPosition = getParentModule()->par("nodeYPosition");
    int nodeIdentifier = getParentModule()->par("nodeIdentifier");

    fprintf(filePointerToWrite, "TransceiverNode          NumOfPacketGeneratorPackets        Position\n");
    fprintf(filePointerToWrite, "%d,                       %d,                               %d,%d\n",
            nodeIdentifier, numOfPacketsGenerated, nodeXPosition, nodeYPosition);
    fclose(filePointerToWrite);

}


void PacketGenerator::initialize()
{
    numOfPacketsGenerated = 0;


    iatDistribution = par("iatDistribution");
    messageSize = par("messageSize");

    seqno = 0;//local variable in each packet generator

    senderId = getParentModule()->par("nodeIdentifier");//from TransmitterNode's identifier

    scheduleAt(simTime() + iatDistribution, new cMessage("START"));// sends the message to itself for simulating inter-generation time

}

void PacketGenerator::handleMessage(cMessage *msg)
{
    if (dynamic_cast<AppMessage*>(msg))// check the type of the msg is AppMessage or not
    {
        delete msg;// discard AppMessage from MAC
    }
    else
    {
        if (strcmp(msg->getName(), "START") == 0)//from PacketGenerator itself
        {
            AppMessage * appMsg = generateMessage();// create a new AppMessage
            send(appMsg, "gate$o");// send the message
            scheduleAt(simTime() + iatDistribution, new cMessage("START"));// schedule the next transmission
        }
        delete msg;
    }
}

AppMessage *PacketGenerator::generateMessage()
{
    simtime_t timeStamp = simTime();//current simulation time

    int sequenceNumber = seqno++;//sequence id++

    int msgSize = messageSize;

    char msgname[40];// generate message name include info
    snprintf(msgname, 40, "sender=%d seq=%d ts=%f", senderId, sequenceNumber, timeStamp.dbl());

    AppMessage * msg = new AppMessage(msgname);

    msg->setTimeStamp(timeStamp);
    msg->setSenderId(senderId);
    msg->setSequenceNumber(sequenceNumber);
    msg->setMsgSize(msgSize);

    numOfPacketsGenerated++;

    return msg;
}
