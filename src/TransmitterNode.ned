module TransmitterNode
{
    parameters:
        int nodeIdentifier;//set in configuration file
        int nodeXPosition;// maybe change to transciever later
        int nodeYPosition;// maybe change to transciever later
    gates:
        inout gateForChnl;// TXNode as a whole to connect to channel
    submodules:
        PacketGeneratorModule: PacketGenerator;
        MACModule: MAC;
        TransceiverModule: Transceiver;
    connections:
        PacketGeneratorModule.gate <--> MACModule.gateForPacket;
        MACModule.gateForTX <--> TransceiverModule.gateForMAC;
        TransceiverModule.gateForTXRXNode <--> gateForChnl;      
}
