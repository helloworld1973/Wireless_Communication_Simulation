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
            IDLE = 0,

            RX_DONE,
            RX_TIMEOUT,
            RX_ERROR,

            TX_DONE,
            TX_TIMEOUT,
        } TransceiverState_t;

        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

        int txPowerDBm;
        int bitRate;
        int csThreshDBm;
        int noisePowerDBm;
        int turnaroundTime;
        int csTime;

        TransceiverState_t transceiverState;
        int currentTransmissions;
};

Define_Module(Transceiver);

Transceiver::Transceiver()
{

}

Transceiver::~Transceiver()
{
    // take parameters
    txPowerDBm = par("txPowerDBm");
    bitRate = par("bitRate");
    csThreshDBm = par("csThreshDBm");
    noisePowerDBm = par("noisePowerDBm");
    turnaroundTime = par("turnaroundTime");
    csTime = par("csTime");

    // initialize internal variable
    transceiverState = IDLE;
}

void Transceiver::initialize()
{

}

void Transceiver::handleMessage(cMessage *msg)
{

}
