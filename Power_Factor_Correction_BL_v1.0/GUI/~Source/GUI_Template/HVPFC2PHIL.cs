using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Text.RegularExpressions;

namespace GUI_Template
{
    public partial class GuiStartUp : Form
    {
        //---global variables---
        GuiSetBtn[] btnGroup;
        GuiSetTxt[] txtGroup;
        GuiSetSldr[] sldrGroup;

        GuiGetVar[] varGroup;
        GuiGetArray[] arrayGroup;
        public GuiGraphTSArray[] TSGraphGroup;
        GuiGetMemory[] memoryGroup;

        bool autoUpdateOn = false;
        bool graphOn = false;
        bool powerCycled = false;

        //GraphForm grphForm;
        ProjectProperties propsForm;

        public CommsManager commsMngr;

        int guiSetBtnSize = 1;
        int guiSetTxtSize = 9;//1;
        int guiSetSldrSize = 6;//9;

        int guiGetVarSize = 4;//2;//4
        int guiGetArraySize = 1;
        int guiGetTSGraphSize = 0;//2;
        int guiGetMemorySize = 0;

        //---initialization---
        public GuiStartUp()
        {      
            InitializeComponent();
            EnableCtrls(false);

            commsMngr = new CommsManager(this);

            //grphForm = new GraphForm(this);
            propsForm = new ProjectProperties(this,commsMngr);

            commsMngr.Close();

            MainTimer.Enabled = true;
            MainTimer.Stop();
            //GraphTimer.Enabled = true;
            //GraphTimer.Stop();

            graphOn = true;
            

            btnGroup = new GuiSetBtn[guiSetBtnSize];
            txtGroup = new GuiSetTxt[guiSetTxtSize];
            sldrGroup = new GuiSetSldr[guiSetSldrSize];

            varGroup = new GuiGetVar[guiGetVarSize];
            arrayGroup = new GuiGetArray[guiGetArraySize];
            TSGraphGroup = new GuiGraphTSArray[guiGetTSGraphSize];
            memoryGroup = new GuiGetMemory[guiGetMemorySize];

            //---Set Controls---
            //btns
            //***(btnGroup[0] used to ClearFaultFlag)
            //btnGroup[0] = new GuiSetBtn(btnClearFaultFlg, 0x00);      //do not use
            btnGroup[0] = new GuiSetBtn(btn2P2Zselect, 0x00);
            //btnGroup[1] = new GuiSetBtn(btnCntl_Calc, 0x01);
            //txts
            txtGroup[0] = new GuiSetTxt(txtVbus_set, btnPFC, 6, 0x00);
      //      txtGroup[1] = new GuiSetTxt(txtItrip, btnItrip, 15, 0x01);
        //    txtGroup[2] = new GuiSetTxt(txtComp_mode, 0x02);   
 
            //sldrs
       /*     sldrGroup[0] = new GuiSetSldr(sldrVfb, txtVfbSet, lblVfbMin, lblVfbMax, 0x00, 0); //Comp Ampltd
            sldrGroup[1] = new GuiSetSldr(sldrShoulder, txtShoulder, lblShoulderMin, lblShoulderMax, 0x01, 0); //Shoulder
            sldrGroup[2] = new GuiSetSldr(sldrdlog, txtdlogSet, lbldlogmin, lbldlogmax, 0x02, 0); //Dlog Trigger
            */
            sldrGroup[0] = new GuiSetSldr(sldrPgain_I, txtPgain_I, lblPgain_IMin, lblPgain_IMax, 0x00, 0); //Shoulder
            sldrGroup[1] = new GuiSetSldr(sldrIgain_I, txtIgain_I, lblIgain_IMin, lblIgain_IMax, 0x01, 0); //Dlog Trigger
            sldrGroup[2] = new GuiSetSldr(sldrDgain_I, txtDgain_I, lblDgain_IMin, lblDgain_IMax, 0x02, 0); //Shoulder
  
            sldrGroup[3] = new GuiSetSldr(sldrPgain_V, txtPgain_V, lblPgain_VMin, lblPgain_VMax, 0x03, 0); //Dlog Trigger
            sldrGroup[4] = new GuiSetSldr(sldrIgain_V, txtIgain_V, lblIgain_VMin, lblIgain_VMax, 0x04, 0); //Shoulder
            sldrGroup[5] = new GuiSetSldr(sldrDgain_V, txtDgain_V, lblDgain_VMin, lblDgain_VMax, 0x05, 0); //Shoulder

            //---Get Controls---
            //vars
            varGroup[0] = new GuiGetVar(txtVbus, 0x00, 6);
            varGroup[1] = new GuiGetVar(txtVrectAvg, 0x01, 6);
//            varGroup[1] = new GuiGetVar(txtIpfc, 0x01, 0);
 //           varGroup[2] = new GuiGetVar(txtVline, 0x02, 0);
            varGroup[2] = new GuiGetVar(txtVrms, 0x02, 6);
    //        varGroup[3] = new GuiGetVar(txtVN, 0x03, 0);
            varGroup[3] = new GuiGetVar(txtFrequency, 0x03, 6);
          /*  varGroup[4] = new GuiGetVar(txtVpfcOut, 0x04, 6);
            varGroup[5] = new GuiGetVar(txtIfb, 0x05, 0);    // Comp Ampltd
            varGroup[6] = new GuiGetVar(txtVfbOut, 0x06, 0); // Shoulder
            varGroup[7] = new GuiGetVar(txtFaultFlg, 0x07, 0);
            varGroup[8] = new GuiGetVar(txtInput_Pwr, 0x08, 2);
            */
            //arrays
            //arrayGroup[0] = new GuiGetArray(new TextBox[2] {txtVGetCH1, txtVGetCH2}, 0x00, 12);

            //Time-sequenced graphs (block of data from target)
          // TSGraphGroup[0] = new GuiGraphTSArray(25, 0x00, pnlIpcTotal, lblIpfcTotalASMin, lblIpfcTotalASMax, 11, chbAutoScaleIpfc);
            //TSGraphGroup[0] = new GuiGraphTSArray(25, 0x00, pnlVbusTotal, lblVbusTotalASMin, lblVbusTotalASMax, 11, chbAutoScaleVbus);
          // TSGraphGroup[1] = new GuiGraphTSArray(25, 0x01, pnlVacRect, lblVacTotalASMin, lblVacTotalASMax, 6, chbAutoScaleVac);
          //  TSGraphGroup[0] = new GuiGraphTSArray(128, 0x00, pnlIpcTotal, lblIpfcTotalASMin, lblIpfcTotalASMax, 11, chbAutoScaleIpfc);
          //  TSGraphGroup[1] = new GuiGraphTSArray(128, 0x01, pnlVacRect, lblVacTotalASMin, lblVacTotalASMax, 6, chbAutoScaleVac);

            //memory gets
            //memoryGroup[0] = new GuiGetMemory(textBox1, textBox2, 0x00);


            #region Initialize common components among the ctrl groups (ex. commsMngr reference)

            for (int i = 0; i < guiSetBtnSize; i++)
            {
                if (btnGroup[i] != null)
                {
                    btnGroup[i].commsMngr = commsMngr;
                }
            }

            for (int i = 0; i < guiSetTxtSize; i++)
            {
                if (txtGroup[i] != null)
                {
                    txtGroup[i].commsMngr = commsMngr;
                }
            }

            for (int i = 0; i < guiSetSldrSize; i++)
            {
                if (sldrGroup[i] != null)
                {
                    sldrGroup[i].commsMngr = commsMngr;
                }
            }

            for (int i = 0; i < guiGetVarSize; i++)
            {
                if (varGroup[i] != null)
                {
                    varGroup[i].commsMngr = commsMngr;
                }
            }
            for (int i = 0; i < guiGetArraySize; i++)
            {
                if (arrayGroup[i] != null)
                {
                    arrayGroup[i].commsMngr = commsMngr;
                }
            }
            for (int i = 0; i < guiGetTSGraphSize; i++)
            {
                if (TSGraphGroup[i] != null)
                {
                    TSGraphGroup[i].commsMngr = commsMngr;
                }
            }
            for (int i = 0; i < guiGetMemorySize; i++)
            {
                if (memoryGroup[i] != null)
                {
                    memoryGroup[i].commsMngr = commsMngr;
                }
            }

            #endregion
        }


        //----------------------------------------------------------
        #region Main Form Event Handlers and Timers
        //----------------------------------------------------------

        //---Connect to the target via SCI and change displays to show this---
        public void Connect()
        {
            if (commsMngr.SciConnect() == false)
            {
                pnlConnect.BackColor = System.Drawing.Color.Red;
                lblStatus.Text = "Could Not Connect:  Please Check Connections";
                btnConnect.Text = "Connect";
            }
            else
            {
                #region Connected Successfully
                commsMngr.ClearCommands();

                EnableCtrls(true);

                commsMngr.ptrWriteAt = 0;
                commsMngr.ptrWorkingAt = 0;
                commsMngr.isReceiving = false;

                //Set Gui Controls to default settings
                Properties.Settings.Default.Reload();
                //SetDefault();

                lblStatus.Text = "Connected";
                pnlConnect.BackColor = System.Drawing.Color.Green;

                MainTimer.Start();
                
                btnConnect.Text = "Disconnect";
                cmbMainUpdateRate_SelectedIndexChanged(this, new EventArgs());
                #endregion
            }
        /*    if (graphOn == true)        //restart graph if it is still open
            {
                if (cmbGraphUpdateRate.SelectedItem != null)
                {
                    GraphTimer.Interval = (int)(Convert.ToDouble(cmbGraphUpdateRate.SelectedItem.ToString()) * 1000);
                    GraphTimer.Start();
                }
            }*/
        }


        //---Boot to the target and display status/errors---
        public void Boot()
        {
            string portName = propsForm.PortName;
            string hexPath = propsForm.HexPath;

            if (portName == null)
            {
                lblStatus.Text = "Error: COM Port is Invalid.";
                pnlConnect.BackColor = System.Drawing.Color.Red;
            }
            else if (hexPath.Length < 6)
            {
                lblStatus.Text = "Error: Please load a valid .a00 file";
                pnlConnect.BackColor = System.Drawing.Color.Red;
            }
            else if (hexPath.Substring(hexPath.Length - 4) != ".a00")
            {
                lblStatus.Text = "Error: Please load a valid .a00 file";
                pnlConnect.BackColor = System.Drawing.Color.Red;
            }
            else if (powerCycled == true)
            {
                lblStatus.Text = "Loading Program...";
                Application.DoEvents();
                lblStatus.Text = commsMngr.LoadProgramFromFile(portName, hexPath, prbConnectStatus);
                if (lblStatus.Text.StartsWith("Error")) pnlConnect.BackColor = System.Drawing.Color.Crimson;
                else pnlConnect.BackColor = System.Drawing.Color.SeaGreen;
                powerCycled = false;
            }
            else
            {
                lblStatus.Text = "Please Power Cycle Board then Push \"Connect\"";
                lblStatus.ForeColor = System.Drawing.Color.Black;
                Application.DoEvents();
                powerCycled = true;
            }
        }


        //---Disconnect from the target and change displays to show this---
        public void Disconnect()
        {
            if (commsMngr.comValid == true && commsMngr.IsOpen == true)
            {
              //  txtVfbSet.Text = "0.00";
               // txtVbus_set.Text = "0.00";
               //sldrGroup[0].SetDefault();
               //txtGroup[0].SetDefault();

                autoUpdateOn = false;       //stop automatically getting variables from target 
                MainTimer.Stop();
               // GraphTimer.Stop();


                //Allow SerialPort to finish backlogged commands
                for (int i = 0; commsMngr.ptrWorkingAt != commsMngr.ptrWriteAt; i++)
                {
                    Application.DoEvents();                 //allow main thread to update GUI items (textboxes, etc)
                    System.Threading.Thread.Sleep(20);      // 50 ms
                    if (i > 100)
                    {
                        commsMngr.isReceiving = false;
                        commsMngr.TryNewCommsTask();
                    }
                }
                Application.DoEvents();
            }

            
            commsMngr.Close();
            btnConnect.Text = "Connect";
            lblStatus.Text = "Disconnected";
            pnlConnect.BackColor = System.Drawing.Color.Red;
            EnableCtrls(false);
        }


        //---Lost Connection so Disconnect and warn user---
        public void connectionLost()
        {
            MainTimer.Stop();
            commsMngr.Close();
            commsMngr.ptrWorkingAt = commsMngr.ptrWriteAt;
            btnConnect.Text = "Connect";
            lblStatus.Text = "Connection Lost : Board May Not Be Properly Turned Off";
            pnlConnect.BackColor = System.Drawing.Color.Yellow;
            EnableCtrls(false);
        }


        //---Choose whether to disconnect, boot or connect then do this---
        public void btnConnect_Click(object sender, EventArgs e)
        {
            pnlConnect.BackColor = System.Drawing.Color.Yellow;
            //comErrorFound = false;

            if (commsMngr.IsOpen == true)
            {
                Disconnect();
            }
            else
            {
                //Connect Sequence               
                bool bootOnConnect = Properties.Settings.Default.BootOnConnect;

                if (bootOnConnect == true)
                {
                    Boot();
                }

                if (bootOnConnect == false || (lblStatus.Text.StartsWith("Error") == false && lblStatus.Text.StartsWith("Please") == false))
                {
                    Connect();
                }
            }
        }


        //---Enables and Disables Controls on the TabPages
        private void EnableCtrls(bool status)
        {
           // int showedTabpageIndex;
            foreach (Control ctrl in pnlMain.Controls)
            {
                ctrl.Enabled = status;
            }

            //showedTabpageIndex = pnlMain.SelectedIndex;
            //if (status == true)
            //{
                //foreach (TabPage tabpage in tabControl1.TabPages)
                //{
                //    tabpage.Show();
                //}
                //tabControl1.SelectedIndex = 0;
            //}
        }


        //---Get variables/arrays/data from host---
        private void GetData()
        {
            if (varGroup[0] != null)
            {
                for (int i = 0; i < varGroup.Length; i++)
                {
                    varGroup[i].RequestBuffer();
                }
            }

            if (arrayGroup[0] != null)
            {
                for (int i = 0; i < arrayGroup.Length; i++)
                {
                    arrayGroup[i].RequestBuffer();
                }
            }
        }


        //---(After [interval] ms this event triggers)---
        //---Blink LED and update the Get Group if autoUpdateOn is enabled---
        private void MainTimer_Tick(object sender, EventArgs e)
        {
            //toggle an LED
            //NewSetTask(ctrl who sent cmd, cmd#, item#, data)
            commsMngr.NewSetTask(null, 0x00, 0x00, 2);

            commsMngr.TryNewCommsTask();

            if (autoUpdateOn == true)
            {
                //update the "get" group
                GetData();
            }
            //lblStatus.Text = lblStatus.Text + "Tick";    //debug
        }


        //---Change update rate when user changes the value---
        private void cmbMainUpdateRate_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (cmbMainUpdateRate.SelectedIndex == -1)
            {
                cmbMainUpdateRate.SelectedIndex = 0;
            }
            else if (cmbMainUpdateRate.SelectedIndex == 0)
            {
                autoUpdateOn = false;
                MainTimer.Interval = 1000;   // time between update (in ms)
            }
            else
            {
                MainTimer.Interval = (int)(Convert.ToDouble(cmbMainUpdateRate.SelectedItem.ToString()) * 1000);
                autoUpdateOn = true;
            }
        }


        //---Sets all 'Set' Controls to their default state
        private void SetDefault()
        {
            int defaultValIndex = cmbMainUpdateRate.Items.IndexOf(Properties.Settings.Default.VariableUpdateInterval);
            if (defaultValIndex != -1)
            {
                cmbMainUpdateRate.SelectedIndex = defaultValIndex;
            }
            else cmbMainUpdateRate.SelectedIndex = 0;
            //defaultValIndex = cmbGraphUpdateRate.Items.IndexOf(Properties.Settings.Default.GraphUpdateInterval);
            //if (defaultValIndex != -1)
            //{
            //    cmbGraphUpdateRate.SelectedIndex = defaultValIndex;
            //}
            //else cmbGraphUpdateRate.SelectedIndex = 0;

            for (int i = 0; i < guiSetBtnSize; i++)
            {
                if (btnGroup[i] != null)
                {
                    btnGroup[i].SetDefault();
                }
            }

            for (int i = 0; i < guiSetTxtSize; i++)
            {
                if (txtGroup[i] != null)
                {
                    txtGroup[i].SetDefault();
                }
            }

            for (int i = 0; i < guiSetSldrSize; i++)
            {
                if (sldrGroup[i] != null)
                {
                    sldrGroup[i].SetDefault();
                }
            }
        }


        //---Restore Old Settings (internal storage or the settings stored in "[Project].exe.config")
        private void btnResetDefaults_Click(object sender, EventArgs e)
        {
            Properties.Settings.Default.Reload();

            if (commsMngr.IsOpen == true)
            {
                //SetDefault();
            }
            else
            {
                propsForm.SetDefault();
            }
        }


        //---Save new Settings to Internal Storage
        private void btnSaveDefaults_Click(object sender, EventArgs e)
        {
            if (cmbMainUpdateRate.SelectedItem != null)
            {
                Properties.Settings.Default.VariableUpdateInterval = cmbMainUpdateRate.SelectedItem.ToString();
            }
            Properties.Settings.Default.Save();
        }


        //---Close sci and peripheral forms in case user closes form---
        private void TwoChannelBuck_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (btnConnect.Text == "Disconnect")
            {
                Disconnect();
            }
            propsForm.Close();
            //grphForm.Close();
            commsMngr.Close();
        }


        //***************************************
        #region Graph and Graph Timer code

        //Show the GraphForm and start the graph's timer
        //public void btnShowGraph_Click(object sender, EventArgs e)
        //{
        //    if (btnShowGraph.Text == "Show Graph")
        //    {
        //        grphForm.Show();
        //        btnShowGraph.Text = "Hide Graph";
        //        graphOn = true;
        //        GraphTimer.Interval = (int)(Convert.ToDouble(cmbGraphUpdateRate.SelectedItem.ToString()) * 1000);
        //        GraphTimer.Start();
        //    }
        //    else
        //    {
        //        GraphTimer.Stop();
        //        grphForm.TopMost = false;
        //        grphForm.Hide();
        //        btnShowGraph.Text = "Show Graph";
        //        graphOn = false;
        //    }
        //}


        //Call GetGraphData each time the graph timer ticks
        private void GraphTimer_Tick(object sender, EventArgs e)
        {
            if (graphOn == true && commsMngr.IsOpen == true)
            {
                GetGraphData();
            }
        }


        //Request Data Buffer from the target
        private void GetGraphData()
        {
            //***Remove if using Graphs to save bandwidth
            if (TSGraphGroup[0] != null)
            {
                for (int i = 0; i < TSGraphGroup.Length; i++)
                {
                    TSGraphGroup[i].RequestBuffer();
                }
            }
        }


        //Change the graph's update rate when the user changes it
/*        private void cmbGraphUpdateRate_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (cmbGraphUpdateRate.SelectedIndex == -1)
            {
                cmbGraphUpdateRate.SelectedIndex = 0;
            }
            else
            {
                if (cmbGraphUpdateRate.SelectedItem.ToString() == "---")
                {
                    graphOn = false;
                }
                else
                {
                    GraphTimer.Interval = (int)(Convert.ToDouble(cmbGraphUpdateRate.SelectedItem.ToString()) * 1000);
                    graphOn = true;
                }
            }
        }*/


        #endregion
        //***************************************

        //Begin ProjectProperties Form
        private void btnSetupConnection_Click(object sender, EventArgs e)
        {
            Disconnect();
            Properties.Settings.Default.Reload();
            propsForm.Refresh();
            powerCycled = false;
            propsForm.Show();
        }


        #endregion

        //----------------------------------------------------------

        #region Individual Event Handlers
        //---------------------------------------------------------

        #region Button Set Event Handlers

        private void GenericEventHandler_SetBtn_BtnClick(object sender, EventArgs e)
        {
            int temp = (int)((Control)sender).Tag;
            btnGroup[temp].SetButton();
            commsMngr.NewSetTask(null, 0x01, 0x0b, (Int16)(1)); //coeff_change
        }

        #endregion



        #region Text Set Event Handlers

        private void GenericEventHandler_SetTxt_TxtChanged(object sender, EventArgs e)
        {
            int temp = (int)((Control)sender).Tag;
            txtGroup[temp].CheckValidity();
        }

        private void GenericEventHandler_SetTxt_TxtKeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                int temp = (int)((Control)sender).Tag;
                txtGroup[temp].SetText();
            }
        }

        private void GenericEventHandler_SetTxt_BtnClick(object sender, EventArgs e)
        {
            int temp = (int)((Control)sender).Tag;
            txtGroup[temp].SetText();
        }

        #endregion



        #region Slider Set Event Handlers
        private void GenericEventHandler_SetSldr_SldrScroll(object sender, EventArgs e)
        {
            int temp = (int)((Control)sender).Tag;
            sldrGroup[temp].SetSlider();
        }

        private void GenericEventHandler_SetSldr_TxtKeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                int temp = (int)((Control)sender).Tag;
                sldrGroup[temp].SetText();
            }
        }

        private void GenericEventHandler_SetSldr_TxtChanged(object sender, EventArgs e)
        {
            int temp = (int)((Control)sender).Tag;
            sldrGroup[temp].CheckValidity();
        }
        #endregion

        private void tabPage_0_Click(object sender, EventArgs e)
        {

        }


        #endregion

        private void btnClearFaultFlg_Click(object sender, EventArgs e)
        {
            commsMngr.NewSetTask(null, 0x02, 0x00, 1);
        }

        private void label13_Click(object sender, EventArgs e)
        {

        }

        private void GuiStartUp_Load(object sender, EventArgs e)
        {

        }



        private void txtVpfc_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter && txtVbus_set.ForeColor == System.Drawing.Color.SeaGreen)
            {
                int ptr = commsMngr.ptrWriteAt;
                commsMngr.cmdNum[ptr] = 0x01;
                commsMngr.itemNum[ptr] = 0x00;
                commsMngr.data[ptr] = (Int32)(Convert.ToDouble(txtVbus_set.Text) * Math.Pow(2, 6));
                commsMngr.ctrl[ptr] = this;
                commsMngr.ptrWriteAt++;

                commsMngr.TryNewCommsTask();

                //txtGetData1.Text = txtGetData1.Text + "int";   //debug
                txtVbus_set.Invoke(new EventHandler(delegate
                {
                    txtVbus_set.ForeColor = SystemColors.WindowText;
                }));
            }
            else if (e.KeyCode == Keys.Enter)
            {
                txtVbus_set.Text = "";
            }
        }

        private void txtVpfc_TextChanged(object sender, EventArgs e)
        {
            Regex HasRptingDecimals = new Regex("[.][0-9]*[.]");
            Regex HasRptingNegSign = new Regex("[-][0-9.]*[-]");
            Regex HasBadNegSign = new Regex("[0-9.]+[-]");
            Regex HasInvalidChars = new Regex("[^0-9.-]");
            Regex HasInvalidChars_NoNeg = new Regex("[^0-9.]");
            Regex IsNotEmpty = new Regex("[0-9]");

            string checkText = txtVbus_set.Text;
            txtVbus_set.ForeColor = System.Drawing.Color.Crimson;

            if (HasInvalidChars.IsMatch(checkText) == false && HasBadNegSign.IsMatch(checkText) == false && HasRptingNegSign.IsMatch(checkText) == false && HasRptingDecimals.IsMatch(checkText) == false && IsNotEmpty.IsMatch(checkText) == true)
            {
                double test = Convert.ToDouble(txtVbus_set.Text);
                if ((test == 0) || ((test <= 400 && test >= 345)))
                {
                    txtVbus_set.ForeColor = System.Drawing.Color.SeaGreen;
                    //_value = test;
                }
            }
        }

        private void btnPFC_Click(object sender, EventArgs e)
        {
            if (txtVbus_set.ForeColor == System.Drawing.Color.SeaGreen)
            {

                int ptr = commsMngr.ptrWriteAt;
                commsMngr.cmdNum[ptr] = 0x01;
                commsMngr.itemNum[ptr] = 0x00;
                commsMngr.data[ptr] = (Int32)(Convert.ToDouble(txtVbus_set.Text) * Math.Pow(2, 6));
                commsMngr.ctrl[ptr] = this;
                commsMngr.ptrWriteAt++;

                commsMngr.TryNewCommsTask();

                //txtGetData1.Text = txtGetData1.Text + "int";   //debug
                txtVbus_set.Invoke(new EventHandler(delegate
                {
                    txtVbus_set.ForeColor = SystemColors.WindowText;
                }));
            }
            else
            {
                txtVbus_set.Text = "";
            }

        }

/*
        private void txtItrip_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter && txtItrip.ForeColor == System.Drawing.Color.SeaGreen)
            {
                int ptr = commsMngr.ptrWriteAt;
                commsMngr.cmdNum[ptr] = 0x01;
                commsMngr.itemNum[ptr] = 0x01;
                commsMngr.data[ptr] = (Int32)(Convert.ToDouble(txtItrip.Text) * Math.Pow(2, 15)/(4.9699)); //Q15 Value for DACval
                commsMngr.ctrl[ptr] = this;                                                                // 4.9699 = Max Current             
                commsMngr.ptrWriteAt++;

                commsMngr.TryNewCommsTask();

                //txtGetData1.Text = txtGetData1.Text + "int";   //debug
                txtItrip.Invoke(new EventHandler(delegate
                {
                    txtItrip.ForeColor = SystemColors.WindowText;
                }));
            }
            else if (e.KeyCode == Keys.Enter)
            {
                txtItrip.Text = "";
            }
        }

        private void txtItrip_TextChanged(object sender, EventArgs e)
        {
            Regex HasRptingDecimals = new Regex("[.][0-9]*[.]");
            Regex HasRptingNegSign = new Regex("[-][0-9.]*[-]");
            Regex HasBadNegSign = new Regex("[0-9.]+[-]");
            Regex HasInvalidChars = new Regex("[^0-9.-]");
            Regex HasInvalidChars_NoNeg = new Regex("[^0-9.]");
            Regex IsNotEmpty = new Regex("[0-9]");

            string checkText = txtItrip.Text;
            txtItrip.ForeColor = System.Drawing.Color.Crimson;

            if (HasInvalidChars.IsMatch(checkText) == false && HasBadNegSign.IsMatch(checkText) == false && HasRptingNegSign.IsMatch(checkText) == false && HasRptingDecimals.IsMatch(checkText) == false && IsNotEmpty.IsMatch(checkText) == true)
            {
                double test = Convert.ToDouble(txtItrip.Text);
                if (test >= 0 && test <= 4.6)
                {
                    txtItrip.ForeColor = System.Drawing.Color.SeaGreen;
                    //_value = test;
                }
            }
        }

        private void txtItrip_Click(object sender, EventArgs e)
        {
            if (txtItrip.ForeColor == System.Drawing.Color.SeaGreen)
            {

                int ptr = commsMngr.ptrWriteAt;
                commsMngr.cmdNum[ptr] = 0x01;
                commsMngr.itemNum[ptr] = 0x01;
                commsMngr.data[ptr] = (Int32)(Convert.ToDouble(txtItrip.Text) * Math.Pow(2, 15)/(4.9699));
                commsMngr.ctrl[ptr] = this;
                commsMngr.ptrWriteAt++;

                commsMngr.TryNewCommsTask();

                //txtGetData1.Text = txtGetData1.Text + "int";   //debug
                txtItrip.Invoke(new EventHandler(delegate
                {
                    txtItrip.ForeColor = SystemColors.WindowText;
                }));
            }
            else
            {
                txtItrip.Text = "";
            }

        }

*/
      

        private void statusStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {

        }

        private void pnlMain_Paint(object sender, PaintEventArgs e)
        {

        }

 /*       private void txtComp_mode_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter && txtComp_mode.ForeColor == System.Drawing.Color.SeaGreen)
            {
                int ptr = commsMngr.ptrWriteAt;
                commsMngr.cmdNum[ptr] = 0x01;
                commsMngr.itemNum[ptr] = 0x02;
                commsMngr.data[ptr] = (Int32)(Convert.ToDouble(txtComp_mode.Text) * Math.Pow(2, 0)); //Q0 Value for Auto-Compensate
                commsMngr.ctrl[ptr] = this;                                                                        
                commsMngr.ptrWriteAt++;

                commsMngr.TryNewCommsTask();

                //txtGetData1.Text = txtGetData1.Text + "int";   //debug
                txtComp_mode.Invoke(new EventHandler(delegate
                {
                    txtComp_mode.ForeColor = SystemColors.WindowText;
                }));
            }
            else if (e.KeyCode == Keys.Enter)
            {
                txtComp_mode.Text = "";
            }
        }

        private void txtComp_mode_TextChanged(object sender, EventArgs e)
        {
            Regex HasRptingDecimals = new Regex("[.][0-9]*[.]");
            Regex HasRptingNegSign = new Regex("[-][0-9.]*[-]");
            Regex HasBadNegSign = new Regex("[0-9.]+[-]");
            Regex HasInvalidChars = new Regex("[^0-9.-]");
            Regex HasInvalidChars_NoNeg = new Regex("[^0-9.]");
            Regex IsNotEmpty = new Regex("[0-9]");

            string checkText = txtComp_mode.Text;
            txtComp_mode.ForeColor = System.Drawing.Color.Crimson;

            if (HasInvalidChars.IsMatch(checkText) == false && HasBadNegSign.IsMatch(checkText) == false && HasRptingNegSign.IsMatch(checkText) == false && HasRptingDecimals.IsMatch(checkText) == false && IsNotEmpty.IsMatch(checkText) == true)
            {
                double test = Convert.ToDouble(txtComp_mode.Text);
                if (test >= 0 && test <= 1.0)
                {
                    txtComp_mode.ForeColor = System.Drawing.Color.SeaGreen;
                    //_value = test;
                }
            }
        }*/


        private void lblIpfc1_Click(object sender, EventArgs e)
        {

        }

        //-----------------------------------------------------------
    
    
        private void btnCntl_Calc_Click(object sender, EventArgs e)
        {
            double fz1, fz2, fp1, fp2, Kdc, fs, wz1, wz2, wp1, wp2, pi, wr, Q, c2, c1, c0, d2, d1, d0, den, b0, b1, b2, a0, a1, a2;
            pi = 3.141592654; //pi = 22/7 but value used here = PI() from Excel

            //  btnCntl_Calc.BackColor = System.Drawing.Color.Green;
            //  btnCntl_Calc.Text = "On";

            fz1 = Convert.ToDouble(txtz1.Text) * Math.Pow(10, 3);
            fz2 = Convert.ToDouble(txtz2.Text) * Math.Pow(10, 3);
            fp1 = Convert.ToDouble(txtp1.Text) * Math.Pow(10, 3);
            fp2 = Convert.ToDouble(txtp2.Text) * Math.Pow(10, 3);
            Kdc = Convert.ToDouble(txtKdc.Text);
            fs = Convert.ToDouble(txtfs.Text) * Math.Pow(10, 3);

            wz1 = 2 * pi * fz1;
            wz2 = 2 * pi * fz2;
            wp1 = 0;        //2*pi*fp1
            wp2 = 2 * pi * fp2;

            wr = Math.Sqrt(wz1 * wz2);
            Q = wz1 * wz2 / ((wz1 + wz2) * wr);

            c2 = Kdc / Math.Pow(wr, 2);
            c1 = Kdc / (Q * wr);
            c0 = Kdc;

            d2 = 1 / wp2;
            d1 = 1;
            d0 = 0;

            den = 4 * d2 * Math.Pow(fs, 2) + 2 * d1 * fs + d0;

            b0 = (4 * c2 * Math.Pow(fs, 2) + 2 * c1 * fs + c0) / den;
            b1 = (-8 * c2 * Math.Pow(fs, 2) + 2 * c0) / den;
            b2 = (4 * c2 * Math.Pow(fs, 2) - 2 * c1 * fs + c0) / den;

            a0 = 1;
            a1 = (-1) * (-8 * d2 * Math.Pow(fs, 2) + 2 * d0) / den;
            a2 = (-1) * (4 * d2 * Math.Pow(fs, 2) - 2 * d1 * fs + d0) / den;

            if (b0 < 32 && b1 < 32 && b2 < 32 && a0 < 32 && a1 < 32 && a2 < 32)
            {
                commsMngr.NewSetTask(null, 0x01, 0x01, (Int16)(b0 * Math.Pow(2, 10)));
                commsMngr.NewSetTask(null, 0x01, 0x02, (Int16)(b1 * Math.Pow(2, 10)));
                commsMngr.NewSetTask(null, 0x01, 0x03, (Int16)(b2 * Math.Pow(2, 10)));

                commsMngr.NewSetTask(null, 0x01, 0x04, (Int16)(a0 * Math.Pow(2, 10)));
                commsMngr.NewSetTask(null, 0x01, 0x05, (Int16)(a1 * Math.Pow(2, 10)));
                commsMngr.NewSetTask(null, 0x01, 0x06, (Int16)(a2 * Math.Pow(2, 10)));

                commsMngr.NewSetTask(null, 0x01, 0x07, (Int16)(1)); //coeff_change
            }
            /*   int i,j;

               for (i = 0; i < 32000; i++)
               {
                   for (j = 0; j < 100; j++)
                   { 
                   }
               }

               btnCntl_Calc.BackColor = System.Drawing.Color.LightGray;
               btnCntl_Calc.Text = "Off";*/

        }

        private void label36_Click(object sender, EventArgs e)
        {

        }

        private void txtIfb_TextChanged(object sender, EventArgs e)
        {

        }

        private void txtFrequency_TextChanged(object sender, EventArgs e)
        {

        }

        private void pnlIpcTotal_Paint(object sender, PaintEventArgs e)
        {

        }

        private void btn_Vloop_cntl_calc_Click(object sender, EventArgs e)
        {
            commsMngr.NewSetTask(null, 0x01, 0x08, (Int16)(1)); //Vloop coeff_change
        }


        
        
        //-----------------------------------------------------------
    }
}
    
   