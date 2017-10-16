#include <omnetpp.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <ctgmath>

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
    int numOfTxToChannelPackets;
    int numOfLostInChannelPackets;

protected:
    typedef enum
    {
        RXState,
        TXState
    } TransceiverState_t;//finite state machine

    virtual void finish();
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    SignalStartMessage * updateCurrentTransmissions(SignalStopMessage *stopMsg);

    int txPowerDBm;
    int bitRate;
    int csThreshDBm;
    int noisePowerDBm;
    double turnaroundTime;
    double csTime;
    const double pathLossExponent=4.0;

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

void Transceiver::finish()
{
    FILE * filePointerToWrite = fopen("TxToChannelDataNum.txt", "a");
    if (filePointerToWrite == NULL) return;


    fprintf(filePointerToWrite, "TxOrRxNode               NumOfTxToChannelPackets           NumOfLostInChannelPackets           Position\n");
    fprintf(filePointerToWrite, "%d,                       %d,                               %d,                                    %d,%d\n",
                   nodeIdentifier, numOfTxToChannelPackets,numOfLostInChannelPackets, nodeXPosition, nodeYPosition);
    fclose(filePointerToWrite);
}

void Transceiver::initialize()
{
    numOfTxToChannelPackets = 0;
    numOfLostInChannelPackets=0;

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
    if (dynamic_cast<CSRequestMessage *>(msg))
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
    else if (dynamic_cast<SignalStartMessage *>(msg))
    {
        SignalStartMessage *startMsg = static_cast<SignalStartMessage *>(msg);
        if (!currentTransmissions.empty())//currentTransmission[] not empty
        {
            for (auto it = currentTransmissions.begin(); it != currentTransmissions.end(); it++)
            {
                if ((*it)->getIdentifier() == startMsg->getIdentifier())
                {
                    std::cout << "update SignalStartMessage to the CurrentTransmissions[] error !!!" << std::endl;
                    delete startMsg;
                    numOfLostInChannelPackets++;
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
    else if (dynamic_cast<SignalStopMessage *>(msg))
    {
        SignalStopMessage *stopMsg = static_cast<SignalStopMessage *>(msg);
        SignalStartMessage *startMsg=updateCurrentTransmissions(stopMsg);
        delete stopMsg;

        if(startMsg==NULL)
        {
            numOfLostInChannelPackets++;
            return;
        }

        if (startMsg->getCollidedFlag())//the collided flag is true
        {
            delete startMsg;
            numOfLostInChannelPackets++;
            return;//no further action
        }
        else
        {
            MacMessage *mpkt = static_cast<MacMessage *>(startMsg->decapsulate());// extract mac packet
            double received_power_db = getReceivedPowerDBm(startMsg);// calculate the received power in dBm
            delete startMsg;
            double bit_rate_db = 10 * log10(bitRate);// -> dB domain
            double snr_db = received_power_db - (noisePowerDBm + bit_rate_db);//calculate signal-to-noise ratio(SNR)
            double snr_n = pow(10, snr_db/10);// -> normal domain
            double bit_error_rate = erfc(sqrt(2 * snr_n));// calculate bit error rate
            int packet_length = 8 * static_cast<AppMessage *>(mpkt->getEncapsulatedPacket())->getMsgSize();//get packet length(1byte=8bits)
            double packet_error_rate = 1 - pow((1 - bit_error_rate), packet_length);// calculate packet error rate(PER=1-(1-BER)^n) PER=at least one bit is wrong

            srand(time(NULL));
            double u = (rand()%100)*0.01;//random num(0-1)( two numbers to the right of the decimal)
            if (u < packet_error_rate)
            {
                delete mpkt;
                numOfLostInChannelPackets++;
            }
            else
            {
                TransmissionIndicationMessage * tiMsg = new TransmissionIndicationMessage;
                tiMsg->encapsulate(mpkt);
                send(tiMsg, "gateForMAC$o");//send encapsulated MacMessage to higher layer
            }
            return;
        }
    }



    switch (transceiverState)//finite State Machine(related to carrier sense and Transmit Path)
    {
    case RXState:
    {
        // Transmit Path(RX State)step 1,go on
        if (dynamic_cast<TransmissionRequestMessage *>(msg))
        {
            TransmissionRequestMessage *trMsg = static_cast<TransmissionRequestMessage *>(msg);
            MacMessage *macMsg = static_cast<MacMessage *>(trMsg->decapsulate());
            transceiverState = TXState;//RXState -> TXState
            scheduleAt(simTime() + turnaroundTime, macMsg);// wait for the TurnaroundTime

            delete trMsg;

        }
        // Carrier Sense(RX State)step 2,finish
        else if (dynamic_cast<cMessage *>(msg))
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
        if (dynamic_cast<TransmissionRequestMessage *>(msg))
        {
            delete msg;
            TransmissionConfirmMessage * tcMsg = new TransmissionConfirmMessage("statusBusy");
            tcMsg->setStatus("statusBusy");
            send(tcMsg, "gateForMAC$o");
        }

        // Transmit Path(TX State)step 2,go on
        else if (dynamic_cast<MacMessage *>(msg))//mac message is received from itself after the turnaround time
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

            // send the message to the channel
            send(startMsg, "gateForTXRXNode$o");
            numOfTxToChannelPackets++;

            // wait for the end of the packet transmission
            scheduleAt(simTime() + packet_length / bitRate, new cMessage("STEP_3"));


        }

        // Transmit Path(TX State)step 3,go on
        else if (dynamic_cast<cMessage *>(msg))
        {
            if (strcmp(msg->getName(), "STEP_3") == 0)
            {
                delete msg;
                SignalStopMessage *stopMsg = new SignalStopMessage;
                stopMsg->setIdentifier(nodeIdentifier);// set identifier
                send(stopMsg, "gateForTXRXNode$o");// send the message to the channel
                scheduleAt(simTime() + turnaroundTime, new cMessage("STEP_4"));//
            }
            // Transmit Path(TX State)step 4,finish
            else if (strcmp(msg->getName(), "STEP_4") == 0)
            {
                delete msg;
                transceiverState = RXState;//TXState -> RXState
                TransmissionConfirmMessage * tcMsg = new TransmissionConfirmMessage("statusOK");
                tcMsg->setStatus("statusOK");
                send(tcMsg, "gateForMAC$o");
            }
            // Carrier Sense(TX State)step 2,finish
            else if (strcmp(msg->getName(), "CARRIER_SENSE_WAIT") == 0)
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
        path_loss_ratio = pow(dist, pathLossExponent);//dist^pathLossExponent
    }

    double path_loss_db = 10 * log10(path_loss_ratio);    // convert the path loss ratio into decibal
    int transmitPowerDBm = startMsg->getTransmitPowerDBm();// get transmit power in db
    double receivedPowerDBm = transmitPowerDBm - path_loss_db;// calculate received power

    return receivedPowerDBm;
}

SignalStartMessage * Transceiver::updateCurrentTransmissions(SignalStopMessage *stopMsg)
{
    for (auto it = currentTransmissions.begin(); it != currentTransmissions.end(); it++)
    {
        if ((*it)->getIdentifier() == stopMsg->getIdentifier())
        {
            SignalStartMessage *startMsg = *it;
            currentTransmissions.erase(it);
            return startMsg;
        }
    }
    std::cout << "update SignalStopMessage to the CurrentTransmissions[] error !!!" << std::endl;
    return NULL;
}
