#include <omnetpp.h>

using namespace omnetpp;

class Channel : public cSimpleModule
{
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
};
Define_Module(Channel);

void Channel::initialize()
{

}

void Channel::handleMessage(cMessage *msg)
{

}
