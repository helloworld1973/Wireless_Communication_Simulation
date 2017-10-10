module ReceiverNode
{
	parameters:
		int nodeIdentifier;
    	int nodeXPosition;
    	int nodeYPosition;
	gates:
		inout gateForChnl;
	submodules:
		PacketSinkModule: PacketSink;
        MACModule: MAC;
        TransceiverModule: Transceiver;    
    connections allowunconnected:
        PacketSinkModule.in <-- MACModule.gateForPacket$o;
        MACModule.gateForTX <--> TransceiverModule.gateForMAC;       
        TransceiverModule.gateForTXRXNode <--> gateForChnl;
}