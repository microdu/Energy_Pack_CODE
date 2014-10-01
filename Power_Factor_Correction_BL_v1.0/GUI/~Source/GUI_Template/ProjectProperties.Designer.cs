namespace GUI_Template
{
    partial class ProjectProperties
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ProjectProperties));
            this.btnRefreshComports = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.btnOk = new System.Windows.Forms.Button();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.txtProgramFileName = new System.Windows.Forms.TextBox();
            this.btnProgramFileName = new System.Windows.Forms.Button();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.lblStatus = new System.Windows.Forms.ToolStripStatusLabel();
            this.pnlConnectionProperties = new System.Windows.Forms.Panel();
            this.label3 = new System.Windows.Forms.Label();
            this.panel2 = new System.Windows.Forms.Panel();
            this.btnFindComport = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.chbBootOnConnect = new System.Windows.Forms.CheckBox();
            this.panel4 = new System.Windows.Forms.Panel();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.txtBaudRate = new System.Windows.Forms.TextBox();
            this.cmbComms = new System.Windows.Forms.ComboBox();
            this.statusStrip1.SuspendLayout();
            this.pnlConnectionProperties.SuspendLayout();
            this.panel2.SuspendLayout();
            this.panel4.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnRefreshComports
            // 
            this.btnRefreshComports.BackColor = System.Drawing.Color.Gainsboro;
            this.btnRefreshComports.Location = new System.Drawing.Point(86, 223);
            this.btnRefreshComports.Name = "btnRefreshComports";
            this.btnRefreshComports.Size = new System.Drawing.Size(123, 23);
            this.btnRefreshComports.TabIndex = 0;
            this.btnRefreshComports.Text = "Refresh Comports";
            this.btnRefreshComports.UseVisualStyleBackColor = true;
            this.btnRefreshComports.Click += new System.EventHandler(this.btnRefreshComports_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(17, 69);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(56, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "COM Port:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(17, 43);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(61, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "Baud Rate:";
            // 
            // btnOk
            // 
            this.btnOk.BackColor = System.Drawing.Color.Gainsboro;
            this.btnOk.Location = new System.Drawing.Point(406, 311);
            this.btnOk.Name = "btnOk";
            this.btnOk.Size = new System.Drawing.Size(75, 23);
            this.btnOk.TabIndex = 8;
            this.btnOk.Text = "OK";
            this.btnOk.UseVisualStyleBackColor = true;
            this.btnOk.Click += new System.EventHandler(this.btnOk_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.Filter = ".a00 files|*.a00|All files|*.*";
            this.openFileDialog1.FileOk += new System.ComponentModel.CancelEventHandler(this.openFileDialog1_FileOk);
            // 
            // txtProgramFileName
            // 
            this.txtProgramFileName.Location = new System.Drawing.Point(309, 79);
            this.txtProgramFileName.Name = "txtProgramFileName";
            this.txtProgramFileName.Size = new System.Drawing.Size(130, 20);
            this.txtProgramFileName.TabIndex = 14;
            // 
            // btnProgramFileName
            // 
            this.btnProgramFileName.BackColor = System.Drawing.Color.Gainsboro;
            this.btnProgramFileName.Location = new System.Drawing.Point(445, 78);
            this.btnProgramFileName.Name = "btnProgramFileName";
            this.btnProgramFileName.Size = new System.Drawing.Size(27, 20);
            this.btnProgramFileName.TabIndex = 15;
            this.btnProgramFileName.Text = "...";
            this.btnProgramFileName.UseVisualStyleBackColor = true;
            this.btnProgramFileName.Click += new System.EventHandler(this.btnProgramFileName_Click);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lblStatus});
            this.statusStrip1.Location = new System.Drawing.Point(0, 344);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(492, 22);
            this.statusStrip1.TabIndex = 20;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // lblStatus
            // 
            this.lblStatus.Name = "lblStatus";
            this.lblStatus.Size = new System.Drawing.Size(0, 17);
            // 
            // pnlConnectionProperties
            // 
            this.pnlConnectionProperties.BackColor = System.Drawing.Color.WhiteSmoke;
            this.pnlConnectionProperties.Controls.Add(this.label3);
            this.pnlConnectionProperties.Controls.Add(this.panel2);
            this.pnlConnectionProperties.Controls.Add(this.btnOk);
            this.pnlConnectionProperties.Location = new System.Drawing.Point(3, 4);
            this.pnlConnectionProperties.Name = "pnlConnectionProperties";
            this.pnlConnectionProperties.Size = new System.Drawing.Size(489, 337);
            this.pnlConnectionProperties.TabIndex = 23;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 7.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(9, 316);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(29, 13);
            this.label3.TabIndex = 27;
            this.label3.Text = "V1.0";
            // 
            // panel2
            // 
            this.panel2.BackColor = System.Drawing.Color.White;
            this.panel2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel2.Controls.Add(this.btnFindComport);
            this.panel2.Controls.Add(this.panel1);
            this.panel2.Controls.Add(this.chbBootOnConnect);
            this.panel2.Controls.Add(this.panel4);
            this.panel2.Controls.Add(this.label4);
            this.panel2.Controls.Add(this.label2);
            this.panel2.Controls.Add(this.txtBaudRate);
            this.panel2.Controls.Add(this.txtProgramFileName);
            this.panel2.Controls.Add(this.label1);
            this.panel2.Controls.Add(this.btnProgramFileName);
            this.panel2.Controls.Add(this.btnRefreshComports);
            this.panel2.Controls.Add(this.cmbComms);
            this.panel2.Location = new System.Drawing.Point(3, 3);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(483, 302);
            this.panel2.TabIndex = 26;
            // 
            // btnFindComport
            // 
            this.btnFindComport.BackColor = System.Drawing.Color.Gainsboro;
            this.btnFindComport.Location = new System.Drawing.Point(86, 253);
            this.btnFindComport.Name = "btnFindComport";
            this.btnFindComport.Size = new System.Drawing.Size(122, 23);
            this.btnFindComport.TabIndex = 30;
            this.btnFindComport.Text = "Find Comport";
            this.btnFindComport.UseVisualStyleBackColor = true;
            this.btnFindComport.Click += new System.EventHandler(this.btnFindComport_Click);
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.Color.WhiteSmoke;
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel1.Location = new System.Drawing.Point(235, 27);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(1, 274);
            this.panel1.TabIndex = 29;
            // 
            // chbBootOnConnect
            // 
            this.chbBootOnConnect.AutoSize = true;
            this.chbBootOnConnect.Checked = global::GUI_Template.Properties.Settings.Default.BootOnConnect;
            this.chbBootOnConnect.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chbBootOnConnect.DataBindings.Add(new System.Windows.Forms.Binding("Checked", global::GUI_Template.Properties.Settings.Default, "BootOnConnect", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.chbBootOnConnect.Location = new System.Drawing.Point(253, 42);
            this.chbBootOnConnect.Name = "chbBootOnConnect";
            this.chbBootOnConnect.Size = new System.Drawing.Size(106, 17);
            this.chbBootOnConnect.TabIndex = 28;
            this.chbBootOnConnect.Text = "Boot on Connect";
            this.chbBootOnConnect.UseVisualStyleBackColor = true;
            this.chbBootOnConnect.CheckedChanged += new System.EventHandler(this.chbBootOnConnect_CheckedChanged);
            // 
            // panel4
            // 
            this.panel4.BackColor = System.Drawing.Color.DimGray;
            this.panel4.Controls.Add(this.label5);
            this.panel4.Location = new System.Drawing.Point(5, 5);
            this.panel4.Name = "panel4";
            this.panel4.Size = new System.Drawing.Size(472, 23);
            this.panel4.TabIndex = 27;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.ForeColor = System.Drawing.Color.White;
            this.label5.Location = new System.Drawing.Point(16, 4);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(156, 13);
            this.label5.TabIndex = 24;
            this.label5.Text = "SCI Connection Properties";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(252, 82);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(51, 13);
            this.label4.TabIndex = 23;
            this.label4.Text = "Boot File:";
            // 
            // txtBaudRate
            // 
            this.txtBaudRate.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::GUI_Template.Properties.Settings.Default, "ComBaudRate", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.txtBaudRate.Location = new System.Drawing.Point(86, 40);
            this.txtBaudRate.Name = "txtBaudRate";
            this.txtBaudRate.Size = new System.Drawing.Size(123, 20);
            this.txtBaudRate.TabIndex = 4;
            this.txtBaudRate.Text = global::GUI_Template.Properties.Settings.Default.ComBaudRate;
            this.txtBaudRate.TextChanged += new System.EventHandler(this.txtBaudRate_TextChanged);
            // 
            // cmbComms
            // 
            this.cmbComms.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::GUI_Template.Properties.Settings.Default, "ComPortName", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.cmbComms.DropDownStyle = System.Windows.Forms.ComboBoxStyle.Simple;
            this.cmbComms.Location = new System.Drawing.Point(86, 66);
            this.cmbComms.Name = "cmbComms";
            this.cmbComms.Size = new System.Drawing.Size(123, 151);
            this.cmbComms.Sorted = true;
            this.cmbComms.TabIndex = 6;
            this.cmbComms.Text = global::GUI_Template.Properties.Settings.Default.ComPortName;
            this.cmbComms.SelectedIndexChanged += new System.EventHandler(this.cmbComms_SelectedIndexChanged);
            // 
            // ProjectProperties
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(492, 366);
            this.Controls.Add(this.pnlConnectionProperties);
            this.Controls.Add(this.statusStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "ProjectProperties";
            this.Text = "Setup Connection";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.ProjectProperties_FormClosing);
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.pnlConnectionProperties.ResumeLayout(false);
            this.pnlConnectionProperties.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.panel4.ResumeLayout(false);
            this.panel4.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnRefreshComports;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtBaudRate;
        private System.Windows.Forms.ComboBox cmbComms;
        private System.Windows.Forms.Button btnOk;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.TextBox txtProgramFileName;
        private System.Windows.Forms.Button btnProgramFileName;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.Panel pnlConnectionProperties;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Panel panel4;
        private System.Windows.Forms.CheckBox chbBootOnConnect;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button btnFindComport;
        private System.Windows.Forms.ToolStripStatusLabel lblStatus;
        private System.Windows.Forms.Label label3;
    }
}