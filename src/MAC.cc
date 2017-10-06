#include <omnetpp.h>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <iostream>
using namespace omnetpp;

#include "AppMessage_m.h"
#include "CSRequestMessage_m.h"
#include "CSResponseMessage_m.h"
#include "TransmissionRequestMessage_m.h"
#include "TransmissionConfirmMessage_m.h"
#include "TransmissionHigherLayerMessage_m.h"
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
        IDLE = 0,
        CARRIER_SENSE_RETRY,
        CARRIER_SENSE_WAIT,
        TRANSMIT_START,
        TRANSMIT_WAIT,
        TRANSMIT_DONE
    } MACState_t;//treat MAC as a finite state machine as well

    MACState_t MACState;
    int bufferSize;
    int maxBackoffs;
    double backoffDistribution;

    std::vector<AppMessage> macBuffer;
    int backoffCounter;//local variable
};
Define_Module(MAC);

MAC::MAC()
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
    macBuffer.clear();//allocate memory for macBuffer

    backoffCounter = 0;
}

void MAC::handleMessage(cMessage *msg)
{
    //check the type of the message if the received packet is from Packet Generator
    //if it is nullptr, check_and_cast raises an OMNeT++ error. Using check_and_cast saves you from writing error checking code
    if (dynamic_cast<AppMessage *>(msg))
    {
        AppMessage *appMsg = msg;

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
        CSResponseMessage *csMsg = msg;

        if (csMsg->getBusyChannel())//the channel is busy
        {
            // increase the backoffCounter
            backoffCounter++;

            // std::cout << "Retry:" << backoffCounter << std::endl;

            // test if the counter has reached the maximum value
            if (backoffCounter < maxBackoffs)
            {
                // wait for a random time
                // schedule next event
                // send a dummy packet to itself
                scheduleAt(simTime() + backoffDistribution, new cMessage("CSMA_FAILED"));

                // wait for response
                MACState = CARRIER_SENSE_WAIT;
            }
            else
            {
                // cancel the schedule for current packet transmission
                AppMessage *appMsg = macBuffer.front();
                macBuffer.pop_front();
                delete appMsg;

                // cancel the current transmission
                MACState = IDLE;
            }
        }
        else//the carrier is idle,so transmit immediately
        {
            MACState = TRANSMIT_START;
        }
        delete msg;//delete the CSResponseMessage
    }


    // received transmission confirm packet
    else if (dynamic_cast<TransmissionConfirmMessage *>(msg))
    {
        TransmissionConfirmMessage *tcMsg = msg;

        // status busy
        if (strcmp(tcMsg->getStatus(), "statusBusy") == 0)//If the two strings are equal, strcmp returns 0.
        {
            MACState = IDLE;//set state to IDLE
        }
        // status ok
        else if (strcmp(tcMsg->getStatus(), "statusOK") == 0)
        {
            MACState = TRANSMIT_DONE;// set the state to transmit done(can extract a packet in the front of macBuffer)
        }
        delete msg;//delete the TransmissionConfirmMessage
    }


    // received transmission Higher Layer message
    else if (dynamic_cast<TransmissionHigherLayerMessage *>(msg))
    {
        TransmissionHigherLayerMessage *thlMsg = msg;

        MacMessage *macMsg = thlMsg->decapsulate();
        AppMessage *appMsg = macMsg->decapsulate();

        send(appMsg, "gate1$o");// send it to higher layer

        delete macMsg;
        delete thlMsg;
    }


    //itself's packets
    else
    {
        //get the packet from itself,simulate as waiting for time for next carrier sensing procedure
        if (strcmp(msg->getName(), "CSMA_FAILED") == 0)
        {
            MACState = CARRIER_SENSE_RETRY;
        }
        delete msg;
    }

    //MAC finite state machine(CSMA)
    switch (MACState)
    {
        // the MAC module will stay in IDLE state if there is no packet in the macBuffer
        case IDLE:
        {
            // if there are packets in the macBuffer, then starts the MAC process.
            if (!macBuffer.empty())
            {
                // reset the backoff counter
                backoffCounter = 0;

                // start the carrier sensing procedure
                // send a message of type CSRequest to the Transceiver
                CSRequestMessage *csMsg = new CSRequestMessage;

                send(csMsg, "gate2$o");

                // advance to next state
                MACState = CARRIER_SENSE_WAIT;
            }
            break;
        }
        case CARRIER_SENSE_RETRY:
        {
            // start the carrier sensing procedure
            // send a message of type CSRequest to the Transceiver
            CSRequestMessage *csMsg = new CSRequestMessage;

            send(csMsg, "gate2$o");

            // advance to next state
            MACState = CARRIER_SENSE_WAIT;

            break;
        }
        case CARRIER_SENSE_WAIT:
        {
            // the process will trap here until CSResponse is received
            break;
        }
        case TRANSMIT_START:
        {
            // CSResponse is received and the channel is clear to transmit
            // we already know there will be packet in the queue

            // extract the oldest message from buffer (create a deep copy)
            AppMessage *appMsg = new AppMessage(*macBuffer.front());

            // encapsulate it into a message mmsg of type MacMessage
            MacMessage *mmsg = new MacMessage;
            mmsg->encapsulate(appMsg);

            // encapsulate again into TransmissionRequest packet
            TransmissionRequestMessage *trMsg = new TransmissionRequestMessage;
            trMsg->encapsulate(mmsg);

            // nullify the pointers
            appMsg = nullptr;
            mmsg = nullptr;

            // transmit the MacMessage
            send(trMsg, "gate2$o");

            // wait for transmission confirm
            MACState = TRANSMIT_WAIT;

            break;
        }
        case TRANSMIT_WAIT:
        {
            // the process will trap here until TransmissionConfirm is received
            break;
        }
        case TRANSMIT_DONE:
        {
            AppMessage *appMsg = macBuffer.front();
            macBuffer.pop_front();

            delete appMsg;

            // advance to IDLE state
            MACState = IDLE;
        }

        default:
        {
            break;
        }
    }
}
