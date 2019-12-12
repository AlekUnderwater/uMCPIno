namespace uMCPTest
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.mainToolStrip = new System.Windows.Forms.ToolStrip();
            this.toolStripLabel1 = new System.Windows.Forms.ToolStripLabel();
            this.portNameCbx = new System.Windows.Forms.ToolStripComboBox();
            this.connectBtn = new System.Windows.Forms.ToolStripButton();
            this.startBtn = new System.Windows.Forms.ToolStripButton();
            this.isSelectedByDefaultChb = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.clearHistoryBtn = new System.Windows.Forms.ToolStripButton();
            this.isExtraOutputBtn = new System.Windows.Forms.ToolStripButton();
            this.historyTxb = new System.Windows.Forms.RichTextBox();
            this.textToSendTxb = new System.Windows.Forms.TextBox();
            this.textToSendContextMnu = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.sendRandom8ByteStrToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.sendRandom16ByteStrToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.sendRandom32ByteStrToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.sendRandom64ByteStrToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.sendRandom128ByteStrToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.sendBtn = new System.Windows.Forms.Button();
            this.mainStatusStrip = new System.Windows.Forms.StatusStrip();
            this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
            this.stateLbl = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStripStatusLabel2 = new System.Windows.Forms.ToolStripStatusLabel();
            this.selectLbl = new System.Windows.Forms.ToolStripStatusLabel();
            this.mainToolStrip.SuspendLayout();
            this.textToSendContextMnu.SuspendLayout();
            this.mainStatusStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // mainToolStrip
            // 
            this.mainToolStrip.Font = new System.Drawing.Font("Segoe UI", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.mainToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripLabel1,
            this.portNameCbx,
            this.connectBtn,
            this.startBtn,
            this.isSelectedByDefaultChb,
            this.toolStripSeparator1,
            this.clearHistoryBtn,
            this.isExtraOutputBtn});
            this.mainToolStrip.Location = new System.Drawing.Point(0, 0);
            this.mainToolStrip.Name = "mainToolStrip";
            this.mainToolStrip.Size = new System.Drawing.Size(949, 31);
            this.mainToolStrip.TabIndex = 0;
            this.mainToolStrip.Text = "toolStrip1";
            // 
            // toolStripLabel1
            // 
            this.toolStripLabel1.Name = "toolStripLabel1";
            this.toolStripLabel1.Size = new System.Drawing.Size(51, 28);
            this.toolStripLabel1.Text = "PORT";
            // 
            // portNameCbx
            // 
            this.portNameCbx.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.portNameCbx.Font = new System.Drawing.Font("Segoe UI", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.portNameCbx.Name = "portNameCbx";
            this.portNameCbx.Size = new System.Drawing.Size(121, 31);
            // 
            // connectBtn
            // 
            this.connectBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.connectBtn.Font = new System.Drawing.Font("Segoe UI", 10.2F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.connectBtn.Image = ((System.Drawing.Image)(resources.GetObject("connectBtn.Image")));
            this.connectBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.connectBtn.Name = "connectBtn";
            this.connectBtn.Size = new System.Drawing.Size(94, 28);
            this.connectBtn.Text = "CONNECT";
            this.connectBtn.Click += new System.EventHandler(this.connectBtn_Click);
            // 
            // startBtn
            // 
            this.startBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.startBtn.Font = new System.Drawing.Font("Segoe UI", 10.2F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.startBtn.Image = ((System.Drawing.Image)(resources.GetObject("startBtn.Image")));
            this.startBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.startBtn.Name = "startBtn";
            this.startBtn.Size = new System.Drawing.Size(66, 28);
            this.startBtn.Text = "START";
            this.startBtn.Click += new System.EventHandler(this.startBtn_Click);
            // 
            // isSelectedByDefaultChb
            // 
            this.isSelectedByDefaultChb.Checked = true;
            this.isSelectedByDefaultChb.CheckOnClick = true;
            this.isSelectedByDefaultChb.CheckState = System.Windows.Forms.CheckState.Checked;
            this.isSelectedByDefaultChb.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.isSelectedByDefaultChb.Font = new System.Drawing.Font("Segoe UI", 10.2F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.isSelectedByDefaultChb.Image = ((System.Drawing.Image)(resources.GetObject("isSelectedByDefaultChb.Image")));
            this.isSelectedByDefaultChb.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.isSelectedByDefaultChb.Name = "isSelectedByDefaultChb";
            this.isSelectedByDefaultChb.Size = new System.Drawing.Size(197, 28);
            this.isSelectedByDefaultChb.Text = "SELECTED BY DEFAULT";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 31);
            // 
            // clearHistoryBtn
            // 
            this.clearHistoryBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.clearHistoryBtn.Font = new System.Drawing.Font("Segoe UI", 10.2F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.clearHistoryBtn.Image = ((System.Drawing.Image)(resources.GetObject("clearHistoryBtn.Image")));
            this.clearHistoryBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.clearHistoryBtn.Name = "clearHistoryBtn";
            this.clearHistoryBtn.Size = new System.Drawing.Size(66, 28);
            this.clearHistoryBtn.Text = "CLEAR";
            this.clearHistoryBtn.Click += new System.EventHandler(this.clearHistoryBtn_Click);
            // 
            // isExtraOutputBtn
            // 
            this.isExtraOutputBtn.Checked = true;
            this.isExtraOutputBtn.CheckOnClick = true;
            this.isExtraOutputBtn.CheckState = System.Windows.Forms.CheckState.Checked;
            this.isExtraOutputBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.isExtraOutputBtn.Font = new System.Drawing.Font("Segoe UI", 10.2F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.isExtraOutputBtn.Image = ((System.Drawing.Image)(resources.GetObject("isExtraOutputBtn.Image")));
            this.isExtraOutputBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.isExtraOutputBtn.Name = "isExtraOutputBtn";
            this.isExtraOutputBtn.Size = new System.Drawing.Size(77, 28);
            this.isExtraOutputBtn.Text = "EXTRAS";
            this.isExtraOutputBtn.Click += new System.EventHandler(this.isExtraOutputBtn_Click);
            // 
            // historyTxb
            // 
            this.historyTxb.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.historyTxb.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.historyTxb.Location = new System.Drawing.Point(12, 42);
            this.historyTxb.Name = "historyTxb";
            this.historyTxb.ReadOnly = true;
            this.historyTxb.Size = new System.Drawing.Size(925, 487);
            this.historyTxb.TabIndex = 1;
            this.historyTxb.Text = "";
            this.historyTxb.TextChanged += new System.EventHandler(this.historyTxb_TextChanged);
            // 
            // textToSendTxb
            // 
            this.textToSendTxb.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textToSendTxb.ContextMenuStrip = this.textToSendContextMnu;
            this.textToSendTxb.Font = new System.Drawing.Font("Segoe UI", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.textToSendTxb.Location = new System.Drawing.Point(12, 536);
            this.textToSendTxb.Name = "textToSendTxb";
            this.textToSendTxb.Size = new System.Drawing.Size(844, 30);
            this.textToSendTxb.TabIndex = 2;
            this.textToSendTxb.TextChanged += new System.EventHandler(this.textToSendTxb_TextChanged);
            this.textToSendTxb.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.textToSendTxb_PreviewKeyDown);
            // 
            // textToSendContextMnu
            // 
            this.textToSendContextMnu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.sendRandom8ByteStrToolStripMenuItem,
            this.sendRandom16ByteStrToolStripMenuItem,
            this.sendRandom32ByteStrToolStripMenuItem,
            this.sendRandom64ByteStrToolStripMenuItem,
            this.sendRandom128ByteStrToolStripMenuItem});
            this.textToSendContextMnu.Name = "textToSendContextMnu";
            this.textToSendContextMnu.Size = new System.Drawing.Size(235, 146);
            // 
            // sendRandom8ByteStrToolStripMenuItem
            // 
            this.sendRandom8ByteStrToolStripMenuItem.Name = "sendRandom8ByteStrToolStripMenuItem";
            this.sendRandom8ByteStrToolStripMenuItem.Size = new System.Drawing.Size(234, 24);
            this.sendRandom8ByteStrToolStripMenuItem.Text = "Send random 8 bytes";
            this.sendRandom8ByteStrToolStripMenuItem.Click += new System.EventHandler(this.textToSendContextMnuToolStripMenuItem_Click);
            // 
            // sendRandom16ByteStrToolStripMenuItem
            // 
            this.sendRandom16ByteStrToolStripMenuItem.Name = "sendRandom16ByteStrToolStripMenuItem";
            this.sendRandom16ByteStrToolStripMenuItem.Size = new System.Drawing.Size(234, 24);
            this.sendRandom16ByteStrToolStripMenuItem.Text = "Send random 16 bytes";
            this.sendRandom16ByteStrToolStripMenuItem.Click += new System.EventHandler(this.textToSendContextMnuToolStripMenuItem_Click);
            // 
            // sendRandom32ByteStrToolStripMenuItem
            // 
            this.sendRandom32ByteStrToolStripMenuItem.Name = "sendRandom32ByteStrToolStripMenuItem";
            this.sendRandom32ByteStrToolStripMenuItem.Size = new System.Drawing.Size(234, 24);
            this.sendRandom32ByteStrToolStripMenuItem.Text = "Send random 32 bytes";
            this.sendRandom32ByteStrToolStripMenuItem.Click += new System.EventHandler(this.textToSendContextMnuToolStripMenuItem_Click);
            // 
            // sendRandom64ByteStrToolStripMenuItem
            // 
            this.sendRandom64ByteStrToolStripMenuItem.Name = "sendRandom64ByteStrToolStripMenuItem";
            this.sendRandom64ByteStrToolStripMenuItem.Size = new System.Drawing.Size(234, 24);
            this.sendRandom64ByteStrToolStripMenuItem.Text = "Send random 64 bytes";
            this.sendRandom64ByteStrToolStripMenuItem.Click += new System.EventHandler(this.textToSendContextMnuToolStripMenuItem_Click);
            // 
            // sendRandom128ByteStrToolStripMenuItem
            // 
            this.sendRandom128ByteStrToolStripMenuItem.Name = "sendRandom128ByteStrToolStripMenuItem";
            this.sendRandom128ByteStrToolStripMenuItem.Size = new System.Drawing.Size(234, 24);
            this.sendRandom128ByteStrToolStripMenuItem.Text = "Send random 128 bytes";
            this.sendRandom128ByteStrToolStripMenuItem.Click += new System.EventHandler(this.textToSendContextMnuToolStripMenuItem_Click);
            // 
            // sendBtn
            // 
            this.sendBtn.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.sendBtn.Font = new System.Drawing.Font("Segoe UI", 10.2F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.sendBtn.Location = new System.Drawing.Point(862, 535);
            this.sendBtn.Name = "sendBtn";
            this.sendBtn.Size = new System.Drawing.Size(75, 30);
            this.sendBtn.TabIndex = 3;
            this.sendBtn.Text = "SEND";
            this.sendBtn.UseVisualStyleBackColor = true;
            this.sendBtn.Click += new System.EventHandler(this.sendBtn_Click);
            // 
            // mainStatusStrip
            // 
            this.mainStatusStrip.Font = new System.Drawing.Font("Segoe UI", 10.2F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.mainStatusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel1,
            this.stateLbl,
            this.toolStripStatusLabel2,
            this.selectLbl});
            this.mainStatusStrip.Location = new System.Drawing.Point(0, 572);
            this.mainStatusStrip.Name = "mainStatusStrip";
            this.mainStatusStrip.Size = new System.Drawing.Size(949, 28);
            this.mainStatusStrip.TabIndex = 4;
            this.mainStatusStrip.Text = "statusStrip1";
            // 
            // toolStripStatusLabel1
            // 
            this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
            this.toolStripStatusLabel1.Size = new System.Drawing.Size(64, 23);
            this.toolStripStatusLabel1.Text = "STATE:";
            // 
            // stateLbl
            // 
            this.stateLbl.Name = "stateLbl";
            this.stateLbl.Size = new System.Drawing.Size(75, 23);
            this.stateLbl.Text = "HALTED";
            // 
            // toolStripStatusLabel2
            // 
            this.toolStripStatusLabel2.Name = "toolStripStatusLabel2";
            this.toolStripStatusLabel2.Size = new System.Drawing.Size(73, 23);
            this.toolStripStatusLabel2.Text = "SELECT:";
            // 
            // selectLbl
            // 
            this.selectLbl.Name = "selectLbl";
            this.selectLbl.Size = new System.Drawing.Size(45, 23);
            this.selectLbl.Text = "True";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(949, 600);
            this.Controls.Add(this.mainStatusStrip);
            this.Controls.Add(this.sendBtn);
            this.Controls.Add(this.textToSendTxb);
            this.Controls.Add(this.historyTxb);
            this.Controls.Add(this.mainToolStrip);
            this.Name = "MainForm";
            this.Text = "uMCP Chat";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.MainForm_FormClosed);
            this.mainToolStrip.ResumeLayout(false);
            this.mainToolStrip.PerformLayout();
            this.textToSendContextMnu.ResumeLayout(false);
            this.mainStatusStrip.ResumeLayout(false);
            this.mainStatusStrip.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStrip mainToolStrip;
        private System.Windows.Forms.ToolStripLabel toolStripLabel1;
        private System.Windows.Forms.ToolStripComboBox portNameCbx;
        private System.Windows.Forms.ToolStripButton connectBtn;
        private System.Windows.Forms.ToolStripButton startBtn;
        private System.Windows.Forms.RichTextBox historyTxb;
        private System.Windows.Forms.TextBox textToSendTxb;
        private System.Windows.Forms.Button sendBtn;
        private System.Windows.Forms.ToolStripButton isSelectedByDefaultChb;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton clearHistoryBtn;
        private System.Windows.Forms.ToolStripButton isExtraOutputBtn;
        private System.Windows.Forms.StatusStrip mainStatusStrip;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
        private System.Windows.Forms.ToolStripStatusLabel stateLbl;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel2;
        private System.Windows.Forms.ToolStripStatusLabel selectLbl;
        private System.Windows.Forms.ContextMenuStrip textToSendContextMnu;
        private System.Windows.Forms.ToolStripMenuItem sendRandom8ByteStrToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem sendRandom16ByteStrToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem sendRandom32ByteStrToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem sendRandom64ByteStrToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem sendRandom128ByteStrToolStripMenuItem;

    }
}

