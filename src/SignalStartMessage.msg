packet SignalStartMessage extends SignalMessage
{
    int transmitPowerDBm;
    
    int positionX;
    int positionY;
    
    bool collidedFlag = false;
}
