#include <omnetpp.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace omnetpp;

class Transceiver : public cSimpleModule
{
    public:
        Transceiver();
        ~Transceiver();

    protected:
        typedef enum
        {
            RX,
            TX
        } TransceiverState_t;//finite state machine

        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

        int txPowerDBm;
        int bitRate;
        int csThreshDBm;
        int noisePowerDBm;
        int turnaroundTime;
        int csTime;

        TransceiverState_t transceiverState;
        std::vector<SignalStartMessage *> currentTransmissions;
};

Define_Module(Transceiver);

Transceiver::Transceiver()
{

}

Transceiver::~Transceiver()
{

}

void Transceiver::initialize()
{
    txPowerDBm = par("txPowerDBm");
    bitRate = par("bitRate");
    csThreshDBm = par("csThreshDBm");
    noisePowerDBm = par("noisePowerDBm");
    turnaroundTime = par("turnaroundTime");
    csTime = par("csTime");


    transceiverState = RX;

}

void Transceiver::handleMessage(cMessage *msg)
{

}
