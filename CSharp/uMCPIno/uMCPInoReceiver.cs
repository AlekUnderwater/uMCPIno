using System;
using System.Collections.Generic;
using System.Threading;

namespace uMCPIno
{
    public class uMCPInoReceiver
    {
        #region Properties

        byte[] ring;

        int rSize = 0;
        int rPos = 0;
        int wPos = 0;
        int rCnt = 0;

        int rLock = 0;

        List<uMCPInoPacket> packets;

        #endregion

        #region Constructor

        public uMCPInoReceiver(int ringSize)
        {
            if (ringSize <= 0)
                throw new ArgumentOutOfRangeException("ringSize");

            rSize = ringSize;
            ring = new byte[rSize];

            packets = new List<uMCPInoPacket>();
        }

        #endregion        

        #region Methods

        private byte RingReadByte(int rPos, int rCnt, out int rPosOut, out int rCntOut)
        {
            if (rCnt > 0)
            {
                byte result = ring[rPos];
                rPosOut = (rPos + 1) % rSize;
                rCntOut = rCnt - 1;
                return result;
            }
            else
            {
                throw new ArgumentOutOfRangeException();
            }
        }

        private byte[] RingRead(int rPos, int rCnt, int cnt, out int rPosOut, out int rCntOut)
        {
            if ((rCnt >= cnt) && (cnt > 0))
            {
                rPosOut = rPos;
                rCntOut = rCnt;

                byte[] result = new byte[cnt];
                for (int i = 0; i < cnt; i++)
                {
                    result[i] = ring[rPosOut];
                    rPosOut = (rPosOut + 1) % rSize;
                    rCntOut--;
                }

                return result;
            }
            else
            {
                throw new ArgumentOutOfRangeException("cnt");
            }

        }

        public int InputDataProcess(byte[] data)
        {
            while (Interlocked.CompareExchange(ref rLock, 1, 0) == 0)
                Thread.SpinWait(1);

            #region write data to ring

            for (int i = 0; i < data.Length; i++)
            {
                ring[wPos] = data[i];
                wPos = (wPos + 1) % rSize;
                rCnt++;
            }

            #endregion

            bool isStep = false;
            bool isEmpty = false;
            while (!isEmpty)
            {
                #region look for a packet start

                bool isPacketStart = false;
                while ((rCnt >= uMCPInoPacket.MIN_SIZE) && (!isPacketStart))
                {
                    if (ring[rPos] == uMCPIno.SIGN)
                        isPacketStart = true;
                    else
                    {
                        rPos = (rPos + 1) % rSize;
                        rCnt--;
                    }
                }

                #endregion

                if (isPacketStart)
                {
                    #region try to parse

                    int rPosCache = rPos;
                    int rCntCache = rCnt;

                    RingReadByte(rPosCache, rCntCache, out rPosCache, out rCntCache); // skip SIGN
                    uMCPInoPacketType pType = (uMCPInoPacketType)RingReadByte(rPosCache, rCntCache, out rPosCache, out rCntCache);                    

                    if (pType != uMCPInoPacketType.INVALID)
                    {                        
                        rPosCache = rPos;
                        rCntCache = rCnt;

                        if (rCntCache >= uMCPIno.HSIZE)
                        {
                            #region read header & its CRC

                            var packet = RingRead(rPosCache, rCntCache, uMCPIno.HSIZE, out rPosCache, out rCntCache);

                            byte declaredPacketCRC = packet[packet.Length - 1];
                            byte actualPacketCRC = CRC.CRC8_Get(packet, 0, packet.Length - 1);

                            #endregion

                            if (declaredPacketCRC == actualPacketCRC)
                            {
                                #region if ctrl header CRC is OK

                                #region decode control header

                                byte trcnt = packet[uMCPInoPacket.TRCNT_OFFSET];
                                byte tcnt = Convert.ToByte((trcnt >> 4) & 0x0F);
                                byte rcnt = Convert.ToByte(trcnt & 0x0F);

                                #endregion

                                if ((pType != uMCPInoPacketType.DTA) && (pType != uMCPInoPacketType.DTE))
                                {
                                    isStep = true;
                                    packets.Add(new uMCPInoCTRLPacket(pType, tcnt, rcnt));
                                }
                                else
                                {
                                    #region DTA & DTE

                                    // try to get dcnt
                                    if (rCntCache > 1)
                                    {
                                        byte actualDBlockCRC = 0xFF;
                                        byte dcnt = RingReadByte(rPosCache, rCntCache, out rPosCache, out rCntCache);
                                        if (dcnt >= 1)
                                        {
                                            actualDBlockCRC = CRC.CRC8_Update(dcnt, actualDBlockCRC);
                                            if (rCntCache >= dcnt + 1)
                                            {
                                                var dataBlock = RingRead(rPosCache, rCntCache, dcnt, out rPosCache, out rCntCache);
                                                actualDBlockCRC = CRC.CRC8_Update(dataBlock, 0, dataBlock.Length, actualDBlockCRC);
                                                byte declaredDBlockCRC = RingReadByte(rPosCache, rCntCache, out rPosCache, out rCntCache);

                                                if (declaredDBlockCRC == actualDBlockCRC)
                                                {
                                                    packets.Add(new uMCPInoDATAPacket(tcnt, rcnt, dataBlock, (pType == uMCPInoPacketType.DTE)));
                                                }

                                                isStep = true; // even if bad crc
                                            }
                                            else
                                            {
                                                // wait for data...
                                                isEmpty = true;
                                            }
                                        }
                                        else
                                        {
                                            // bad data block, use only header
                                            isStep = true;
                                            packets.Add(new uMCPInoCTRLPacket(uMCPInoPacketType.ACK, tcnt, rcnt));
                                        }
                                    }
                                    else
                                    {
                                        // wait for data...
                                        isEmpty = true;
                                    }

                                    #endregion                                
                                }
                               
                                #endregion
                            }
                            else
                            {
                                // bad crc, just skip it
                                isStep = true;
                            }
                        }
                        else
                        {
                            // leave, wait for data
                            isEmpty = true;
                        }
                    }
                    else
                    {
                        // bad or not a packet, skip
                        isStep = true;
                    }

                    if (isStep)
                    {
                        rPos = rPosCache;
                        rCnt = rCntCache;
                    }

                    #endregion
                }
                else
                {
                    // no packet start found
                    isEmpty = true;
                }                                    
            } // while (!isEmpty)...

            Interlocked.Decrement(ref rLock);

            return packets.Count;
        }

        public List<uMCPInoPacket> ReadAllPackets()
        {
            List<uMCPInoPacket> result = new List<uMCPInoPacket>();

            while (Interlocked.CompareExchange(ref rLock, 1, 0) == 0)
                Thread.SpinWait(1);
            
            result.AddRange(packets.ToArray());
            packets.Clear();

            Interlocked.Decrement(ref rLock);

            return result;
        }

        #endregion
    }
}
