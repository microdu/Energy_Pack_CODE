using System;
using System.Collections.Generic;
using System.Text;

namespace GUI_Template
{
    public class GuiComms
    {
        public System.IO.Ports.SerialPort port;
        public System.Windows.Forms.Label statusLabel;
        public System.Windows.Forms.Panel statusPanel;

        private int[] echobyte = new int[4];

        //---Writes 4 bytes of data across the SCI and echo checks---
        public void DataWrite(byte cmd, byte item, byte datHigh, byte datLow)
        {
            try
            {
                port.Write(new byte[] { cmd }, 0, 1);
                echobyte[3] = port.ReadByte();
                //label1.Text = echobyte3.ToString();          //Debug
                if (echobyte[3] != cmd)
                {
                    throw new InvalidProgramException();
                }

                port.Write(new byte[] { item }, 0, 1);
                echobyte[2] = port.ReadByte();
                //label1.Text = label1.Text + " " + echobyte2.ToString();     //Debug
                if (echobyte[2] != item)
                {
                    throw new InvalidProgramException();
                }

                port.Write(new byte[] { datLow }, 0, 1);  //host expects LSB then MSB
                echobyte[0] = port.ReadByte();
                //label1.Text = label1.Text + " " + echobyte1.ToString();     //Debug
                if (echobyte[0] != datLow)
                {
                    throw new InvalidProgramException();
                }

                port.Write(new byte[] { datHigh }, 0, 1);
                echobyte[1] = port.ReadByte();
                //label1.Text = label1.Text + " " + echobyte0.ToString();     //Debug
                if (echobyte[1] != datHigh)
                {
                    throw new InvalidProgramException();
                }
                //if (echobyte[0] == 2)     //debug
                //{ 
                //}
            }

            catch (InvalidProgramException err)
            {
                statusLabel.Text = "RX/TX Data Do Not Match";
                statusPanel.BackColor = System.Drawing.Color.Yellow;
                //comErrorFound = true;
            }
            catch (TimeoutException err)
            {
                statusLabel.Text = "Timeout Error";
                statusPanel.BackColor = System.Drawing.Color.Yellow;
                //comErrorFound = true;
            }
            catch (Exception err)
            {
                statusLabel.Text = "Unhandled Exception";
                statusPanel.BackColor = System.Drawing.Color.Yellow;

                //comErrorFound = true;
            }
        }

    }
}
