#include <omnetpp.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <ctgmath>
#include <iostream>
#include <vector>
#include <cmath>
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

#define pathLossExponent 4.0f

class Transceiver : public cSimpleModule
{
public:
    Transceiver();
    ~Transceiver();

protected:
    typedef enum
    {
        RXState,
        TXState
    } TransceiverState_t;//finite state machine

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    int txPowerDBm;
    int bitRate;
    int csThreshDBm;
    int noisePowerDBm;
    double turnaroundTime;
    double csTime;

    double total_power_db;//use to store channel's signal power

    TransceiverState_t transceiverState;
    std::vector<SignalStartMessage *> currentTransmissions;

    int nodeXPosition;
    int nodeYPosition;
    int nodeIdentifier;

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

    transceiverState = RXState;

    total_power_db=0.0;

    nodeXPosition = getParentModule()->par("nodeXPosition");
    nodeYPosition = getParentModule()->par("nodeYPosition");
    nodeIdentifier = getParentModule()->par("nodeIdentifier");
}

void Transceiver::handleMessage(cMessage *msg)
{
    //Carrier Sense,step 1,go on
    if (check_and_cast<CSRequestMessage *>(msg))
    {
        delete msg;

        // traverse and calculate received power for all ongoing transmissions
        double total_power_ratio = 0.0;
        for (auto it = currentTransmissions.begin(); it != currentTransmissions.end(); it++)
        {

            double power_db = getReceivedPowerDBm(*it);// calculate the received power in dbm
            double power_ratio = pow(10, power_db/10);
            total_power_ratio += power_ratio;
        }
        total_power_db = 10 * log10(total_power_ratio);// conver from normal domain to dB domain

        cMessage *ciMsg=new cMessage("CARRIER_SENSE_WAIT");
        scheduleAt(simTime() + csTime, ciMsg);// wait for csTime
        return;
    }


    // Receive Path(SignalStart from the Channel)
    if (check_and_cast<SignalStartMessage *>(msg))
    {
        SignalStartMessage *startMsg = static_cast<SignalStartMessage *>(msg);
        if (!currentTransmissions.empty())//currentTransmission[] not empty
        {
            for (auto it = currentTransmissions.begin(); it != currentTransmissions.end(); it++)
            {
                if ((*it)->getIdentifier() == startMsg->getIdentifier())
                {
                    std::cout << "update SignalStartMessage to the CurrentTransmissions[] error !!!" << std::endl;
                    delete msg; delete startMsg;
                    return;
                }
            }//check whether the extracted identifier equals to the identifier which in currentTransmission[]
            for (auto it = currentTransmissions.begin(); it != currentTransmissions.end(); it++)
            {
                (*it)->setCollidedFlag(true);
            }//collision happened, set all the currentTransmission[]'s CollidedFlag TRUE
            startMsg->setCollidedFlag(true);
            currentTransmissions.push_back(new SignalStartMessage(*startMsg));//add SignalStartMessage to the currentTransmission[]
        }else//currentTransmission[] empty
        {
            currentTransmissions.push_back(new SignalStartMessage(*startMsg));
        }
        delete msg;
        return;
    }

    // Receive Path(SignalStop from the Channel)
    if (check_and_cast<SignalStopMessage *>(msg))
    {
        SignalStopMessage *stopMsg = static_cast<SignalStopMessage *>(msg);
        SignalStartMessage *startMsg;
        int flagTemp=0;
        for (auto it = currentTransmissions.begin(); it != currentTransmissions.end(); it++)
            {
                if ((*it)->getIdentifier() == stopMsg->getIdentifier())//find the corresponding SignalStartMessage with same identifier
                {
                    SignalStartMessage *startMsgTemp = *it;// get the SignalStartMessage
                    startMsg = startMsgTemp;
                    currentTransmissions.erase(it);// remove the SignalStartMessage from list
                    flagTemp++;
                }
            }
        if(flagTemp==0)
            {
                delete stopMsg;delete startMsg;
                std::cout << "update SignalStopMessage to the CurrentTransmissions[] error !!!" << std::endl;
                return;
            }
        else if(flagTemp==1)
            {
                delete stopMsg;
                if (startMsg->getCollidedFlag())//the collided flag is true
                {
                    delete startMsg;
                    return;//no further action
                }
                else
                {





                    delete startMsg;
                    return;
                }
            }
    }



    switch (transceiverState)//finite State Machine(related to carrier sense and Transmit Path)
    {
    case RXState:
    {
        // Transmit Path(RX State)step 1,go on
        if (check_and_cast<TransmissionRequestMessage *>(msg))
        {
            TransmissionRequestMessage *trMsg = static_cast<TransmissionRequestMessage *>(msg);
            MacMessage *macMsg = static_cast<MacMessage *>(trMsg->decapsulate());
            delete trMsg;
            transceiverState = TXState;//RXState -> TXState
            scheduleAt(simTime() + turnaroundTime, macMsg);// wait for the TurnaroundTime
        }
        // Carrier Sense(RX State)step 2,finish
        else if (check_and_cast<cMessage *>(msg))
        {
            if (strcmp(msg->getName(), "CARRIER_SENSE_WAIT") == 0)
            {
                delete msg;
                CSResponseMessage *crMsg = new CSResponseMessage;
                if (total_power_db > csThreshDBm)// compare with the CS threshold
                {
                    crMsg->setBusyChannel(true);
                }
                else
                {
                    crMsg->setBusyChannel(false);
                }

                send(crMsg, "gateForMAC$o");
                total_power_db=0.0;
            }else
            {
                delete msg;
            }
        }
        break;
    }

    case TXState:
    {
        // Transmit Path(TX State)step 1,go back to MAC
        if (check_and_cast<TransmissionRequestMessage *>(msg))
        {
            delete msg;
            TransmissionConfirmMessage * tcMsg = new TransmissionConfirmMessage("statusBusy");
            tcMsg->setStatus("statusBusy");
            send(tcMsg, "gateForMAC$o");
        }

        // Transmit Path(TX State)step 2,go on
        else if (check_and_cast<MacMessage *>(msg))//mac message is received from itself after the turnaround time
        {
            MacMessage *macMsg = static_cast<MacMessage *>(msg);

            // retrieve the packet length in bits from the mac packet (actually from AppMessage)
            int64_t packet_length = 8 * static_cast<AppMessage *>(macMsg->getEncapsulatedPacket())->getMsgSize();

            // send a SignalStart message to the channel
            SignalStartMessage *startMsg = new SignalStartMessage;

            // copy critical information
            startMsg->setIdentifier(nodeIdentifier);
            startMsg->setTransmitPowerDBm(txPowerDBm);
            startMsg->setPositionX(nodeXPosition);
            startMsg->setPositionY(nodeYPosition);
            startMsg->setCollidedFlag(false);

            // encapsulate the mac message
            startMsg->encapsulate(macMsg);
            macMsg = nullptr;

            // send the message to the channel
            send(startMsg, "gateForTXRXNode$o");

            // wait for the end of the packet transmission
            scheduleAt(simTime() + packet_length / bitRate, new cMessage("STEP_3"));
        }

        // Transmit Path(TX State)step 3,go on
        else if (check_and_cast<cMessage *>(msg))
        {
            if (strcmp(msg->getName(), "STEP_3") == 0)
            {
                delete msg;
                SignalStopMessage *stopMsg = new SignalStopMessage;
                int nodeIdentifier = getParentModule()->par("nodeIdentifier");
                stopMsg->setIdentifier(nodeIdentifier);// set identifier
                send(stopMsg, "gateForTXRXNode$o");// send the message to the channel
                scheduleAt(simTime() + turnaroundTime, new cMessage("STEP_4"));//
            }
        }

        // Transmit Path(TX State)step 4,finish
        else if (check_and_cast<cMessage *>(msg))
        {
            if (strcmp(msg->getName(), "STEP_4") == 0)
            {
                delete msg;
                transceiverState = RXState;//TXState -> RXState
                TransmissionConfirmMessage * tcMsg = new TransmissionConfirmMessage("statusOK");
                tcMsg->setStatus("statusOK");
                send(tcMsg, "gateForMAC$o");
            }
        }

        // Carrier Sense(TX State)step 2,finish
        else if (check_and_cast<cMessage *>(msg))
        {
            if (strcmp(msg->getName(), "CARRIER_SENSE_WAIT") == 0)
            {
                delete msg;
                CSResponseMessage *crMsg = new CSResponseMessage;
                crMsg->setBusyChannel(true);// send channel busy
                send(crMsg, "gateForMAC$o");
            }
        }

        else//unknown message
        {
            delete msg;
        }
        break;
    }
    }



}


double Transceiver::getReceivedPowerDBm(SignalStartMessage *startMsg)
{
    // calculate the euclidean distance between two nodes
    int otherXPosition = startMsg->getPositionX();
    int otherYPosition = startMsg->getPositionY();
    double dist = sqrt((nodeXPosition - otherXPosition) * (nodeXPosition - otherXPosition)
            +(nodeYPosition - otherYPosition) * (nodeYPosition - otherYPosition));

    //calculate path loss
    const double dist0 = 1.0;
    double path_loss_ratio;

    if (dist < dist0)    //distance<1.0  no packet loss
    {
        path_loss_ratio = 1;
    }
    else
    {
        path_loss_ratio = pow(dist, pathLossExponent);
    }

    double path_loss_db = 10 * log10(path_loss_ratio);    // convert the path loss ratio into decibal
    int transmitPowerDBm = startMsg->getTransmitPowerDBm();// get transmit power in db
    double receivedPowerDBm = transmitPowerDBm - path_loss_db;// calculate received power

    return receivedPowerDBm;
}
