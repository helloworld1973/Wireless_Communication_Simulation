module TransmitterNode
{
    parameters:
        int nodeIdentifier;
        int nodeXPosition;
        int nodeYPosition;
    gates:
        inout gateForChnl;
    submodules:
        PacketGeneratorModule: PacketGenerator;
        MACModule: MAC;
        TransceiverModule: Transceiver;
    connections:
        PacketGeneratorModule.gate <--> MACModule.gateForPacket;
        MACModule.gateForTX <--> TransceiverModule.gateForMAC;
        TransceiverModule.gateForTXRXNode <--> gateForChnl;    
}
