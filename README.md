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
