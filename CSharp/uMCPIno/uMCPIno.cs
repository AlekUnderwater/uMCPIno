using System;

namespace uMCPIno
{
    [Flags]
    public enum uMCPInoPacketFlags
    {
        None,
        ACK = 1,
        REP = 2,
        STA = 4,
        STR = 8,
        DTA = 16,
        SEL = 32        
    }

    public enum uMCPInoPacketType : byte
    {
        ACK = (1 | 32),
        REP = (2 | 32),
        STA = (4 | 32),
        STR = (8 | 32),
        DTA = (1 | 16),
        DTE = (1 | 16 | 32),
        INVALID
    }

    public enum uMCPInoState
    {
        HALTED,
        ISTART,
        ASTART,
        RUNNING,
        INVALID
    }

    public class uMCPInoDataEventArgs : EventArgs
    {
        public byte[] Data { get; private set; }

        public uMCPInoDataEventArgs(byte[] data)
        {
            Data = data;
        }
    }

    public class uMCPInoActionInfoEventArgs : EventArgs
    {
        public string Info { get; private set; }

        public uMCPInoActionInfoEventArgs(string info)
        {
            Info = info;
        }
    }
    
    public static class uMCPIno
    {
        public static readonly int HSIZE = 4;
        public static readonly byte SIGN = 0xAD;

        public static void Rise(this EventHandler handler, object sender, EventArgs e)
        {
            if (handler != null)
                handler(sender, e);
        }

        public static void Rise<TEventArgs>(this EventHandler<TEventArgs> handler,
            object sender, TEventArgs e) where TEventArgs : EventArgs
        {
            if (handler != null)
                handler(sender, e);
        }              
    }
}
