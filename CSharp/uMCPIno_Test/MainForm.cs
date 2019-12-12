using System;
using System.IO.Ports;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using uMCPIno;

namespace uMCPTest
{
    public partial class MainForm : Form
    {
        #region Properties

        uMCPInoPort port;
        Random rnd = new Random();

        bool isExtra = true;

        #endregion

        #region Constructor

        public MainForm()
        {
            InitializeComponent();

            connectBtn.Enabled = false;
            startBtn.Enabled = false;
            sendBtn.Enabled = false;

            var portNames = SerialPort.GetPortNames();

            if (portNames.Length > 0)
            {
                portNameCbx.Items.AddRange(portNames);
                portNameCbx.SelectedIndex = 0;
                connectBtn.Enabled = true;                              
            }
        }

        #endregion
        
        #region Methods

        private void AppendHLine(string line)
        {
            historyTxb.AppendText(line);
            if (!line.EndsWith("\r\n"))
                historyTxb.AppendText("\r\n");
        }

        private void InvokeAppendHLine(string line)
        {
            if (historyTxb.InvokeRequired)
                historyTxb.Invoke((MethodInvoker)delegate { AppendHLine(line); });
            else
                AppendHLine(line);
        }

        private void InvokeSetEnabledState(ToolStrip strip, ToolStripItem item, bool enabled)
        {
            if (strip.InvokeRequired)
                strip.Invoke((MethodInvoker)delegate { item.Enabled = enabled; });
            else
                item.Enabled = enabled;
        }

        private void CheckSendEnabled()
        {
            sendBtn.Enabled = (!string.IsNullOrEmpty(textToSendTxb.Text)) && (port != null) && (port.IsOpen) && (port.State == uMCPInoState.RUNNING);            
        }

        private void InvokeCheckSendEnabled()
        {
            if (this.InvokeRequired)
                this.Invoke((MethodInvoker)delegate { CheckSendEnabled(); });
            else
                CheckSendEnabled();
        }

        private void InvokeSetText(StatusStrip strip, ToolStripStatusLabel lbl, string text)
        {
            if (strip.InvokeRequired)
                strip.Invoke((MethodInvoker)delegate { lbl.Text = text; });
            else
                lbl.Text = text;
        }

        private void InvokeSetText(ToolStrip strip, ToolStripButton btn, string text)
        {
            if (strip.InvokeRequired)
                strip.Invoke((MethodInvoker)delegate { btn.Text = text; });
            else
                btn.Text = text;
        }

        private string GetRandomString(int length)
        {
            byte[] bytes = new byte[length / 2];
            rnd.NextBytes(bytes);

            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < bytes.Length; i++)
                sb.AppendFormat("{0:X2}", bytes[i]);

            return sb.ToString();
        }


        #endregion

        #region Handlers

        private void connectBtn_Click(object sender, EventArgs e)
        {
            if ((port == null) || (!port.IsOpen))
            {
                try
                {

                    port = new uMCPInoPort(portNameCbx.SelectedItem.ToString(),
                        UCNLDrivers.BaudRate.baudRate9600, isSelectedByDefaultChb.Checked, 8100, 2000, 64, 8);

                    port.Open();

                    port.OnSelectChangedEventHandler += (o, ex) => 
                    { 
                        if (isExtra) InvokeAppendHLine(string.Format("SELECT={0}\r\n", port.Select));
                        InvokeSetText(mainStatusStrip, selectLbl, port.Select.ToString());
                    };

                    port.OnStateChangedEventHandler += (o, ex) => 
                    { 
                        if (isExtra) InvokeAppendHLine(string.Format("STATE={0}\r\n", port.State));
                        InvokeSetEnabledState(mainToolStrip, startBtn, (port.State == uMCPInoState.RUNNING) || (port.State == uMCPInoState.HALTED));
                        InvokeCheckSendEnabled();

                        if (port.State == uMCPInoState.RUNNING)
                            InvokeSetText(mainToolStrip, startBtn, "STOP");
                        else if (port.State == uMCPInoState.HALTED)
                            InvokeSetText(mainToolStrip, startBtn, "START");
                        else
                            InvokeSetText(mainToolStrip, startBtn, "...");

                        InvokeSetText(mainStatusStrip, stateLbl, port.State.ToString());
                    };
                    port.OnDataBlockAcknowledgedEventHandler += (o, ex) => { InvokeAppendHLine(string.Format("<< ACK: \"{0}\"", Encoding.ASCII.GetString(ex.Data))); };
                    port.OnDataBlockReceivedEventHandler += (o, ex) => { InvokeAppendHLine(string.Format(">>: \"{0}\"", Encoding.ASCII.GetString(ex.Data))); };
                    port.OnActionInfoEventHandler += (o, ex) => { if (isExtra) InvokeAppendHLine(ex.Info); };

                    startBtn.Enabled = true;
                    isSelectedByDefaultChb.Enabled = false;

                    connectBtn.Checked = true;
                    connectBtn.Text = "DISCONNECT";

                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            else
            {
                try
                {
                    port.Close();
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }

                connectBtn.Checked = false;
                connectBtn.Text = "CONNECT";
                startBtn.Enabled = false;
                isSelectedByDefaultChb.Enabled = true;
                sendBtn.Enabled = false;
            }
        }

        private void startBtn_Click(object sender, EventArgs e)
        {
            if (port.State == uMCPInoState.HALTED)
                port.Start();
            else
                port.Stop();
        }

        private void sendBtn_Click(object sender, EventArgs e)
        {            
            port.Send(Encoding.ASCII.GetBytes(textToSendTxb.Text));
            AppendHLine(string.Format("<<: \"{0}\"", textToSendTxb.Text));
            textToSendTxb.Clear();
        }

        private void historyTxb_TextChanged(object sender, EventArgs e)
        {
            historyTxb.ScrollToCaret();
        }

        private void textToSendTxb_TextChanged(object sender, EventArgs e)
        {
            CheckSendEnabled();
        }

        private void clearHistoryBtn_Click(object sender, EventArgs e)
        {
            historyTxb.Clear();
        }

        private void textToSendTxb_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            if (e.KeyData == Keys.Return)
            {
                if (sendBtn.Enabled)
                    sendBtn_Click(sendBtn, new EventArgs());
            }
        }

        private void isExtraOutputBtn_Click(object sender, EventArgs e)
        {
            isExtra = isExtraOutputBtn.Checked;
        }

        private void textToSendContextMnuToolStripMenuItem_Click(object sender, EventArgs e)
        {
            int bytesNum = int.Parse(Regex.Match((sender as ToolStripMenuItem).Text, @"\d+").Value);
            textToSendTxb.Text = GetRandomString(bytesNum);
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {

            if (port != null)
            {
                if (port.IsOpen)
                    port.Close();
                port.Dispose();
            }
        }

        #endregion                                                        
    }
}
