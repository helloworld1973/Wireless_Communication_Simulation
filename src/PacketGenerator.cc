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

    protected:
        double iatDistribution;
        int messageSize;

        int seqno;
        int senderId;

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

void PacketGenerator::initialize()
{
    iatDistribution = par("iatDistribution");
    messageSize = par("messageSize");

    seqno = 0;//local variable in each packet generator

    senderId = getParentModule()->par("nodeIdentifier");//from TransmitterNode's identifier

    scheduleAt(simTime() + iatDistribution, new cMessage("SCHEDULE"));// sends the message to itself for simulating inter-generation time

}

void PacketGenerator::handleMessage(cMessage *msg)
{
    if (dynamic_cast<AppMessage*>(msg))// check the type of the msg is AppMessage or not
    {
        delete msg;// discard AppMessage from MAC
    }
    else
    {
        if (strcmp(msg->getName(), "SCHEDULE") == 0)//from PacketGenerator itself
        {
            AppMessage * appMsg = generateMessage();// create a new AppMessage
            send(appMsg, "gate$o");// send the message
            scheduleAt(simTime() + iatDistribution, new cMessage("SCHEDULE"));// schedule the next transmission
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

    return msg;
}
