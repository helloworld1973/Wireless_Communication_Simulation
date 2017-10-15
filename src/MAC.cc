#include <omnetpp.h>
#include <stdio.h>
#include <deque>
#include <string.h>
#include <iostream>
using namespace omnetpp;

#include "AppMessage_m.h"
#include "CSRequestMessage_m.h"
#include "CSResponseMessage_m.h"
#include "TransmissionRequestMessage_m.h"
#include "TransmissionConfirmMessage_m.h"
#include "TransmissionIndicationMessage_m.h"
#include "MacMessage_m.h"

class MAC : public cSimpleModule
{
    public:
        MAC();
        ~MAC();

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

        typedef enum
        {
            IDLE,
            CS_RETRY,
            CS_WAITING,
            TRANSMIT_START,
            TRANSMIT_WAITING,
            TRANSMIT_OVER
        } MACState_t;//treat MAC as a finite state machine as well

        MACState_t MACState;
        int bufferSize;
        int maxBackoffs;
        double backoffDistribution;

        std::deque<AppMessage *> macBuffer;
        int backoffCounter;//local variable
};
Define_Module(MAC);

MAC::MAC()
{
}

MAC::~MAC()
{
    // dump all packets in the queue
    while (macBuffer.size() > 0)
    {
        AppMessage *appMsg = macBuffer.front();
        macBuffer.pop_front();
        delete appMsg;
    }
}

void MAC::initialize()
{
    bufferSize = par("bufferSize");
    maxBackoffs = par("maxBackoffs");
    backoffDistribution = par("backoffDistribution");

    macBuffer.resize(bufferSize);
    macBuffer.clear();//allocate memory for macBuffer

    backoffCounter = 0;
    MACState = IDLE;
}

void MAC::handleMessage(cMessage *msg)
{
    //check the type of the message if the received packet is from Packet Generator
    //if it is nullptr, dynamic_cast raises an OMNeT++ error. Using dynamic_cast saves you from writing error checking code
    if (dynamic_cast<AppMessage *>(msg))
    {
        AppMessage *appMsg = static_cast<AppMessage *>(msg);

        // add packet in the end of macBuffer,  if the buffer is full, then drop the packet
        if (macBuffer.size() == bufferSize)
        {
            delete appMsg;
        }
        else
        {
            macBuffer.push_back(appMsg);
        }
    }


    // received carrier sensing response packet
    else if (dynamic_cast<CSResponseMessage *>(msg))
    {
        CSResponseMessage *csMsg = static_cast<CSResponseMessage *>(msg);

        if (csMsg->getBusyChannel())//the channel is busy
        {
            backoffCounter++;

            if (backoffCounter < maxBackoffs)//still can do carrier sense retry
            {
                // wait for a random time for next carrier sense
                scheduleAt(simTime() + backoffDistribution, new cMessage("CS_Retry"));
                MACState = CS_WAITING;//reset the state
            }
            else//reach the max carrier sense retry times
            {
                AppMessage *appMsg = macBuffer.front();
                macBuffer.pop_front();//drop the first packect in macBuffer
                delete appMsg;// cancel the schedule for current packet transmission
                MACState = IDLE;
            }
        }
        else//the carrier is idle, transmit
        {
            MACState = TRANSMIT_START;
        }
        delete msg;//delete the CSResponseMessage
    }


    // received transmission confirm packet
    else if (dynamic_cast<TransmissionConfirmMessage *>(msg))
    {
        TransmissionConfirmMessage *tcMsg = static_cast<TransmissionConfirmMessage *>(msg);

        // status busy
        if (strcmp(tcMsg->getStatus(), "statusBusy") == 0)//If the two strings are equal, strcmp returns 0.
        {
            MACState = IDLE;//set state to IDLE
        }
        // status ok
        else if (strcmp(tcMsg->getStatus(), "statusOK") == 0)
        {
            MACState = TRANSMIT_OVER;// set the state to transmit done(can extract a packet in the front of macBuffer)
        }
        delete msg;//delete the TransmissionConfirmMessage
    }


    // received TransmissionIndicationMessage
    else if (dynamic_cast<TransmissionIndicationMessage *>(msg))
    {
        TransmissionIndicationMessage *thlMsg = static_cast<TransmissionIndicationMessage *>(msg);

        MacMessage *macMsg = static_cast<MacMessage *>(thlMsg->decapsulate());
        AppMessage *appMsg = static_cast<AppMessage *>(macMsg->decapsulate());

        send(appMsg, "gateForPacket$o");// send it to higher layer

        delete appMsg;
        delete macMsg;
        delete thlMsg;
        delete msg;
    }


    //itself's packets
    else if((dynamic_cast<cMessage *>(msg)))
    {
        //get the packet from itself,simulate as waiting for time for next carrier sensing procedure
        if (strcmp(msg->getName(), "CS_Retry") == 0)
        {
            MACState = CS_RETRY;
        }
        delete msg;
    }


    switch (MACState)//MAC finite state machine(CSMA)
    {
        case IDLE:
        {
            if (!macBuffer.empty())//macBuffer is not empty
            {
                backoffCounter = 0;// reset the backoff counter
                CSRequestMessage *csMsg = new CSRequestMessage;// start the carrier sensing procedure
                send(csMsg, "gateForTX$o");
                MACState = CS_WAITING;
            }
            break;
        }
        case CS_RETRY:
        {
            CSRequestMessage *csMsg = new CSRequestMessage; // start the carrier sensing procedure
            send(csMsg, "gateForTX$o");
            MACState = CS_WAITING;
            break;
        }
        case TRANSMIT_START:
        {
            // CSResponse is received and the channel is clear to transmit
            AppMessage *appMsg = new AppMessage(*macBuffer.front()); //extract the oldest message from buffer
            MacMessage *mmsg = new MacMessage;
            mmsg->encapsulate(appMsg);
            TransmissionRequestMessage *trMsg = new TransmissionRequestMessage;
            trMsg->encapsulate(mmsg);
            send(trMsg, "gateForTX$o");

            delete appMsg;
            delete mmsg;
            delete trMsg;

            MACState = TRANSMIT_WAITING;
            break;
        }
        case TRANSMIT_OVER:
        {
            //AppMessage *appMsg = macBuffer.front();
            macBuffer.pop_front();
            //delete appMsg;
            MACState = IDLE;
            break;
        }
        case CS_WAITING:break;
        case TRANSMIT_WAITING:break;
    }
}
