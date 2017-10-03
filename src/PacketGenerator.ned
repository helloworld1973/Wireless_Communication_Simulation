simple PacketGenerator
{
	parameters:
		double iatDistribution;// inter-generation time(random distribution) 
		int messageSize;//message size in bytes(to PacketGenerator.msgSize) 
	gates:
		inout gate;
}