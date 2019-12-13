# uMCPIno
uMCP Ino -  is a free view on old (but still good!) DEC's [DDCMP protocol](https://en.wikipedia.org/wiki/Digital_Data_Communications_Message_Protocol).
* a lightweight (only 6 extra bytes for data packets and 4 bytes for control packets)
* byte-oriented
* point-to-point
* with handshake
* with [Nagle's algorithm](https://en.wikipedia.org/wiki/Nagle%27s_algorithm)
* pipelining
* integrity control
* guaranteed packets ordering
* and guaranteed delivery

In the Arduino-compatible implementation, there are some limitation due to memory size:
* the packet size is limited to 64 bytes
* the number of sent and unacknowledged packets is limited to 8
* the number of packets to be send in pipelining mode is limited to 4

# Message framing:

## Control Header
Offset, bytes|Item|Size, bits|   
-------------|----|----------|
0|SIGN = 0xAD|8|constant, packet start signature  
1|PTYPE|8|packet type. Can be one of the uMCP_PacketType  
2|TCNT|4|number of last transmitted packet  
2|RCNT|4|number of last received packet  
3|HCHK|8|header checksum  

```
if PTYPE == uMCP_PTYPE_DTA || PTYPE == uMCP_PTYPE_DTE
```
## Data block

Offset, bytes|Item|Size, bits|
-------------|----|----------|
4|DCNT|8|data packet size, should be from a range [1..64] inclusively, 0 is not valid value for this field  
..|DATA|8 * DCNT|data  
5 + DCNT|DCHK|8|packet checksum  

```
endif
```

There are two types of nodes in the protocol: 
* Selected by default (Master node)
* And unselected by default (tributary node)

A node can only send data messages when its SELECT flag is set to true

The main differences between Master and Tributary nodes are: 
* a master node has SELECT flag is set to true by default
* a tributary node should return the SELECT flag as soon as it transmits all the data to be transmitted
* if for some reason a master node doesn't receive the SELECT flag, it regains it by timeout

All control messages transfer the SELECT flag.


# How to test?

- Program your Arduino board with [uMCPInoNode sketch](https://github.com/AlekUnderwater/uMCPIno/blob/master/Arduino/uMCPInoNode.ino)
- Connect your Arduino board with your PC via USB-UART converter
- On your PC, run the [uMCPIno Test application](https://github.com/AlekUnderwater/uMCPIno/releases/download/1.0/uMCPIno_.zip)
- Open Port monitor in Arduino IDE
- Connect the uMCPIno test application to the USB-UART converter port
- Now, to establish uMCPIno connection you can reset your Arduino board or press "START" button in the application.

Here some screenshots:

## uMCPIno autostart on board power-on:
![](https://github.com/AlekUnderwater/uMCPIno/blob/master/Docs/Screenshots/autostart_on_poweron.png)

## Example of handling with temporary dead line:
![](https://github.com/AlekUnderwater/uMCPIno/blob/master/Docs/Screenshots/line_dead.png)

# How to use?

* if you want to use a standalone Arduino board as a uMCP-converter, when you put some data to it, and want it
to be safely transferred to another uMCP Node, use [uMCPIno.ino sketch](https://github.com/AlekUnderwater/uMCPIno/blob/master/Arduino/uMCPIno.ino). It supposed to use 2 serial ports - one to send and receive data bytes, and another to use as uMCP line (it can be Bluetooth, RF, or even underwater acoustic link).
* if you want to embed the uMCPIno protocol to your project, you can use the [uMCPInoNode.ino sketch](https://github.com/AlekUnderwater/uMCPIno/blob/master/Arduino/uMCPInoNode.ino). In this case you should change the sketch's code:
    1) To send data over uMCPIno link use the function:  
    ```bool uMCPIno_SendData(byte* dataToSend, byte dataSize);```
    where:  
    ```dataToSend``` - a byte buffer to send  
    ```dataSize``` - size of the buffer  
    2) Before sending data, check if there is enough space in tx buffer:  
    ```bool uMCP_IsCanSend(byte dataSize);```  
    where:  
    ```dataSize``` - number of bytes to send  
    
    3) To analyse the received data:    
    The received over uMCP link data is stored in a ring buffer ```il_ring```.   
    The amount of received bytes is stored in ```il_Cnt```  
    The read pointer is in ```il_rPos```.  
    To perform the analysis of the received data, put your code in function:   
    ```void USER_uMCPIno_DataPacketReceived();```  
    It fires when new data packet has been received from a remote node over uMCP line  

    4) If you need to be informed, when the tx buffer is empty, put your code in function:           
    ```void USER_uMCP_OnTxBufferEmptry();```   
    it fires when all sent messages has been acknowlegded by the remote node

