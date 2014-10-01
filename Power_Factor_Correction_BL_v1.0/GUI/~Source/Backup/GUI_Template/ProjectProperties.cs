using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.IO;
using System.Text.RegularExpressions;

namespace GUI_Template
{
    public partial class ProjectProperties : Form
    {
        GuiStartUp parentForm;
        CommsManager commsMngr;
        Regex HasInvalidChars;
        Regex IsNotEmpty;
        int numPorts;
        int findComportState;
        System.Collections.Specialized.StringCollection comportEchoWhenOff;

        public string PortName
        {
            get 
            {
                if (cmbComms.SelectedIndex != -1)
                {
                    return cmbComms.SelectedItem.ToString();
                }
                else return null;
            }
        }
        public string HexPath
        {
            get { return txtProgramFileName.Text; }
            set { txtProgramFileName.Text = value; }
        }
            

        public ProjectProperties(GuiStartUp parent, CommsManager comMngr)
        {
            InitializeComponent();

            comportEchoWhenOff = new System.Collections.Specialized.StringCollection();

            //**Override Designer Settings
            openFileDialog1.FileName = Properties.FileLocationSettings.Default.A00_Location;
            txtProgramFileName.DataBindings.Add(new System.Windows.Forms.Binding("Text", Properties.FileLocationSettings.Default, "A00_Location", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.txtProgramFileName.Text = Properties.FileLocationSettings.Default.A00_Location;
            //**

            parentForm = parent;
            //hexConverter = new HexConverter(txtProgramFileName);
            HasInvalidChars = new Regex("[^0-9]");
            IsNotEmpty = new Regex("[0-9]");
            txtBaudRate.Text = comMngr.BaudRate.ToString();
            txtBaudRate.ForeColor = SystemColors.ControlText;
            cmbComms.Items.Clear();
            numPorts = 0;
            commsMngr = comMngr;

            foreach (string s in SerialPort.GetPortNames())
            {
                cmbComms.Items.Add(s);
                if (s == comMngr.PortName)
                {
                    cmbComms.SelectedItem = s;
                }
                numPorts++;
            }
        }

        public void SetDefault()
        {
            commsMngr.BaudRate = Convert.ToInt32(txtBaudRate.Text);
            commsMngr.PortName = cmbComms.SelectedItem.ToString();
        }

        public override void Refresh()
        {
            findComportState = 0;
            lblStatus.Text = "";
            btnFindComport.Text = "Find Comport";
        }

        private void txtBaudRate_TextChanged(object sender, EventArgs e)
        {
            if (IsNotEmpty.IsMatch(txtBaudRate.Text) == true && HasInvalidChars.IsMatch(txtBaudRate.Text) == false)
            {
                txtBaudRate.ForeColor = System.Drawing.Color.Green;
            }
            else
            {
                txtBaudRate.ForeColor = System.Drawing.Color.Red;
            }
        }

        private void cmbComms_SelectedIndexChanged(object sender, EventArgs e)
        {
            cmbComms.ForeColor = SystemColors.ControlText;
        }

        private void btnRefreshComports_Click(object sender, EventArgs e)
        {
            cmbComms.Items.Clear();
            numPorts = 0;
            foreach (string s in SerialPort.GetPortNames())
            {
                cmbComms.Items.Add(s);
                if (s == commsMngr.PortName)
                {
                    cmbComms.SelectedItem = s;
                }
                numPorts++;
            }
        }

        private void btnFindComport_Click(object sender, EventArgs e)
        {
            switch (findComportState)
            {
                case 0:                  
                    btnFindComport.Text = "Continue";
                    findComportState++;
                    btnRefreshComports_Click(this, new EventArgs());
                    lblStatus.Text = "Please Turn EVM Board Off then Press \"Continue\"";

                    comportEchoWhenOff.Clear();
                    break;
                case 1:
                    lblStatus.Text = "Testing...";
                    btnFindComport.Enabled = false;
                    Application.DoEvents();
                    string tempPortName = commsMngr.PortName;
                    int tempBaudRate = commsMngr.BaudRate;
                    for (int i = 0; i < cmbComms.Items.Count; i++)
                    {

                        commsMngr.Close();

                        commsMngr.PortName = cmbComms.Items[i].ToString();
                        commsMngr.BaudRate = Convert.ToInt32(txtBaudRate.Text);

                        lblStatus.Text = "Trying Communications on: " + cmbComms.Items[i].ToString();
                        Application.DoEvents();

                        bool openSucceeded = commsMngr.Open();

                        if (openSucceeded == true)
                        {
                            commsMngr.WriteString("A");
                            byte readByte = commsMngr.ReadByte();

                            if (readByte != 0)
                            {
                                comportEchoWhenOff.Add(commsMngr.PortName);
                            }
                            commsMngr.Close();
                        }
                    }

                    commsMngr.PortName = tempPortName;
                    commsMngr.BaudRate = tempBaudRate;
                    findComportState++;
                    btnFindComport.Enabled = true;
                    lblStatus.Text = "Please Turn EVM Board On then Press \"Continue\"";

                    break;

                case 2:
                    lblStatus.Text = "Testing...";
                    btnFindComport.Enabled = false;
                    Application.DoEvents();
                    tempPortName = commsMngr.PortName;
                    tempBaudRate = commsMngr.BaudRate;
                    System.Collections.Specialized.StringCollection comportProper = new System.Collections.Specialized.StringCollection();

                    for (int i = 0; i < cmbComms.Items.Count; i++)
                    {


                        commsMngr.PortName = cmbComms.Items[i].ToString();
                        commsMngr.BaudRate = Convert.ToInt32(txtBaudRate.Text);

                        lblStatus.Text = "Trying Communications on: " + cmbComms.Items[i].ToString();
                        Application.DoEvents();

                        bool openSucceeded = commsMngr.Open();

                        if(openSucceeded == true)
                        {
                        commsMngr.WriteString("A");
                        byte readByte = commsMngr.ReadByte();

                        if (readByte != 0)
                        {
                            if (comportEchoWhenOff.Contains(commsMngr.PortName) == false)
                            {
                                comportProper.Add(commsMngr.PortName);
                            }
                        }
                        commsMngr.Close();
                        }
                    }

                    if (comportProper.Count != 1)
                    {
                        lblStatus.Text = "Please Ensure SCI Boot Jumper is Placed or that the Target is Flashed Then Try Again";
                    }
                    else
                    {
                        Application.DoEvents();
                        cmbComms.SelectedItem = comportProper[0];
                        lblStatus.Text = "Comport found: " + comportProper[0];
                        Application.DoEvents();
                    }

                    commsMngr.PortName = tempPortName;
                    commsMngr.BaudRate = tempBaudRate;
                    findComportState = 0;
                    btnFindComport.Enabled = true;
                    btnFindComport.Text = "Find Comport";
                    break;
            }
        }

        private void chbBootOnConnect_CheckedChanged(object sender, EventArgs e)
        {
            if (chbBootOnConnect.Checked == true)
            {
                txtProgramFileName.Enabled = true;
                btnProgramFileName.Enabled = true;
            }
            else
            {
                txtProgramFileName.Enabled = false;
                btnProgramFileName.Enabled = false;
            }
        }

        private void btnProgramFileName_Click(object sender, EventArgs e)
        {
            openFileDialog1.ShowDialog();
        }

        private void openFileDialog1_FileOk(object sender, CancelEventArgs e)
        {
            txtProgramFileName.Text = openFileDialog1.FileName;
        }

        private void btnOk_Click(object sender, EventArgs e)
        {
            if (commsMngr.IsOpen)
            {
                parentForm.btnConnect_Click(this, new EventArgs());
            }
            if (IsNotEmpty.IsMatch(txtBaudRate.Text) == true && HasInvalidChars.IsMatch(txtBaudRate.Text) == false)
            {
                txtBaudRate.ForeColor = SystemColors.ControlText;
                if (cmbComms.SelectedIndex != -1)
                {
                    Hide();
                    commsMngr.BaudRate = Convert.ToInt32(txtBaudRate.Text);
                    commsMngr.PortName = cmbComms.SelectedItem.ToString();
                }
                else
                {
                    cmbComms.ForeColor = System.Drawing.Color.Red;
                }
            }
            //lblStatus.Text = "";
            Properties.FileLocationSettings.Default.Save();
            Properties.Settings.Default.Save();

            //parentForm.btnConnect_Click(this, new EventArgs());
            //cmbComms.SelectedIndex = cmbComms.Items.IndexOf(commsMngr.PortName);           
        }

        private void ProjectProperties_FormClosing(object sender, FormClosingEventArgs e)
        {
            Hide();
            Properties.FileLocationSettings.Default.Save();
            e.Cancel = true;
        }

        //public void ChangeA00File(string newFileName)
        //{
        //    txtProgramFileName.Text = newFileName;
        //}

    }
}