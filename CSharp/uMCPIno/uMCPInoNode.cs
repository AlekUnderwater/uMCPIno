using System;
using System.Collections.Generic;

namespace uMCPIno
{    
    /// <summary>    
    ///  uMCPIno node
    /// </summary>

    public class uMCPInoNode
    {
        #region Properties

        byte R = 0;
        byte N = 0;        
        byte A = 0;

        uMCPInoState state = uMCPInoState.HALTED;
        public uMCPInoState STATE
        {
            get { return state; }
            private set
            {
                if (value != state)
                {
                    state = value;
                    if ((state == uMCPInoState.ISTART) ||
                        (state == uMCPInoState.HALTED))
                    {
                        R = 0;
                        N = 0;                        
                        A = 0;
                        sentDataBlocks.Clear();
                    }

                    OnSTATEChangedEventHandler.Rise(this, new EventArgs());
                }
            }
        }

        DateTime timeoutTimerStartTime;
        bool isTimeoutTimerRunning = false;
        bool IsTimeoutTimerRunning
        {
            get { return isTimeoutTimerRunning; }
            set
            {
                if (value)
                    timeoutTimerStartTime = DateTime.Now;
                else
                {
                    // in case transmission passed faster than expected
                    if (isTxFinishedTimerRunning)
                        IsTxFinishedTimerRunning = false;
                }

                isTimeoutTimerRunning = value;                
            }
        }
        
        DateTime selectTimerStartTime;
        bool isSelectTimerRunning = false;
        bool IsSELECTTimerRunning
        {
            get { return isSelectTimerRunning; }
            set
            {
                if (value)
                    selectTimerStartTime = DateTime.Now;
                isSelectTimerRunning = value;                
            }
        }

        bool selectDefaultState = false;
        bool select = false;
        public bool SELECT
        {
            get { return select; }
            private set
            {
                if (selectDefaultState)
                    IsSELECTTimerRunning = (value != selectDefaultState);

                select = value;

                if (select)
                    PerformProtocol();

                OnSELECTChangedEventHandler.Rise(this, new EventArgs());
            }
        }

        bool isTimerStartPendingOnTxFinish = false;

        uMCPInoReceiver receiver;

        private delegate void packetProcessorDelegate(uMCPInoPacket packet);
        private Dictionary<uMCPInoPacketType, packetProcessorDelegate> onIncomingPacket;
        
        uint timeoutIntervalMs = 1000;
        uint selectIntervalMs = 2000;

        uMCPInoPacket sentPacket;

        TSQueue<byte[]> dataToSend;
        Dictionary<int, byte[]> sentDataBlocks;

        public byte MaxMessages { get; private set; }

        public byte PacketSize { get; private set; }
        
        double fixedTxDelayS = 0.05;
        double baudRateBps = 9600;
        public double BaudrateBps
        {
            get { return baudRateBps; }
            set
            {
                if ((value > 0) && (value <= 115200))
                    baudRateBps = value;
                else
                    throw new ArgumentOutOfRangeException();
            }
        }

        DateTime txFinishedTimerStartTime;
        uint txFinishedInterval = 1000;
        bool isTxFinishedTimerRunning = false;
        bool IsTxFinishedTimerRunning
        {
            get { return isTxFinishedTimerRunning; }
            set
            {
                if (value)
                    txFinishedTimerStartTime = DateTime.Now;

                isTxFinishedTimerRunning = value;
            }
        }
        
        bool SREP = false;
        bool SACK = false;

        public uint DTADTESent { get; private set; }
        public uint REPSent { get; private set; }
        public uint ACKSent { get; private set; }
        public uint Timeouts { get; private set; }
        public uint BytesTransmitted { get; private set; }        

        public uint DTADTEReceived { get; private set; }
        public uint REPReceived { get; private set; }
        public uint ACKReceived { get; private set; }
        public uint BytesReceived { get; private set; }

        int pipeliningLimit = 4;
        public int PipeliningLimit
        {
            get { return pipeliningLimit; }
            set
            {
                if ((value >= 1) && (value < 128))
                    pipeliningLimit = value;
                else
                    throw new ArgumentOutOfRangeException();
            }
        }

        #endregion

        #region Constructor

        public uMCPInoNode(bool selDefState, uint toutIntervalMs, uint selIntervalMs, byte packetSize, byte maxMessages)
        {
            selectDefaultState = selDefState;
            timeoutIntervalMs = toutIntervalMs;
            selectIntervalMs = selIntervalMs;

            /// TODO: check ranges
            PacketSize = packetSize;
            MaxMessages = maxMessages;

            receiver = new uMCPInoReceiver(1024);

            onIncomingPacket = new Dictionary<uMCPInoPacketType, packetProcessorDelegate>();
            onIncomingPacket.Add(uMCPInoPacketType.STR, onSTR);
            onIncomingPacket.Add(uMCPInoPacketType.STA, onSTA);
            onIncomingPacket.Add(uMCPInoPacketType.REP, onREP);
            onIncomingPacket.Add(uMCPInoPacketType.ACK, onACK);
            onIncomingPacket.Add(uMCPInoPacketType.DTA, onDTADTE);
            onIncomingPacket.Add(uMCPInoPacketType.DTE, onDTADTE);

            dataToSend = new TSQueue<byte[]>();
            sentDataBlocks = new Dictionary<int, byte[]>();

            DTADTESent = 0;
            REPSent = 0;
            ACKSent = 0;
            Timeouts = 0;
            BytesTransmitted = 0;
            
            DTADTEReceived = 0;
            REPReceived = 0;
            ACKReceived = 0;
            BytesReceived = 0;
        }

        #endregion

        #region Methods

        #region Public

        public void Start()
        {
            if (state == uMCPInoState.HALTED)
            {
                STATE = uMCPInoState.ISTART;
                SendPacket(new uMCPInoCTRLPacket(uMCPInoPacketType.STR, 0, 0), true);                
            }
            else
                throw new InvalidOperationException("Protocol should be in HALTED state to perform such operation");
        }

        public void Stop()
        {
            STATE = uMCPInoState.HALTED;            
        }

        public void OnIncomingData(byte[] data)
        {
            if (receiver.InputDataProcess(data) > 0)
            {
                var packets = receiver.ReadAllPackets();
                foreach (var packet in packets)
                    if (onIncomingPacket.ContainsKey(packet.PTYPE))
                    {
                        OnActionInfoEventHandler.Rise(this, new uMCPInoActionInfoEventArgs(string.Format("Received: {0}", packet.ToString())));
                        onIncomingPacket[packet.PTYPE](packet);
                    }
            }
        }

        public void OnTransmissionFinished()
        {
            if (isTimerStartPendingOnTxFinish && !isTimeoutTimerRunning)
            {
                isTimerStartPendingOnTxFinish = false;
                IsTimeoutTimerRunning = true;
            }
            else
                PerformProtocol();
        }


        public bool IsCanSend(int dataSize)
        {
            return (sentDataBlocks.Count + dataSize / PacketSize < MaxMessages);
        }

        public bool Send(byte[] data)
        {
            if (IsCanSend(data.Length))
            {
                if (data.Length <= PacketSize)
                    dataToSend.Enqueue(data);
                else
                {
                    int rPos = 0;
                    int step = PacketSize;
                    while (rPos < data.Length)
                    {
                        if (data.Length - rPos >= PacketSize)
                            step = PacketSize;
                        else
                            step = data.Length - rPos;

                        byte[] block = new byte[step];
                        for (int i = 0; i < step; i++)
                            block[i] = data[rPos + i];

                        rPos += step;
                        dataToSend.Enqueue(block);
                    }
                }

                PerformProtocol();

                return true;
            }
            else
            {
                return false;
            }
        }

        public void OnTick()
        {
            DateTime now = DateTime.Now;

            if ((isSelectTimerRunning) &&
                (now.Subtract(selectTimerStartTime).TotalMilliseconds > selectIntervalMs))
            {
                SELECT = selectDefaultState;                
            }

            if ((isTimeoutTimerRunning) &&
                (now.Subtract(timeoutTimerStartTime).TotalMilliseconds > timeoutIntervalMs))
            {
                IsTimeoutTimerRunning = false;
                OnTimeout();

                Timeouts++;
            }

            if ((isTxFinishedTimerRunning) &&
                (now.Subtract(txFinishedTimerStartTime).TotalMilliseconds > txFinishedInterval))
            {
                IsTxFinishedTimerRunning = false;
                OnTransmissionFinished();
            }            
        }

        public override string ToString()
        {
            return string.Format("R={0}, N={1}, A={2}, SACK={3}, SREP={4}, SELECT={5}, STATE={6}",
                R, N, A, SACK, SREP, SELECT, STATE);
        }

        #endregion

        #region Private

        private void SendPacket(uMCPInoPacket packet, bool isStartTimer)
        {
            OnActionInfoEventHandler.Rise(this, new uMCPInoActionInfoEventArgs(string.Format("Sending: {0}", packet.ToString())));

            var serializedPacket = packet.Serialize();
            OnOutcomingEventHandler.Rise(this, new uMCPInoDataEventArgs(serializedPacket));
            sentPacket = packet;
            isTimerStartPendingOnTxFinish = isStartTimer;
            txFinishedInterval = Convert.ToUInt32(1000 * (fixedTxDelayS + serializedPacket.Length * 8 / baudRateBps));
            IsTxFinishedTimerRunning = true;

            SELECT = (packet.PTYPE == uMCPInoPacketType.DTA);
           
            if (packet.PTYPE == uMCPInoPacketType.ACK)
                ACKSent++;
            else if ((packet.PTYPE == uMCPInoPacketType.DTA) || (packet.PTYPE == uMCPInoPacketType.DTE))
                DTADTESent++;
            else if (packet.PTYPE == uMCPInoPacketType.REP)
                REPSent++;
        }

        private void OnTimeout()
        {
            OnActionInfoEventHandler.Rise(this, new uMCPInoActionInfoEventArgs("TIMEOUT"));

            if (state == uMCPInoState.ISTART)
                SendPacket(new uMCPInoCTRLPacket(uMCPInoPacketType.STR, 0, 0), true);
            else if (state == uMCPInoState.ASTART)
                SendPacket(new uMCPInoCTRLPacket(uMCPInoPacketType.STA, 0, 0), true);
            else if (state == uMCPInoState.RUNNING)
            {
                SREP = true;
                PerformProtocol();
            }
        }

        private void PerformProtocol()
        {
            if (state == uMCPInoState.RUNNING)
            {
                if ((!isTxFinishedTimerRunning) &&
                    (!isTimeoutTimerRunning) &&
                    (select))
                {
                    if (dataToSend.Count == 0)
                    {
                        if (SREP)
                        {
                            SendPacket(new uMCPInoCTRLPacket(uMCPInoPacketType.REP, N, R), true);
                            SREP = false;
                        }
                        else if (sentDataBlocks.Count > 0)
                        {
                            byte x = Convert.ToByte((A + 1) % MaxMessages);
                            SendPacket(new uMCPInoDATAPacket(x, R, sentDataBlocks[x], true), true);
                        }
                        else if ((!selectDefaultState) || (SACK))
                        {
                            SendPacket(new uMCPInoCTRLPacket(uMCPInoPacketType.ACK, N, R), false);
                            SACK = false;
                        }                                         
                    }
                    else
                    {
                        var blockToSend = dataToSend.Dequeue();
                        N = Convert.ToByte((N + 1) % MaxMessages);
                        sentDataBlocks.Add(N, blockToSend);
                        bool isDTE = (dataToSend.Count == 0) || (sentDataBlocks.Count >= pipeliningLimit);
                        SendPacket(new uMCPInoDATAPacket(N, R, blockToSend, isDTE), isDTE);
                    }
                }
            }             
        }

        private void AcknowledgeSentItems(int to)
        {
            byte from = A;
            byte a = from;
            while (a != to)
            {
                a = Convert.ToByte((a + 1) % MaxMessages);
                A = Convert.ToByte((A + 1) % MaxMessages);
                var acknowledgedBlock = sentDataBlocks[A];
                BytesTransmitted += Convert.ToUInt32(acknowledgedBlock.Length);
                sentDataBlocks.Remove(A);
                OnDataBlockAcknowledgedEventHandler.Rise(this, new uMCPInoDataEventArgs(acknowledgedBlock));                                
            }

            if (sentDataBlocks.Count == 0)
                OnTransmitterEmptyEventHandler.Rise(this, new EventArgs()); 
        }

        private bool IsByteInRangeExclusive(byte st, byte nd, byte val)
        {
            bool result = false;
            byte idx = st;            
            byte _nd = nd;
            _nd--;
            while ((idx != _nd) && (!result))
            {
                idx = Convert.ToByte((idx + 1) % MaxMessages);
                if (idx == val)
                    result = true;                
            }

            return result;
        }

        #region Custom packet processor

        private void onSTR(uMCPInoPacket packet)
        {
            SELECT = true;
            if ((state == uMCPInoState.HALTED) || (state == uMCPInoState.ISTART))
            {
                #region state == HALTED || ISTART

                STATE = uMCPInoState.ASTART;
                IsTimeoutTimerRunning = false;
                SendPacket(new uMCPInoCTRLPacket(uMCPInoPacketType.STA, 0, 0), true);

                #endregion
            }
            else if (state == uMCPInoState.ASTART)
            {
                #region state == ASTART

                STATE = uMCPInoState.RUNNING;
                IsTimeoutTimerRunning = false;
                SendPacket(new uMCPInoCTRLPacket(uMCPInoPacketType.ACK, 0, 0), false);

                #endregion
            }
            else
            {
                STATE = uMCPInoState.HALTED;
            }            
        }

        private void onSTA(uMCPInoPacket packet)
        {
            SELECT = true;

            if ((state == uMCPInoState.ISTART) || (state == uMCPInoState.ASTART) || (state == uMCPInoState.RUNNING))
            {
                STATE = uMCPInoState.RUNNING;
                IsTimeoutTimerRunning = false;
                SendPacket(new uMCPInoCTRLPacket(uMCPInoPacketType.ACK, 0, 0), false);                
            }
        }

        private void onREP(uMCPInoPacket packet)
        {
            REPReceived++;

            if (state == uMCPInoState.RUNNING)
            {
                IsTimeoutTimerRunning = false;
                SACK = true;
                SREP = false;
                SELECT = true;                
            }
        }

        private void onACK(uMCPInoPacket packet)
        {
            ACKReceived++;
            
            if (state == uMCPInoState.ASTART)
            {
                #region state == ASTART

                if (packet.RCNT == 0)
                {
                    IsTimeoutTimerRunning = false;
                    STATE = uMCPInoState.RUNNING;
                }
                
                #endregion
            }
            else if (state == uMCPInoState.RUNNING)
            {
                #region state == RUNNING

                SREP = false;
                IsTimeoutTimerRunning = false;
                if ((packet.RCNT == N) || (IsByteInRangeExclusive(A, N, packet.RCNT)))
                    AcknowledgeSentItems(packet.RCNT);

                #endregion
            }

            SELECT = true;
        }

        private void onDTADTE(uMCPInoPacket packet)
        {
            DTADTEReceived++;

            if (state == uMCPInoState.RUNNING)
            {
                uMCPInoDATAPacket dPacket = (packet as uMCPInoDATAPacket);
                                
                if (dPacket.TCNT <= R + 1)
                {
                    if (dPacket.TCNT == Convert.ToByte((R + 1) % MaxMessages))
                    {
                        R = Convert.ToByte((R + 1) % MaxMessages);
                        OnDataBlockReceivedEventHandler.Rise(this, new uMCPInoDataEventArgs(dPacket.DATA));
                        BytesReceived += Convert.ToUInt32(dPacket.DATA.Length);
                    }

                    SACK = true;
                }

                SREP = false;
                IsTimeoutTimerRunning = false;
                            
                if ((dPacket.RCNT == N) || (IsByteInRangeExclusive(A, N, dPacket.RCNT)))
                    AcknowledgeSentItems(dPacket.RCNT);

                if (dPacket.PTYPE == uMCPInoPacketType.DTA)
                    IsTimeoutTimerRunning = true;

                SELECT = (dPacket.PTYPE == uMCPInoPacketType.DTE);
            }
        }

        #endregion

        #endregion

        #endregion

        #region Events

        public EventHandler OnSTATEChangedEventHandler;
        public EventHandler OnSELECTChangedEventHandler;

        public EventHandler<uMCPInoDataEventArgs> OnDataBlockReceivedEventHandler;
        public EventHandler<uMCPInoDataEventArgs> OnOutcomingEventHandler;
        public EventHandler<uMCPInoDataEventArgs> OnDataBlockAcknowledgedEventHandler;

        public EventHandler OnTransmitterEmptyEventHandler;

        public EventHandler<uMCPInoActionInfoEventArgs> OnActionInfoEventHandler;

        #endregion
    }
}
