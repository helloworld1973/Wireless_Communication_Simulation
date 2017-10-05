module TransmitterNode
{
    parameters:
        int nodeIdentifier;//set in configuration file
        int nodeXPosition;// maybe change to transciever later
        int nodeYPosition;// maybe change to transciever later
    submodules:
        PacketGeneratorModule: PacketGenerator;
        MACModule: MAC;
        TransceiverModule: Transceiver;
    connections:
            
    gates:
        inout gate;
}
