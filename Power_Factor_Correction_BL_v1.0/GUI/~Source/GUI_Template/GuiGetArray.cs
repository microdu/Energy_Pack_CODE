using System;
using System.Collections.Generic;
using System.Text;

namespace GUI_Template
{
    public class GuiGetArray : Object
    {

        private System.Windows.Forms.TextBox[] _txt;
        public System.Windows.Forms.TextBox[] txt
        {
            get { return _txt; }
        }
        private byte _cmdNum;
        private byte _itemNum;

        //0 - not valid, 1 - raw integer, 2 - float w/ fsv, 3 - easy resistive divider based fsv
        private int _guiValidType;
        private int _resistor1;
        private int _resistor2;
        private double[] _dataArray;
        private int _dataArraySize;

        public CommsManager commsMngr;

        private int _qValue;
        private double _Kv;


        #region Constructors
        public GuiGetArray(System.Windows.Forms.TextBox[] textbox, byte itemNumber)
        {
            _txt = textbox;
            _dataArraySize = _txt.Length;
            _cmdNum = 0x05;
            _itemNum = itemNumber;
            _dataArray = new double[_dataArraySize];
            //isReceiving = false;

            //arraySize = 1;  //base on targerVarType?
            _guiValidType = 1;
        }

        public GuiGetArray(System.Windows.Forms.TextBox[] textbox, byte itemNumber, int q_Value)
        {
            _txt = textbox;
            _dataArraySize = _txt.Length;
            _cmdNum = 0x05;
            _itemNum = itemNumber;
            _dataArray = new double[_dataArraySize];
            //isReceiving = false;
            _qValue = q_Value;

            _guiValidType = 2;
        }

        public GuiGetArray(System.Windows.Forms.TextBox[] textbox, byte itemNumber, int R1, int R2)
        {
            _txt = textbox;
            _dataArraySize = _txt.Length;
            _dataArray = new double[_dataArraySize];

            _cmdNum = 0x05;
            _itemNum = itemNumber;
            _resistor1 = R1;
            _resistor2 = R2;
            //isReceiving = false;

            //arraySize = 1;  //base on targerVarType?
            _guiValidType = 3;

            float Gfb = (float)_resistor2 / (float)(_resistor1 + _resistor2);
            float VoMax = 3 / Gfb;
            _qValue = (int)(Math.Log(Math.Pow(2,15) / VoMax) / Math.Log(2));
            _Kv = VoMax / Math.Pow(2, 15 - _qValue);
        }
        #endregion


        public void RequestBuffer()
        {

            int ptr = commsMngr.ptrWriteAt;
            commsMngr.cmdNum[ptr] = _cmdNum;
            commsMngr.itemNum[ptr] = _itemNum;
            commsMngr.amountToGet[ptr] = _dataArraySize;
            commsMngr.data[ptr] = 65535;
            commsMngr.ctrl[ptr] = this;
            commsMngr.ptrWriteAt++;
            commsMngr.TryNewCommsTask();           
        }


        public void ReadBuffer(byte[] buffer)
        {
            byte[] byteBuffer = buffer;

            for (int i = 0; i < _dataArraySize; i++)
            {
                if (_guiValidType == 1)
                {
                    _dataArray[i] = (byteBuffer[2 * i] + byteBuffer[2 * i + 1] * 256);// *8;
                    string toWrite = _dataArray[i].ToString();
                    _txt[i].Invoke(new EventHandler(delegate
                    {
                        _txt[i].Text = toWrite;
                    }));
                }
                else if (_guiValidType == 2)
                {
                    _dataArray[i] = (byteBuffer[2 * i] + byteBuffer[2 * i + 1] * 256) / Math.Pow(2, _qValue);// *8;
                    string toWrite = _dataArray[i].ToString("#.0000");
                    _txt[i].Invoke(new EventHandler(delegate
                    {
                        _txt[i].Text = toWrite;
                    }));
                }
                else if (_guiValidType == 3)
                {
                    _dataArray[i] = (byteBuffer[2 * i] + byteBuffer[2 * i + 1] * 256) * _Kv / Math.Pow(2, _qValue);// *8;
                    string toWrite = _dataArray[i].ToString("#.0000");
                    _txt[i].Invoke(new EventHandler(delegate
                    {
                        _txt[i].Text = toWrite;
                    }));
                }
            }
            commsMngr.TaskComplete();
        }
    }
}
