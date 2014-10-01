using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Text.RegularExpressions;

namespace GUI_Template
{
    public class GuiSetSldr : Object
    {
        private System.Windows.Forms.TextBox _txt;
        private System.Windows.Forms.TrackBar _sldr;
        private System.Windows.Forms.Label _lblMinVal;
        private System.Windows.Forms.Label _lblMaxVal;
        private byte _cmdNum;
        private byte _itemNum;
        private double _fsvVal;
        private double _value;
        private int _guiTypePtr;        //1 - raw integer, 2 - float w/ fsv, 3 - resistive fsv
        //private double _Kv;
        private int _qValue;

        public CommsManager commsMngr;
        
        
        #region Constructors
        //no min-max labels
        public GuiSetSldr(System.Windows.Forms.TrackBar slider, System.Windows.Forms.TextBox textBox, byte itemNumber)
        {
            _sldr = slider;
            _txt = textBox;

            _itemNum = itemNumber;
            if (_txt.Text == "")
            {
                _txt.Text = "0";
            }

            _fsvVal = 0;
            _value = (double)Convert.ToInt32(_txt.Text);

            _txt.Tag = (int)_itemNum;
            _sldr.Tag = (int)_itemNum; 

            _cmdNum = 0x03;
            _guiTypePtr = 1;
        }

        //keeps _value at 0-FSV while using full range of an Int16
        public GuiSetSldr(System.Windows.Forms.TrackBar slider, System.Windows.Forms.TextBox textBox, System.Windows.Forms.Label lblMinimumValue, System.Windows.Forms.Label lblMaximumValue, byte itemNumber)
        {
            _txt = textBox;
            _sldr = slider;
            _lblMaxVal = lblMaximumValue;
            _lblMinVal = lblMinimumValue;

            _itemNum = itemNumber;
            if (_txt.Text == "")
            {
                _txt.Text = "0";
            }

            _fsvVal = Convert.ToDouble(_lblMaxVal.Text);
            _value = Convert.ToDouble(_txt.Text);

            _txt.Tag = (int)_itemNum;
            _sldr.Tag = (int)_itemNum;


            _cmdNum = 0x03;
            _guiTypePtr = 2;
        }

        //full scale value via resistive divider
        public GuiSetSldr(System.Windows.Forms.TrackBar slider, System.Windows.Forms.TextBox textBox, System.Windows.Forms.Label lblMinimumValue, System.Windows.Forms.Label lblMaximumValue, byte itemNumber, int R1, int R2)
        {
            _txt = textBox;
            _sldr = slider;
            _lblMaxVal = lblMaximumValue;
            _lblMinVal = lblMinimumValue;


            _itemNum = itemNumber;
            if (_txt.Text == "")
            {
                _txt.Text = "0";
            }

            _value = Convert.ToDouble(_txt.Text);

            _txt.Tag = (int)_itemNum;
            _sldr.Tag = (int)_itemNum;

            _cmdNum = 0x03;

            double Gfb = (double)R2 / (double)(R1 + R2);
            _fsvVal = 3 / Gfb;
            _qValue = (int)(Math.Log(Math.Pow(2, 15) / _fsvVal) / Math.Log(2));
            //_Kv = _fsvVal / Math.Pow(2, 15 - qValue);
            _lblMinVal.Text = ((int)0).ToString("0.0");
            _lblMaxVal.Text = _fsvVal.ToString("0.0");
            _guiTypePtr = 3;
        }

        //min-max lbls with Q value scaling/limiting
        public GuiSetSldr(System.Windows.Forms.TrackBar slider, System.Windows.Forms.TextBox textBox, System.Windows.Forms.Label lblMinimumValue, System.Windows.Forms.Label lblMaximumValue, byte itemNumber, int q_Value)
        {
            _txt = textBox;
            _sldr = slider;
            _lblMaxVal = lblMaximumValue;
            _lblMinVal = lblMinimumValue;
            _qValue = q_Value;

            _cmdNum = 0x03;
            _itemNum = itemNumber;


            if (_txt.Text == "")
            {
                _txt.Text = "0";
            }

            _value = Convert.ToDouble(_txt.Text);
            _txt.Tag = (int)_itemNum;
            _sldr.Tag = (int)_itemNum;

            int temp = ((int)Math.Pow(2, 15 - _qValue));
            _lblMinVal.Text = Convert.ToDouble(_lblMinVal.Text).ToString("0.0"); ;
            _lblMaxVal.Text = Convert.ToDouble(_lblMaxVal.Text).ToString("0.0");

            _sldr.Minimum = (int)(Convert.ToDouble(_lblMinVal.Text) * (Math.Pow(2, _qValue)));
            _sldr.Maximum = (int)(Convert.ToDouble(_lblMaxVal.Text) * (Math.Pow(2, _qValue)));

            double diff;

            diff = (Convert.ToDouble(_lblMaxVal.Text) - Convert.ToDouble(_lblMinVal.Text));
            for (int i = 0; i < 1000; i++)
            {
                if (diff > 1.5 * (Math.Pow(10, 4 - i)))
                {
                    if (_qValue < 9 && i > 4)
                    {
                        i--;
                    }
                    _sldr.TickFrequency = (int)((Math.Pow(10, 4 - i)) * Math.Pow(2, _qValue));
                    i = 1000;
                }
            }

            _guiTypePtr = 4;
        }

        #endregion


        public void CheckValidity()
        {
            Regex HasRptingDecimals = new Regex("[0-9]*[.][0-9]*[.]");
            Regex HasInvalidChars = new Regex("[^0-9.]");
            Regex HasInvalidChars_Neg = new Regex("[^0-9.-]");
            Regex IsNotEmpty = new Regex("[0-9]");
            Regex HasInt = new Regex("int-[0-9]+");

            string checkText = _txt.Text;

            _txt.ForeColor = System.Drawing.Color.Crimson;

            if (_guiTypePtr == 1)
            {
                if (HasInvalidChars.IsMatch(checkText) == false && IsNotEmpty.IsMatch(checkText) == true && Convert.ToInt32(_txt.Text) < _sldr.Maximum)
                {
                    _txt.ForeColor = System.Drawing.Color.SeaGreen;
                    _value = Convert.ToDouble(_txt.Text);
                }
            }
            if (_guiTypePtr == 2)
            {

                if (HasInvalidChars.IsMatch(checkText) == false && HasRptingDecimals.IsMatch(checkText) == false && IsNotEmpty.IsMatch(checkText) == true)
                {
                    double textFloat = Convert.ToDouble(_txt.Text);
                    double fsvFloat = Convert.ToDouble(_lblMaxVal.Text);

                    if (textFloat <= fsvFloat)
                    {
                        _txt.ForeColor = System.Drawing.Color.SeaGreen;
                        _value = textFloat;
                    }
                }
            }
            if (_guiTypePtr == 3)
            {
                if (HasInvalidChars.IsMatch(checkText) == false && HasRptingDecimals.IsMatch(checkText) == false && IsNotEmpty.IsMatch(checkText) == true)
                {
                    double textFloat = Convert.ToDouble(_txt.Text);
                    double fsvFloat = Convert.ToDouble(_lblMaxVal.Text);

                    if (textFloat <= fsvFloat)
                    {
                        _txt.ForeColor = System.Drawing.Color.SeaGreen;
                        _value = textFloat;
                    }
                }
            }

            if (_guiTypePtr == 4)
            {
                if (HasInvalidChars_Neg.IsMatch(checkText) == false && HasRptingDecimals.IsMatch(checkText) == false && IsNotEmpty.IsMatch(checkText) == true)
                {
                    double textFloat = Convert.ToDouble(_txt.Text);
                    double fsvFloatMax = Convert.ToDouble(_lblMaxVal.Text);
                    double fsvFloatMin = Convert.ToDouble(_lblMinVal.Text);

                    if ((textFloat <= fsvFloatMax) && (textFloat >= fsvFloatMin) && (textFloat <= Convert.ToDouble(_lblMaxVal.Text)) && (textFloat >= Convert.ToDouble(_lblMinVal.Text)))
                    {
                        _txt.ForeColor = System.Drawing.Color.SeaGreen;
                        _value = textFloat;
                    }
                }
            }
        }


        public void SetSlider()
        { 
            if (_guiTypePtr == 1)
            {
                _value = _sldr.Value;

                _txt.ForeColor = SystemColors.WindowText;
                _txt.Text = _sldr.Value.ToString();
                _value = _sldr.Value;

                int ptr = commsMngr.ptrWriteAt;
                commsMngr.cmdNum[ptr] = _cmdNum;
                commsMngr.itemNum[ptr] = _itemNum;
                commsMngr.data[ptr] = _sldr.Value;
                commsMngr.ctrl[ptr] = this;
                commsMngr.ptrWriteAt++;
                commsMngr.TryNewCommsTask();
            }

            if (_guiTypePtr == 2)
            {
                _value = _sldr.Value * _fsvVal / 32767;
                _txt.Text = _value.ToString("0.000");
                _txt.ForeColor = SystemColors.WindowText;

                int ptr = commsMngr.ptrWriteAt;
                commsMngr.cmdNum[ptr] = _cmdNum;
                commsMngr.itemNum[ptr] = _itemNum;
                commsMngr.data[ptr] = _sldr.Value;
                commsMngr.ctrl[ptr] = this;
                commsMngr.ptrWriteAt++;
                commsMngr.TryNewCommsTask();
            }

            if (_guiTypePtr == 3)
            {
                _value = _sldr.Value * _fsvVal / 32767;
                _txt.Text = _value.ToString("0.000");
                _txt.ForeColor = SystemColors.WindowText;

                int ptr = commsMngr.ptrWriteAt;
                commsMngr.cmdNum[ptr] = _cmdNum;
                commsMngr.itemNum[ptr] = _itemNum;
                commsMngr.data[ptr] = _sldr.Value;
                commsMngr.ctrl[ptr] = this;
                commsMngr.ptrWriteAt++;
                commsMngr.TryNewCommsTask();
            }

            if (_guiTypePtr == 4)
            {
                int temp = _sldr.Value;
                if (temp > 32767)
                {
                    temp = 32767;
                    _sldr.Value = temp;
                }
                _value = _sldr.Value / Math.Pow(2, _qValue);
                if (_qValue > 2)
                {
                    _txt.Text = _value.ToString("0.000");
                }
                else if (_qValue == 1)
                {
                    _txt.Text = _value.ToString("0.0");
                }
                else if (_qValue == 0)
                {
                    _txt.Text = _value.ToString("0");
                }
                _txt.ForeColor = SystemColors.WindowText;

                int ptr = commsMngr.ptrWriteAt;
                commsMngr.cmdNum[ptr] = _cmdNum;
                commsMngr.itemNum[ptr] = _itemNum;
                commsMngr.data[ptr] = _sldr.Value;
                commsMngr.ctrl[ptr] = this;
                commsMngr.ptrWriteAt++;
                commsMngr.TryNewCommsTask();
            }
        }


        public void SetText()
        {
            if (_guiTypePtr == 1 && _txt.ForeColor == System.Drawing.Color.SeaGreen)  //raw integer
            {
                _sldr.Value = (Int32)_value;
                _txt.ForeColor = SystemColors.WindowText;

                int ptr = commsMngr.ptrWriteAt;
                commsMngr.cmdNum[ptr] = _cmdNum;
                commsMngr.itemNum[ptr] = _itemNum;
                commsMngr.data[ptr] = _sldr.Value;
                commsMngr.ctrl[ptr] = this;
                commsMngr.ptrWriteAt++;
                commsMngr.TryNewCommsTask();
            }

            if (_guiTypePtr == 2 && _txt.ForeColor == System.Drawing.Color.SeaGreen)   //fsv
            {
                _sldr.Value = (int)(_value * _fsvVal / Math.Pow(2, 16));
                _txt.ForeColor = SystemColors.WindowText;

                int ptr = commsMngr.ptrWriteAt;
                commsMngr.cmdNum[ptr] = _cmdNum;
                commsMngr.itemNum[ptr] = _itemNum;
                commsMngr.data[ptr] = (Int32)((_value * 32767) / Convert.ToDouble(_fsvVal));
                commsMngr.ctrl[ptr] = this;
                commsMngr.ptrWriteAt++;
                commsMngr.TryNewCommsTask();
            }

            if (_guiTypePtr == 3 && _txt.ForeColor == System.Drawing.Color.SeaGreen)  //for easy resistor entry...
            {
                _sldr.Value = (int)(_value * 32768 / _fsvVal);
                _txt.ForeColor = SystemColors.WindowText;
               
                int ptr = commsMngr.ptrWriteAt;
                commsMngr.cmdNum[ptr] = _cmdNum;
                commsMngr.itemNum[ptr] = _itemNum;
                commsMngr.data[ptr] = (Int32)((_value * 32767) / Convert.ToDouble(_fsvVal));
                commsMngr.ctrl[ptr] = this;
                commsMngr.ptrWriteAt++;
                commsMngr.TryNewCommsTask();
            }

            if (_guiTypePtr == 4 && _txt.ForeColor == System.Drawing.Color.SeaGreen)
            {
                int temp = (int)(_value * (Math.Pow(2, _qValue)));
                if (temp > 32767)
                {
                    temp = 32767;
                }
                _sldr.Value = temp;
                _txt.ForeColor = SystemColors.WindowText;

                int ptr = commsMngr.ptrWriteAt;
                commsMngr.cmdNum[ptr] = _cmdNum;
                commsMngr.itemNum[ptr] = _itemNum;
                commsMngr.data[ptr] = (Int32)(_value * Math.Pow(2, _qValue));
                commsMngr.ctrl[ptr] = this;
                commsMngr.ptrWriteAt++;
                commsMngr.TryNewCommsTask();
            }
        }


        public void SetDefault()
        {
            CheckValidity();
            SetText();
        }
    }
}