#include <omnetpp.h>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <iostream>
#include <deque>

using namespace omnetpp;

#include "AppMessage_m.h"
#include "MacMessage_m.h"
#include "CSRequestMessage_m.h"
#include "CSResponseMessage_m.h"
#include "TransmissionRequestMessage_m.h"
#include "TransmissionConfirmMessage_m.h"
#include "TransmissionIndicationMessage_m.h"


Define_Module(MAC);

class MAC : public cSimpleModule
{
public:
    MAC();
    ~MAC();

    int numOfPacketsDroppedOverFlow = 0;
    int numOfPacketsDroppedTimeOut = 0;

protected:
    typedef enum
    {
        IDLE = 0,
        CARRIER_SENSE_RETRY,
        CARRIER_SENSE_WAIT,
        TRANSMIT_START,
        TRANSMIT_WAIT,
        TRANSMIT_DONE
    } MACState_t;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

protected:
    int bufferSize;
    int maxBackoffs;
    double backoffDistribution;

    std::deque<AppMessage *> macBuffer;
    int backoffCounter;
    MACState_t MACState;
};


MAC::MAC()
    : cSimpleModule()
{

}

MAC::~MAC()
{
    while (macBuffer.size() > 0)
    {
        AppMessage *appMsg = macBuffer.front();
        macBuffer.pop_front();
        delete appMsg;
    }


    //std::string filename = "Data_Dropped_MAC.txt";
    FILE * filePointerToWrite = fopen("Data_Dropped_MAC.txt", "a");
    if (filePointerToWrite == NULL) return;


    int nodeXPosition = getParentModule()->par("nodeXPosition");
    int nodeYPosition = getParentModule()->par("nodeYPosition");


    int nodeIdentifier = getParentModule()->par("nodeIdentifier");


    fprintf(filePointerToWrite, "TransceiverNode #           NumOfMessage Dropped-OverFlow          numOfPacketsDroppedTimeOut          Position(X.Y)\n");
    fprintf(filePointerToWrite, "%d,                         %d,                                    %d,                           %d,%d\n",
           nodeIdentifier, numOfPacketsDroppedOverFlow, numOfPacketsDroppedTimeOut, nodeXPosition, nodeYPosition);

    fclose(filePointerToWrite);
}

void MAC::initialize()
{
    numOfPacketsDroppedOverFlow = 0;
    numOfPacketsDroppedTimeOut = 0;

    bufferSize = par("bufferSize");
    maxBackoffs = par("maxBackoffs");
    backoffDistribution = par("backoffDistribution");

    macBuffer.resize(bufferSize);
    macBuffer.clear();

    backoffCounter = 0;

    MACState = IDLE;
}

void MAC::handleMessage(cMessage *msg)
{
    if (dynamic_cast<AppMessage *>(msg))
    {
        AppMessage *appMsg = static_cast<AppMessage *>(msg);

        if (macBuffer.size() == (size_t) bufferSize)
        {
            numOfPacketsDroppedOverFlow++;
            delete appMsg;
        }

        else
        {
            macBuffer.push_back(appMsg);
        }
    }

    else if (dynamic_cast<CSResponseMessage *>(msg))
    {
        CSResponseMessage *csMsg = static_cast<CSResponseMessage *>(msg);

        if (csMsg->getBusyChannel())
        {

            backoffCounter++;

            if (backoffCounter < maxBackoffs)
            {

                scheduleAt(simTime() + backoffDistribution, new cMessage("CSMA_FAILED"));

                MACState = CARRIER_SENSE_WAIT;
            }
            else
            {
                AppMessage *appMsg = macBuffer.front();
                macBuffer.pop_front();
                numOfPacketsDroppedTimeOut++;
                delete appMsg;

                MACState = IDLE;
            }
        }

         else
        {
            MACState = TRANSMIT_START;
        }

        delete msg;
    }


    else if (dynamic_cast<TransmissionConfirmMessage *>(msg))
    {
        TransmissionConfirmMessage *tcMsg = static_cast<TransmissionConfirmMessage *>(msg);

        if (strcmp(tcMsg->getStatus(), "statusBusy") == 0)
        {
            // TODO: check
            MACState = IDLE;
        }

        else if (strcmp(tcMsg->getStatus(), "statusOK") == 0)
        {
            MACState = TRANSMIT_DONE;
        }

        delete msg;
    }

    else if (dynamic_cast<TransmissionIndicationMessage *>(msg))
    {
        TransmissionIndicationMessage *tiMsg = static_cast<TransmissionIndicationMessage *>(msg);

        MacMessage *macMsg = static_cast<MacMessage *>(tiMsg->decapsulate());

        AppMessage *appMsg = static_cast<AppMessage *>(macMsg->decapsulate());

        send(appMsg, "gate1$o");

        delete macMsg;
        delete tiMsg;
    }

    else
    {

        if (strcmp(msg->getName(), "CSMA_FAILED") == 0)
        {
            MACState = CARRIER_SENSE_RETRY;
        }

        delete msg;
    }

    switch (MACState)
    {

        case IDLE:
        {

            if (!macBuffer.empty())
            {

                backoffCounter = 0;

                CSRequestMessage *csMsg = new CSRequestMessage;

                send(csMsg, "gate2$o");

                MACState = CARRIER_SENSE_WAIT;
            }
            break;
        }
        case CARRIER_SENSE_RETRY:
        {

            CSRequestMessage *csMsg = new CSRequestMessage;

            send(csMsg, "gate2$o");

            MACState = CARRIER_SENSE_WAIT;

            break;
        }
        case CARRIER_SENSE_WAIT:
        {

            break;
        }
        case TRANSMIT_START:
        {

            AppMessage *appMsg = new AppMessage(*macBuffer.front());

            MacMessage *mmsg = new MacMessage;
            mmsg->encapsulate(appMsg);

            TransmissionRequestMessage *trMsg = new TransmissionRequestMessage;
            trMsg->encapsulate(mmsg);

            appMsg = nullptr;
            mmsg = nullptr;

            send(trMsg, "gate2$o");

            MACState = TRANSMIT_WAIT;

            break;
        }
        case TRANSMIT_WAIT:
        {
            break;
        }
        case TRANSMIT_DONE:
        {
            AppMessage *appMsg = macBuffer.front();
            macBuffer.pop_front();

            delete appMsg;

            MACState = IDLE;
        }

        default:
        {
            break;
        }
    }
}

