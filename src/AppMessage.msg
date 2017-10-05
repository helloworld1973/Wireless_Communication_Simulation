packet AppMessage {
    simtime_t timeStamp;//message generate simulation time
    int senderId;//node identification of the (TransmitterNode.nodeIdentifier)
   	int sequenceNumber;//current value of (seqno)(copy from PacketGenerator.seqno),then seqno++
   	int msgSize;//message size in bytes(from PacketGenerator.messageSize)
}
