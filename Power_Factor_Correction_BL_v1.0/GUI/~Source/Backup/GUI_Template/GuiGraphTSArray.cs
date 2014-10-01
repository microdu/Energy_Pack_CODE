using System;
using System.Collections.Generic;
using System.Text;

namespace GUI_Template
{
    public class GuiGraphTSArray : Object
    {
        private int[] _dataArray;
        private int _dataArraySize;
        
        private byte _cmdNum;
        private byte _itemNum;
        private int _qValue;
        private bool _autoScaleEnable;
        private int _maxInBuffer;

        public bool isGraphing;
        public byte[] byteBuffer;
        
        private System.Windows.Forms.Panel pnlGraph;

        private float delta;
        private float pointAtX;
        private System.Drawing.Pen redPen;
        private System.Drawing.Pen blackPen;
        private System.Drawing.Graphics formGraphics;
        private System.Windows.Forms.Label _lblASMin;
        private System.Windows.Forms.Label _lblASMax;
        private System.Windows.Forms.CheckBox _chbAutoScaleEnable;

        public CommsManager commsMngr;


        #region Constructors

        public GuiGraphTSArray(int sizeOfBuffer, byte itemNumber, System.Windows.Forms.Panel drawingPanel)
        {
            _dataArraySize = sizeOfBuffer;
            _dataArray = new int[_dataArraySize];
            _itemNum = itemNumber;
            _cmdNum = 0x05;
            pnlGraph = drawingPanel;
            _autoScaleEnable = false;

            redPen = new System.Drawing.Pen(System.Drawing.Color.Red);
            blackPen = new System.Drawing.Pen(System.Drawing.Color.Black);
            CreateGraphics();
        }

        public GuiGraphTSArray(int sizeOfBuffer, byte itemNumber, System.Windows.Forms.Panel drawingPanel, System.Windows.Forms.Label lblASMin, System.Windows.Forms.Label lblASMax, int qValue, System.Windows.Forms.CheckBox chbAutoScaleEnable)
        {
            _dataArraySize = sizeOfBuffer;
            _dataArray = new int[_dataArraySize];
            _itemNum = itemNumber;
            _cmdNum = 0x05;
            _chbAutoScaleEnable = chbAutoScaleEnable;
            _lblASMax = lblASMax;
            _lblASMin = lblASMin;
            _qValue = qValue;

            pnlGraph = drawingPanel;

            redPen = new System.Drawing.Pen(System.Drawing.Color.Red);
            blackPen = new System.Drawing.Pen(System.Drawing.Color.Black);

            _lblASMax.Text = ((double)((double)32767 / (Math.Pow(2.0, _qValue)))).ToString("0");
            _lblASMin.Text = ((double)((double)(-32768) / (Math.Pow(2, _qValue)))).ToString("0");

            CreateGraphics();
        }

        #endregion


        public void CreateGraphics()
        {
            formGraphics = pnlGraph.CreateGraphics();
            delta = (float)pnlGraph.Size.Width / (float)_dataArraySize;
            isGraphing = false;
            GraphBuffer();
        }


        public void RequestBuffer()
        {
            //byte[] amountToGet = BitConverter.GetBytes(bufferSize);
            //int[] RxVarData = new int[2];

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
            byteBuffer = buffer;

            commsMngr.TaskComplete();
            _maxInBuffer = 1;

            for (int i = 0; i < _dataArraySize; i++)
            {
                _dataArray[i] = byteBuffer[i * 2] + byteBuffer[2 * i + 1] * 256;
                if (_dataArray[i] < 32768 && _dataArray[i] > _maxInBuffer)
                    _maxInBuffer = _dataArray[i];
                else if (_dataArray[i] > 32767 && 65545 - _dataArray[i] > _maxInBuffer)
                    _maxInBuffer = 65545 - _dataArray[i];
            }
            GraphBuffer();
        }


        public void GraphBuffer()
        {
            if (isGraphing == false)
            {
                isGraphing = true;
                pointAtX = delta;
                formGraphics.Clear(System.Drawing.Color.White);
                formGraphics.DrawLine(blackPen, 0, pnlGraph.Size.Height / 2, pnlGraph.Size.Width, pnlGraph.Size.Height / 2);

                if (_chbAutoScaleEnable != null)
                {
                    _autoScaleEnable = _chbAutoScaleEnable.Checked;
                }

                if (_autoScaleEnable == false)
                {
                    for (int i = 2; i < _dataArraySize; i++)
                    {
                        formGraphics.DrawLine(redPen, pointAtX, pnlGraph.Size.Height / 2 - (_dataArray[i - 1] * (pnlGraph.Size.Height / 2) / 32767), pointAtX + delta, pnlGraph.Size.Height / 2 - (_dataArray[i] * (pnlGraph.Size.Height / 2) / 32767));
                        pointAtX = pointAtX + delta;
                    }
                    if (_lblASMax != null)
                    {
                        string formatString;
                        if (_qValue < 6)
                        {
                            formatString = "0";
                        }
                        else
                        {
                            formatString = "0.00";
                        }
                        if (commsMngr != null)
                        {
                            _lblASMax.Invoke(new EventHandler(delegate
                            {
                                _lblASMax.Text = ((double)((double)32767 / (Math.Pow(2.0, _qValue)))).ToString(formatString);
                                _lblASMin.Text = ((double)((double)(-32768) / (Math.Pow(2, _qValue)))).ToString(formatString);
                            }));
                        }
                    }
                }
                else
                {
                    
                    for (int i = 2; i < _dataArraySize && _maxInBuffer != 0; i++)
                    {
                        double y1, y2;

                        //check if positive number
                        if (_dataArray[i - 1] < 32768)
                        {
                            //scale and invert
                            y1 = pnlGraph.Size.Height / 2 - ((double)_dataArray[i - 1] / 32767.0 * (double)pnlGraph.Size.Height / 2.0) * (32767.0 / (double)_maxInBuffer);
                        }
                        else
                        {
                            //offset raw, scale and add offset
                            y1 = pnlGraph.Size.Height / 2 + ((double)(65535 - _dataArray[i - 1]) / 32767.0 * (double)pnlGraph.Size.Height / 2.0) * (32767.0 / (double)_maxInBuffer);
                        }

                        //check if positive number
                        if (_dataArray[i] < 32768)
                        {
                            //scale and invert
                            y2 = pnlGraph.Size.Height / 2 - ((double)_dataArray[i] / 32767.0 * (double)pnlGraph.Size.Height / 2.0) * (32767.0 / (double)_maxInBuffer);
                        }
                        else
                        {
                            //offset raw, scale and add offset
                            y2 = pnlGraph.Size.Height / 2 + ((double)(65535 - _dataArray[i]) / 32767.0 * (double)pnlGraph.Size.Height / 2.0) * (32767.0 / (double)_maxInBuffer);
                        }

                        formGraphics.DrawLine(redPen, (float)pointAtX, (float)y1, (float)pointAtX + (float)delta, (float)y2);

                        pointAtX = pointAtX + delta;
                    }
                    if (_lblASMax != null)
                    {
                        string formatString;
                        if (_qValue < 6)
                        {
                            formatString = "0";
                        }
                        else
                        {
                            formatString = "0.00";
                        }
                        _lblASMax.Invoke(new EventHandler(delegate
                        {
                            _lblASMax.Text = ((double)((double)(_maxInBuffer - 1) / (Math.Pow(2.0, _qValue)))).ToString(formatString);
                            _lblASMin.Text = ((double)((double)(-_maxInBuffer) / (Math.Pow(2, _qValue)))).ToString(formatString);
                        }));
                    }

                    
                }
                isGraphing = false;
            }
        }
    }
}
