using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.IO;

namespace GUI_Template
{
    public class CommsManager
    {
        private int[] echobyte = new int[4];

        private System.IO.Ports.SerialPort comPort;

        public bool comValid;
        public bool isReceiving;
        private int[] dataRxedBuffer;
        public bool dataRxedBufferFilled;

        public byte[] cmdNum;
        public byte[] itemNum;
        public int[] data;
        public int[] data2;
        public int[] amountToGet;       
        public Object[] ctrl;

        private int _ptrWorkingAt;
        public int ptrWorkingAt
        {
            get
            { return _ptrWorkingAt; }
            set
            {
                if (value > 63) _ptrWorkingAt = 0;
                else _ptrWorkingAt = value;
            }
        }
        private int _ptrWriteAt;
        public int ptrWriteAt
        {
            get
            { return _ptrWriteAt; }
            set
            {
                if (value > 63) _ptrWriteAt = 0;
                else _ptrWriteAt = value;
            }
        }

        public bool IsOpen
        {
            get { return comPort.IsOpen; }
        }
        public string PortName
        {
            get { return comPort.PortName; }
            set { comPort.PortName = value; }
        }
        public int BaudRate
        {
            get { return comPort.BaudRate; }
            set { comPort.BaudRate = value; }
        }
        
        private GuiStartUp mainForm;
        private Thread RunCommsThread;
        private Thread TryNewCommsThread;


        #region Constructors

        public CommsManager(GuiStartUp parentForm)
        {
            cmdNum = new byte[64];
            itemNum = new byte[64];
            data = new int[64];
            data2 = new int[64];
            amountToGet = new int[64];
            ctrl = new Object[64];
            isReceiving = false;
            //comPort = commsPort;
            _ptrWorkingAt = 0;
            _ptrWriteAt = 0;
            mainForm = parentForm;
            comPort = new System.IO.Ports.SerialPort();
  
            comPort.PortName = Properties.Settings.Default.ComPortName;
            comPort.BaudRate = Convert.ToInt32(Properties.Settings.Default.ComBaudRate);
            comPort.ReceivedBytesThreshold = 600;
            comPort.ReadTimeout = 1000;
            comPort.WriteTimeout = 1000;
            comPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(commsPort_DataReceived);
            RunCommsThread = new Thread(new ThreadStart(RunCommsTask));
            TryNewCommsThread = new Thread(new ThreadStart(TryNewCommsTask));
            dataRxedBufferFilled = false;
        }
        #endregion



        //---Check if the serialport is valid
        public bool TestConnection()
        {
            comValid = true;

            //DataWrite sets comValid to false if write is not echoed properly
            DataWrite(0x00, 0x00, 0x00, 0x02);  //try to blink LED 

            return comValid;
        }


        //---Close the comport
        public void Close()
        {
            if (comPort.IsOpen == true)
            {
                comPort.Close();
            }
        }


        //---Try to open the comport.  Comport can fail if it is currently in use elsewhere
        public bool Open()
        {
            try
            {
                if (comPort.IsOpen == false)
                {
                    comPort.Open();
                    return true;
                }
                return false;
            }
            catch (Exception err)
            {
                return false;
            }
        }


        //---Syncronizes Client and Target and open communications
        public bool SciConnect()
        {
            string[] s = System.IO.Ports.SerialPort.GetPortNames();
            for (int i = 0; i < s.Length; i++)
            {
                if (s[i] == Properties.Settings.Default.ComPortName)
                {
                    comPort.Open();

                    while (comPort.IsOpen == false) { }

                    comPort.DiscardOutBuffer();
                    comPort.DiscardInBuffer();

                    //necessary for some SerialPorts
                    DataWrite(0x00, 0x00, 0x00, 0x00); //initialise data transfer
                    DataWrite(0x00, 0x00, 0x00, 0x00); //initialise data transfer                

                    comValid = true;
                    DataWrite(0x00, 0x00, 0x00, 0x02);  //try to blink LED and receive errors

                    if (comValid == false)
                    {
                        comPort.Close();
                    }

                    return comValid;
                }
            }
            return false;
        }


        //---Deletes all tasks
        public void ClearCommands()
        {
            for (int i = 0; i < 64; i++)
            {
                cmdNum[i] = 0x00;
            }
            ptrWorkingAt = 0;
            ptrWriteAt = 0;
        }





        //---Writes 4 bytes of data across the SCI and echo checks---
        public void DataWrite(byte cmd, byte item, byte datHigh, byte datLow)
        {
            try
            {
                comPort.Write(new byte[] { cmd }, 0, 1);
                echobyte[3] = comPort.ReadByte();
                //label1.Text = echobyte3.ToString();          //Debug
                if (echobyte[3] != cmd)
                {
                    throw new InvalidProgramException();
                }

                comPort.Write(new byte[] { item }, 0, 1);
                echobyte[2] = comPort.ReadByte();
                //label1.Text = label1.Text + " " + echobyte2.ToString();     //Debug
                if (echobyte[2] != item)
                {
                    throw new InvalidProgramException();
                }

                comPort.Write(new byte[] { datLow }, 0, 1);  //host expects LSB then MSB
                echobyte[0] = comPort.ReadByte();
                //label1.Text = label1.Text + " " + echobyte1.ToString();     //Debug
                if (echobyte[0] != datLow)
                {
                    throw new InvalidProgramException();
                }

                comPort.Write(new byte[] { datHigh }, 0, 1);
                echobyte[1] = comPort.ReadByte();
                //label1.Text = label1.Text + " " + echobyte0.ToString();     //Debug
                if (echobyte[1] != datHigh)
                {
                    throw new InvalidProgramException();
                }
            }

            catch (Exception err)
            {
                //lblStatus.Text = "Unhandled Exception";
                comValid = false;
                //err.Message;
            }
        }


        //---Check if a communication is occurring through the serialport
        public void TryNewCommsTask()
        {
            // if serialport is currently not busy
            if (isReceiving == false && ptrWorkingAt!= ptrWriteAt)
            {
                if (ptrWorkingAt == ptrWriteAt + 1)
                {
                    mainForm.Invoke(new EventHandler(delegate
                    {
                        mainForm.connectionLost();
                    }));
                }

                isReceiving = true;
                RunCommsThread = new Thread(new ThreadStart(RunCommsTask));
                RunCommsThread.Start();              
            }          
        }


        //---Sends data across the serialport
        private void RunCommsTask()
        {
            int ptr = ptrWorkingAt;
            if (cmdNum[ptr] >= 0 && cmdNum[ptr] <= 3)
            {
                byte[] byteBuffer = BitConverter.GetBytes(data[ptr]);
                DataWrite(cmdNum[ptr], itemNum[ptr], byteBuffer[1], byteBuffer[0]);
                if (comValid == true)
                {
                    TaskComplete();
                    while (TryNewCommsThread.IsAlive == true) { }
                    TryNewCommsThread = new Thread(new ThreadStart(TryNewCommsTask));
                    TryNewCommsThread.Start();
                }
                else
                {
                    mainForm.Invoke(new EventHandler(delegate
                    {
                        mainForm.connectionLost();
                    }));
                }
            }
            if (cmdNum[ptr] == 4 || cmdNum[ptr] == 5)
            {
                comPort.ReceivedBytesThreshold = amountToGet[ptr] * 2;
                byte[] byteBuffer = BitConverter.GetBytes(amountToGet[ptr]);
                DataWrite(cmdNum[ptr], itemNum[ptr], byteBuffer[1], byteBuffer[0]);
                if (comValid == false)
                {
                    mainForm.Invoke(new EventHandler(delegate
                    {
                        mainForm.connectionLost();
                    }));
                }
            }
            if (cmdNum[ptr] == 6)
            {
                if (amountToGet[ptr] > 0)
                {
                    comPort.ReceivedBytesThreshold = amountToGet[ptr];
                }
                byte[] byteBuffer = BitConverter.GetBytes(data[ptr]);
                DataWrite(cmdNum[ptr], itemNum[ptr], byteBuffer[1], byteBuffer[0]);
                byteBuffer = BitConverter.GetBytes(data2[ptr]);
                DataWrite(cmdNum[ptr], itemNum[ptr], byteBuffer[1], byteBuffer[0]);
                if (comValid == false)
                {
                    mainForm.Invoke(new EventHandler(delegate
                    {
                        mainForm.connectionLost();
                    }));
                }
            }
        }


        //---Once task is complete allow a new task to begin
        public void TaskComplete()
        {
            ptrWorkingAt++;
            isReceiving = false;
        }


        //---Generic get task
        public void NewGetTask(Object _ctrl, byte _cmdNum, byte _itemNum, int _data, int _amountToGet)
        {
            cmdNum[ptrWriteAt] = _cmdNum;
            itemNum[ptrWriteAt] = _itemNum;
            data[ptrWriteAt] = _data;
            amountToGet[ptrWriteAt] = _amountToGet;
            ctrl[ptrWriteAt] = _ctrl;

            ptrWriteAt++;

            TryNewCommsTask();
        }


        //---Generic set task
        public void NewSetTask(Object _ctrl, byte _cmdNum, byte _itemNum, int _data)
        {
            cmdNum[ptrWriteAt] = _cmdNum;
            itemNum[ptrWriteAt] = _itemNum;
            data[ptrWriteAt] = _data;
            ctrl[ptrWriteAt] = _ctrl;

            ptrWriteAt++;

            TryNewCommsTask();
        }


        //---When data is received call the control who owns it to see what to do with the data
        private void commsPort_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            int numBytes = comPort.BytesToRead;
            if (numBytes == comPort.ReceivedBytesThreshold)
            {
                byte[] byteBuffer = new byte[numBytes];
                comPort.Read(byteBuffer, 0, numBytes);

                if ((ctrl[ptrWorkingAt]) is GUI_Template.GuiGetVar)
                {
                    ((GuiGetVar)ctrl[ptrWorkingAt]).ReadBuffer(byteBuffer);
                }
                else if ((ctrl[ptrWorkingAt]) is GUI_Template.GuiGetArray)
                {
                    ((GuiGetArray)ctrl[ptrWorkingAt]).ReadBuffer(byteBuffer);
                }
                else if ((ctrl[ptrWorkingAt]) is GUI_Template.GuiGraphTSArray)
                {
                    ((GuiGraphTSArray)ctrl[ptrWorkingAt]).ReadBuffer(byteBuffer);
                }
                else if ((ctrl[ptrWorkingAt]) is GUI_Template.GuiGetMemory)
                {
                    ((GuiGetMemory)ctrl[ptrWorkingAt]).ReadBuffer(byteBuffer);
                }
                else if (dataRxedBufferFilled == false)
                {
                    dataRxedBuffer = new int[numBytes];
                    for (int i = 0; i < byteBuffer.Length/2; i++)
                    {          
                        dataRxedBuffer[i] = (byteBuffer[2 * i] + byteBuffer[2 * i + 1] * 256);
                    }
                    dataRxedBufferFilled = true;
                }
                while (TryNewCommsThread.IsAlive == true) { }
                TryNewCommsThread = new Thread(new ThreadStart(TryNewCommsTask));
                TryNewCommsThread.Start();
            }
        }







        //---allow a different form to write data through the serialport
        public void WriteString(string text)
        {
            char[] temp = new char[1];
            temp = (text.ToCharArray(0,1));
            comPort.Write(temp, 0, 1);
        }

        //---allow a different form to write data through the serialport
        public byte[] WriteBytes(byte[] text)
        {
               byte[] k = new byte[256];

               byte[] bytebuffer = new byte[text.Length];
               bytebuffer = text;
                
            for (int i = 0; i<bytebuffer.Length; i++)
            {
                comPort.Write(bytebuffer, i, 1);
                k[i] = ReadByte();
                if (text[i] != k[i])
                {
                    return null;
                }
            }
            return k;      
        }

        //---allow a different form to read data through the serialport
        public byte ReadByte()
        {
            byte temp = 0;
            try
            {
                temp = (byte)comPort.ReadByte();
            }
            catch(Exception e)
            {
            }
            return temp;
        }




        //---load a .a00 file, send errors 
        public string LoadProgramFromFile(string port, string path, System.Windows.Forms.ProgressBar prb)
        {
            try
            {
                char oldChar = 'X';
                prb.Value = 0;

                this.Close();

                int oldBaud = this.BaudRate;

                this.PortName = port;
                this.BaudRate = 38400;

                this.Open();

                this.WriteString("A");
                int temp = Convert.ToInt32(this.ReadByte());
                if (temp == 65)
                {
                    // Create an instance of StreamReader to read from a file.
                    // The using statement also closes the StreamReader.
                    using (StreamReader sr = new StreamReader(path))
                    {
                        byte[] echoedBytes = new byte[256];
                        byte[] toBeTxedBytes = new byte[256];
                        char readChar;
                        int i = 0;

                        while (sr.EndOfStream != true)
                        {
                            readChar = (char)(sr.Read());
                            if ((readChar >= 'A' && readChar <= 'F') || (readChar >= '0' && readChar <= '9'))
                            {
                                if (oldChar != 'X')
                                {
                                    byte b = (byte)(Convert.ToInt32(readChar.ToString(), 16) + (Convert.ToInt32(oldChar.ToString(), 16) * 16));

                                    oldChar = 'X';
                                    toBeTxedBytes[i] = b;
                                    i++;
                                }
                                else
                                {
                                    oldChar = readChar;
                                }
                            }
                            else
                            {
                                oldChar = 'X';
                            }
                            if (i == 256)
                            {
                                echoedBytes = this.WriteBytes(toBeTxedBytes);
                                if (echoedBytes == null)
                                {
                                    throw new InvalidProgramException();
                                }
                                if (prb.Value >= 100)
                                {
                                    prb.Value = 0;
                                }
                                prb.Value = prb.Value + 2;
                                i = 0;
                            }
                        }
                        if (i != 0)
                        {
                            byte[] byteBufferRemaining = new byte[i];
                            for (int p = 0; p < i; p++)
                            {
                                byteBufferRemaining[p] = toBeTxedBytes[p];
                            }
                            echoedBytes = this.WriteBytes(byteBufferRemaining);
                            if (echoedBytes == null)
                            {
                                throw new InvalidProgramException();
                            }
                            prb.Value = 100;

                            this.Close();
                            this.BaudRate = oldBaud;
                            return ("Program Successfully Loaded");
                        }
                    }
                    this.Close();
                    this.BaudRate = oldBaud;
                    return ("Error: Program Did Not Load. Please Try Again.");
                }
                else
                {
                    this.Close();
                    this.BaudRate = oldBaud;
                    return ("Error: Improper COM Port, Power is Off, Program was Already Loaded, or SCI Boot Jumper is Not Placed.");
                }
                

            }
            catch (Exception err)
            {
                this.Close();
                return ("Error: Program Did Not Load. Please Try Again.");
            }
        }
    }
}
