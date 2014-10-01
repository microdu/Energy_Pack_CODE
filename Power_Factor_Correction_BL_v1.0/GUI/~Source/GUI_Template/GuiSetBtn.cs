using System;
using System.Collections.Generic;
using System.Text;

namespace GUI_Template 
{
    public class GuiSetBtn : Object
    {
        private System.Windows.Forms.Button _btn;
        private byte _cmdNum;
        private byte _itemNum;
        private string _btnLabelWhenOn;
        private string _btnLabelWhenOff;
        private System.Drawing.Color _colorWhenOn;
        private System.Drawing.Color _colorWhenOff;

        private int _value;
        public int value
        {
            get { return _value; }
            set { if(value == 0 || value == 1) _value = value; }
        }

        public CommsManager commsMngr;


        #region Constructors
        public GuiSetBtn(System.Windows.Forms.Button button, byte itemNumber)
        {
            _btn = button;
            _itemNum = itemNumber;
            _btnLabelWhenOn = "On";
            _btnLabelWhenOff = "Off";
            _colorWhenOn = System.Drawing.Color.SeaGreen;
            _colorWhenOff = System.Drawing.Color.Crimson;
                 
            if (_btn.Text == _btnLabelWhenOn)
            {
                _value = 1;
            }
            else
            {
                _value = 0;
                _btn.Text = _btnLabelWhenOff;
            }
            
            _btn.Tag = (int)_itemNum;

            _cmdNum = 0x02;
        }

        public GuiSetBtn(System.Windows.Forms.Button button, string btnLabelTrue, string btnLabelFalse, byte itemNumber)
        {
            _btn = button;
            _itemNum = itemNumber;
            _btnLabelWhenOn = btnLabelTrue;
            _btnLabelWhenOff = btnLabelFalse;
            _colorWhenOn = System.Drawing.Color.Black;
            _colorWhenOff = System.Drawing.Color.Black;


            if (_btn.Text == _btnLabelWhenOn)
            {
                _value = 1;
            }
            else
            {
                _value = 0;
                _btn.Text = _btnLabelWhenOff;
            }

            _btn.Tag = (int)_itemNum;

            _cmdNum = 0x02;
        }
        #endregion


        public void SetButton()
        {
            if (_value == 0)
            {
                _value = 1;
                _btn.Text = _btnLabelWhenOn;
                _btn.ForeColor = _colorWhenOn; 
            }
            else
            {
                _value = 0;
                _btn.Text = _btnLabelWhenOff;
                _btn.ForeColor = _colorWhenOff;
            }
            int ptr = commsMngr.ptrWriteAt;
            commsMngr.cmdNum[ptr] = _cmdNum;
            commsMngr.itemNum[ptr] = _itemNum;
            commsMngr.data[ptr] = _value;
            commsMngr.ctrl[ptr] = this;
            commsMngr.ptrWriteAt++;
            commsMngr.TryNewCommsTask();
        }


        public void SetDefault()
        {
            if (_btn.Text == _btnLabelWhenOn)
            {
                _value = 1;            
                _btn.ForeColor = _colorWhenOn;
            }
            else
            {
                _btn.Text = _btnLabelWhenOff;
                _value = 0;
                _btn.ForeColor = _colorWhenOff;
            }
            int ptr = commsMngr.ptrWriteAt;
            commsMngr.cmdNum[ptr] = _cmdNum;
            commsMngr.itemNum[ptr] = _itemNum;
            commsMngr.data[ptr] = _value;
            commsMngr.ctrl[ptr] = this;
            commsMngr.ptrWriteAt++;
            commsMngr.TryNewCommsTask();
        }
    }
}


