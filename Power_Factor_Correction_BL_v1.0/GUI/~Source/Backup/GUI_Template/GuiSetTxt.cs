using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Text.RegularExpressions;

namespace GUI_Template
{
    public class GuiSetTxt : Object
    {
        private System.Windows.Forms.TextBox _txt;
        private System.Windows.Forms.TextBox _fsv;
        private System.Windows.Forms.Button _btn;
        private byte _cmdNum;
        private byte _itemNum;
        private double _value;
        private int _qValue;
        private double _minValue;
        private double _maxValue;
        private double _fsvVal;
        private int _validType;  //0 - not valid, 1 - raw integer, 2 - float w/ fsv

        public CommsManager commsMngr;
        
        
        #region Constructors
        //Textbox Constructor, No button
        public GuiSetTxt(System.Windows.Forms.TextBox textBox, byte itemNumber) 
        {
            _txt = textBox;

            _itemNum = itemNumber;
            if (_txt.Text == "")
            {
                _txt.Text = "0";
            }

            //fsvVal = 0;
            _value = Convert.ToDouble(_txt.Text);
            _validType = 1;

            _txt.Tag = (int)_itemNum;
        }

        //Textbox Constructor, with button
        public GuiSetTxt(System.Windows.Forms.TextBox textBox, System.Windows.Forms.Button button, byte itemNumber)
        {
            _txt = textBox;
            _btn = button;

            _itemNum = itemNumber;
            if (_txt.Text == "")
            {
                _txt.Text = "0";
            }

            //fsvVal = 0;
            _value = Convert.ToDouble(_txt.Text);
            _validType = 1;

            _txt.Tag = (int)_itemNum;
            _btn.Tag = (int)_itemNum;

            _cmdNum = 0x01;
        }

        //TextBox Constructor, with qValue
        public GuiSetTxt(System.Windows.Forms.TextBox textBox, System.Windows.Forms.Button button, int qValue, byte itemNumber)
        {
            _txt = textBox;
            _qValue = qValue;
            _btn = button;
            _minValue = -Math.Pow(2, 15 - _qValue);
            _maxValue = Math.Pow(2, 15 - _qValue);

            _itemNum = itemNumber;
            if (_txt.Text == "")
            {
                _txt.Text = "0";
            }

            _value = Convert.ToDouble(_txt.Text);
            _validType = 2;

            _txt.Tag = (int)_itemNum;
            _btn.Tag = (int)_itemNum;

            _cmdNum = 0x01;
        }

        //TextBox Constructor, with qValue and min-max
        public GuiSetTxt(System.Windows.Forms.TextBox textBox, System.Windows.Forms.Button button, int qValue, double minValue, double maxValue, byte itemNumber)
        {
            _txt = textBox;
            _qValue = qValue;
            _btn = button;
            _minValue = minValue;
            _maxValue = maxValue;

            _itemNum = itemNumber;
            if (_txt.Text == "")
            {
                _txt.Text = "0";
            }

            _value = Convert.ToDouble(_txt.Text);
            _validType = 2;

            _txt.Tag = (int)_itemNum;
            _btn.Tag = (int)_itemNum;

            _cmdNum = 0x01;
        }

        //Textbox Constructor, with FSV Textbox
        public GuiSetTxt(System.Windows.Forms.TextBox textBox, System.Windows.Forms.TextBox fsvTextBox, System.Windows.Forms.Button button, byte itemNumber)
        {
            _txt = textBox;
            _fsv = fsvTextBox;
            _btn = button;

            _itemNum = itemNumber;
            if (_txt.Text == "")
            {
                _txt.Text = "0";
            }

            if (_fsv.Text == "")
            {
                _fsvVal = 0;
            }
            else
            {
                _fsvVal = Convert.ToDouble(_fsv.Text);
            }

            _value = Convert.ToDouble(_txt.Text);
            _validType = 3;

            _txt.Tag = (int)_itemNum;
            _btn.Tag = (int)_itemNum;

            _cmdNum = 0x01;
        }

        #endregion


        public void CheckValidity()
        {
            Regex HasRptingDecimals = new Regex("[.][0-9]*[.]");
            Regex HasRptingNegSign = new Regex("[-][0-9.]*[-]");
            Regex HasBadNegSign = new Regex("[0-9.]+[-]");
            Regex HasInvalidChars = new Regex("[^0-9.-]");
            Regex HasInvalidChars_NoNeg = new Regex("[^0-9.]");
            Regex IsNotEmpty = new Regex("[0-9]");

            string checkText = _txt.Text;

            _txt.ForeColor = System.Drawing.Color.Crimson;

            if (_validType == 1)
            {
                if (HasInvalidChars_NoNeg.IsMatch(checkText) == false && IsNotEmpty.IsMatch(checkText) == true)
                {
                    double test = Convert.ToDouble(_txt.Text);
                    if (test < 32768)
                    {
                        _txt.ForeColor = System.Drawing.Color.SeaGreen;
                        _value = test;
                    }
                }
            }

            if (_validType == 2)
            {
                if (HasInvalidChars.IsMatch(checkText) == false && HasBadNegSign.IsMatch(checkText) == false && HasRptingNegSign.IsMatch(checkText) == false && HasRptingDecimals.IsMatch(checkText) == false && IsNotEmpty.IsMatch(checkText) == true)
                {
                    double test = Convert.ToDouble(_txt.Text);
                    if ((test <= _maxValue) && (test >= _minValue))
                    {
                        _txt.ForeColor = System.Drawing.Color.SeaGreen;
                        _value = test;
                    }
                }
            }

            if (_validType == 3)
            {
                if (HasInvalidChars.IsMatch(checkText) == false && HasRptingDecimals.IsMatch(checkText) == false && IsNotEmpty.IsMatch(checkText) == true)
                {
                    double fsvFloat;
                    double textFloat;

                    textFloat = Convert.ToDouble(checkText);
                    fsvFloat = Convert.ToDouble(_fsv.Text);

                    if (textFloat <= fsvFloat)
                    {
                        _txt.ForeColor = System.Drawing.Color.SeaGreen;
                        _value = textFloat;
                    }
                }
            }
        }


        public void SetText()
        {
            if (_validType == 1 && _txt.ForeColor == System.Drawing.Color.SeaGreen)
            {
                int ptr = commsMngr.ptrWriteAt;
                commsMngr.cmdNum[ptr] = _cmdNum;
                commsMngr.itemNum[ptr] = _itemNum;
                commsMngr.data[ptr] = (Int32)_value;
                commsMngr.ctrl[ptr] = this;
                commsMngr.ptrWriteAt++;
                
                commsMngr.TryNewCommsTask();

                //txtGetData1.Text = txtGetData1.Text + "int";   //debug
                _txt.Invoke(new EventHandler(delegate
                {
                    _txt.ForeColor = SystemColors.WindowText;
                }));
            }

            else if (_validType == 2  && _txt.ForeColor == System.Drawing.Color.SeaGreen)
            {
                int ptr = commsMngr.ptrWriteAt;
                commsMngr.cmdNum[ptr] = _cmdNum;
                commsMngr.itemNum[ptr] = _itemNum;
                commsMngr.data[ptr] = (Int32)(_value*Math.Pow(2,_qValue));
                commsMngr.ctrl[ptr] = this;
                commsMngr.ptrWriteAt++;

                commsMngr.TryNewCommsTask();

                //txtGetData1.Text = txtGetData1.Text + "int";   //debug
                _txt.Invoke(new EventHandler(delegate
                {
                    _txt.ForeColor = SystemColors.WindowText;
                }));
            }

            else if (_validType == 3 && _txt.ForeColor == System.Drawing.Color.SeaGreen)  //Textbox set with FSV box..
            {
                int ptr = commsMngr.ptrWriteAt;
                commsMngr.cmdNum[ptr] = _cmdNum;
                commsMngr.itemNum[ptr] = _itemNum;
                commsMngr.data[ptr] = (Int32)((_value * 32767) / Convert.ToDouble(_fsvVal));
                commsMngr.ctrl[ptr] = this;
                commsMngr.ptrWriteAt++;
                commsMngr.TryNewCommsTask();

                //txtGetData2.Text = byteHigh + "  " + byteLow;  //debug
                //txtGetData1.Text = txtGetData1.Text + "float";  //debug
                _txt.Invoke(new EventHandler(delegate
                {
                    _txt.ForeColor = SystemColors.WindowText;
                }));
            }

            else if (_txt.ForeColor == SystemColors.WindowText) { } //do nothing

            else
            {
                _txt.Clear();
            }          
        }


        public void SetDefault()
        {
            CheckValidity();
            SetText();
        }
    }
}
