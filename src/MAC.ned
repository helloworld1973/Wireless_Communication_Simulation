
simple MAC
{
	parameters:
		int bufferSize;
		int maxBackoffs;
		double backoffDistribution;
		
	gates:
		inout gateForPacket;
		inout gateForTX;
}
