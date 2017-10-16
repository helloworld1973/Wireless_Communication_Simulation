network Simulation
{
    parameters:
		int numTransmitters;
		int radius;
    
	submodules:
		TransmitterNodeModule[numTransmitters] : TransmitterNode
		{
			nodeIdentifier = index;	
			nodeXPosition = radius * cos(2 * 3.1416 * index / (numTransmitters));
			nodeYPosition = radius * sin(2 * 3.1416 * index / (numTransmitters));
			
			@display("p=$nodeXPosition,$nodeYPosition");
		};
		ReceiverNodeModule : ReceiverNode
		{
			nodeIdentifier = -1;
			nodeXPosition = 0;
			nodeYPosition = 0;
			
			@display("p=$nodeXPosition,$nodeYPosition");
		};
		ChannelModule : Channel;

	connections:
		ChannelModule.gateNet++ <--> ReceiverNodeModule.gateForChnl;
		for i=0..numTransmitters - 1 
		{
			ChannelModule.gateNet++ <--> TransmitterNodeModule[i].gateForChnl;
		}
}