using System;
using System.Collections.Generic;

namespace uMCPIno
{
    public class uMCPInoCTRLPacket : uMCPInoPacket
    {
        public uMCPInoCTRLPacket(uMCPInoPacketType pType, byte tcnt, byte rcnt) 
            : base(pType, tcnt, rcnt)
        {
            if ((pType == uMCPInoPacketType.DTA) &&
                (pType == uMCPInoPacketType.DTE))
                throw new ArgumentOutOfRangeException("pType");
        }

        public override string ToString()
        {
            return string.Format("{0}(TCNT={1}, RCNT={2})", this.PTYPE, TCNT, RCNT);
        }
    }   
   
    public class uMCPInoDATAPacket : uMCPInoPacket
    {       
        public byte[] DATA { get { return base.dATA; } }

        public uMCPInoDATAPacket(byte tCnt, byte rCnt, byte[] data, bool isSel)
            : base(isSel ? uMCPInoPacketType.DTE : uMCPInoPacketType.DTA, tCnt, rCnt)
        {            
            base.dATA = data;            
        }

        public override string ToString()
        {
            return string.Format("{0}(TCNT={1}, RCNT={2}, DCNT={3})", PTYPE, TCNT, RCNT, DATA.Length);
        }
    }

    public abstract class uMCPInoPacket
    {
        #region Properties

        public uMCPInoPacketType PTYPE { get; private set; }       
        public byte RCNT { get; private set; }
        public byte TCNT { get; private set; }
        protected byte[] dATA { get; set; }

        public static readonly int MIN_SIZE = 4;
        public static readonly int TRCNT_OFFSET = 2;

        #endregion

        #region Constructor

        public uMCPInoPacket(uMCPInoPacketType pType, byte tcnt, byte rcnt)
        {
            PTYPE = pType;
            RCNT = rcnt;
            TCNT = tcnt;            
        }
       
        #endregion

        #region Methods
       
        public byte[] Serialize()
        {
            List<byte> result = new List<byte>();
            result.Add(uMCPIno.SIGN);
            result.Add((byte)PTYPE);
            result.Add(Convert.ToByte((RCNT & 0x0F) | (TCNT << 4)));

            result.Add(CRC.CRC8_Get(result, 0, result.Count));

            if ((PTYPE == uMCPInoPacketType.DTA) ||
                (PTYPE == uMCPInoPacketType.DTE))
            {
                int dtaStartOffset = result.Count;
                result.Add(Convert.ToByte(dATA.Length));
                result.AddRange(dATA);
                result.Add(CRC.CRC8_Get(result, dtaStartOffset, dATA.Length + 1));
            }

            return result.ToArray();
        }

        #endregion
    }
}
