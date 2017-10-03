network TransmitterNode
{
    @display("bgb=234,418");
    submodules:
        PacketGeneratorModule: PacketGenerator {
            @display("p=113,66");
        }
        MACModule: MAC {
            @display("p=113,190");
        }
        TransceiverModule: Transceiver {
            @display("p=113,307");
        }
    connections:
        PacketGeneratorModule.out --> {  delay = 100ms; } --> MACModule.in;
        MACModule.out <--> {  delay = 100ms; } <--> TransceiverModule.gate;
}