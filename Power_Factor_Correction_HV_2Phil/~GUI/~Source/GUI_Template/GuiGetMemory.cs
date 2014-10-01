using System;
using System.Collections.Generic;
using System.Text;

namespace GUI_Template
{
    public class GuiGetMemory : Object
    {
        private System.Windows.Forms.TextBox _txt;
        public System.Windows.Forms.TextBox txt
        {
            get { return _txt; }
        }

        private System.Windows.Forms.TextBox _txtMemoryAddress;
        private byte _cmdNum;
        private byte _itemNum;

        private int _guiValidType;
        private double[] _dataArray;

        public CommsManager commsMngr;


        #region Constructors

        public GuiGetMemory(System.Windows.Forms.TextBox textbox, byte itemNumber)
        {
            _txt = textbox;
            _cmdNum = 0x06;
            _itemNum = itemNumber;
            _dataArray = new double[1];
            _txt.Tag = (int)_itemNum;

            _guiValidType = 1;
        }

        public GuiGetMemory(System.Windows.Forms.TextBox address_textbox, System.Windows.Forms.TextBox result_textbox, byte itemNumber)
        {
            _txt = result_textbox;
            _txtMemoryAddress = address_textbox;
            _cmdNum = 0x06;
            _itemNum = itemNumber;
            _dataArray = new double[1];
            _txtMemoryAddress.Tag = (int)_itemNum;

            _guiValidType = 2;
        }

        #endregion


        public void RequestBuffer()
        {
            int ptr = commsMngr.ptrWriteAt;

            if (_guiValidType == 1)
            {
                commsMngr.data[ptr] = (Convert.ToInt32(_txt.Text, 16)) % 65536;
                commsMngr.data2[ptr] = (Convert.ToInt32(_txt.Text, 16)) / 65536;
            }
            else if (_guiValidType == 2)
            {
                if (_txtMemoryAddress.Text.Substring(0, 2) == "0x") _txtMemoryAddress.Text = _txtMemoryAddress.Text.Remove(0,2);
                commsMngr.data[ptr] = (Convert.ToInt32(_txtMemoryAddress.Text, 16)) % 65536;
                commsMngr.data2[ptr] = (Convert.ToInt32(_txtMemoryAddress.Text, 16)) / 65536;
                _txtMemoryAddress.Invoke(new EventHandler(delegate
                {
                    _txtMemoryAddress.Text = _txtMemoryAddress.Text.ToUpper();
                    _txtMemoryAddress.Text = "0x" + _txtMemoryAddress.Text.PadLeft(8,'0');        
                }));
            }
 
            commsMngr.cmdNum[ptr] = _cmdNum;
            commsMngr.itemNum[ptr] = _itemNum;
            commsMngr.amountToGet[ptr] = 2;

            commsMngr.ctrl[ptr] = this;
            commsMngr.ptrWriteAt++;

            commsMngr.TryNewCommsTask();
        }


        public void ReadBuffer(byte[] buffer)
        {
            byte[] byteBuffer = buffer;
            commsMngr.TaskComplete();

            for (int i = 0; i < 1; i++)
            {
                if (_guiValidType == 1 || _guiValidType == 2)
                {
                    _dataArray[i] = (byteBuffer[2 * i] + byteBuffer[2 * i + 1] * 256);// *8;
                    string toWrite = "0x" + ((int)_dataArray[i]).ToString("X4");

                    _txt.Invoke(new EventHandler(delegate
                    {
                        _txt.Text = toWrite;
                    }));
                }
            }
        }
    }
}
