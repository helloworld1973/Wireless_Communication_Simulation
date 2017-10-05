packet SignalStartMessage
{
    int identifier;
    
    int transmitPowerDBm;
    
    int positionX;
    int positionY;
    
    bool collidedFlag = false;
}
