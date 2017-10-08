#include <omnetpp.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <ctgmath>
#include <iostream>
#include <vector>
using namespace omnetpp;

#include "AppMessage_m.h"

#include "MacMessage_m.h"

#include "CSRequestMessage_m.h"
#include "CSResponseMessage_m.h"

#include "TransmissionRequestMessage_m.h"
#include "TransmissionConfirmMessage_m.h"
#include "TransmissionIndicationMessage_m.h"

#include "SignalStartMessage_m.h"
#include "SignalStopMessage_m.h"

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
        double turnaroundTime;
        double csTime;

        TransceiverState_t transceiverState;
        std::vector<SignalStartMessage *> currentTransmissions;

        int nodeXPosition;
        int nodeYPosition;
        int nodeIdentifier;

        void updateCurrentTransmissions(SignalStartMessage *startMsg);
        SignalStartMessage * updateCurrentTransmissions(SignalStopMessage *stopMsg);

        void markAllCollided();
        double getReceivedPowerDBm(SignalStartMessage *startMsg);

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
