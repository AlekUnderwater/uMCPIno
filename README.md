# uMCPIno
uMCP Ino - a lightweight byte-oriented point-to-point protocol with handshake, integrity control, guaranteed packets ordering and delivery.

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

There are two types of nodes in the protocol: Selected by default (Master) and unselected by default (tributary).
All control messages transfer the SELECT flag. Node can transmitt only if has SELECT flag set to true.
If master node has transfered the SELECT flag and it has not returned back, the node will regain it by timeout.


# How to test?

- Program your Arduino board with [uMCPInoNode sketch](https://github.com/AlekUnderwater/uMCPIno/blob/master/Arduino/uMCPInoNode.ino)
- Connect your Arduino board with your PC via USB-UART converter
- On your PC, run the [uMCPIno Test application](https://github.com/AlekUnderwater/uMCPIno/tree/master/CSharp/uMCPIno_Test)
- Open Port monitor in Arduino IDE
- Connect the uMCPIno test application to the USB-UART converter port
- Now, to establish uMCPIno connection you can reset your Arduino board or press "START" button in the application.

Here some screenshots:

1. uMCPIno autostart on board power-on:
![](https://github.com/AlekUnderwater/uMCPIno/blob/master/Docs/Screenshots/autostart_on_poweron.png)

2. Example of handling with temporary dead line:
![](https://github.com/AlekUnderwater/uMCPIno/blob/master/Docs/Screenshots/line_dead.png)
