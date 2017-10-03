#include <omnetpp.h>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <iostream>
using namespace omnetpp;

#include "AppMessage_m.h"

class MAC : public cSimpleModule
{
    public:
        MAC();
        ~MAC();

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

    protected:
        int bufferSize;
        int maxBackoffs;
        double backoffDistribution;

        std::vector<AppMessage> macBuffer;
        int backoffCounter;
};

Define_Module(MAC);

MAC::MAC(): cSimpleModule()
{

}

MAC::~MAC()
{

}

void MAC::initialize()
{
    bufferSize = par("bufferSize");
    maxBackoffs = par("maxBackoffs");
    backoffDistribution = par("backoffDistribution");

    macBuffer.resize(bufferSize);
    backoffCounter = 0;
}

void MAC::handleMessage(cMessage *msg)
{

}
