#include <iostream>
#include <omnetpp.h>
#include "SignalStartMessage_m.h"
#include "SignalStopMessage_m.h"

using namespace omnetpp;


class Channel : public cSimpleModule
{
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

    protected:
        int numGates;
};
Define_Module(Channel);


void Channel::initialize()
{
    numGates = gateCount() / 2;
}

void Channel::handleMessage(cMessage *msg)
{
    if (dynamic_cast<SignalStartMessage *>(msg))
    {
        SignalStartMessage * startMsg = static_cast<SignalStartMessage *>(msg);
        for (int i = 0; i < numGates; i++)
        {
            SignalStartMessage * newMsg = new SignalStartMessage(*startMsg);
            send(newMsg, "gateNet$o", i);
        }
        delete msg;
    }

    else if (dynamic_cast<SignalStopMessage *>(msg))
    {
        SignalStopMessage * stopMsg = static_cast<SignalStopMessage *>(msg);
        for (int i = 0; i < numGates; i++)
        {
            SignalStopMessage * newMsg = new SignalStopMessage(*stopMsg);
            send(newMsg, "gateNet$o", i);
        }
        delete msg;
    }

    else
    {
        delete msg;
    }

}
