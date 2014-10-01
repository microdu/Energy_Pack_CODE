using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace GUI_Template
{
    public partial class GraphForm : Form
    {
        public GuiStartUp parentForm;
        int oldHeight, oldWidth;


        public GraphForm(GuiStartUp parent)
        {
            InitializeComponent();
            parentForm = parent;
        }

        private void GraphForm_Activated(object sender, EventArgs e)
        {
            //***can remove null checker to save bandwidth if using Graphs
            if (parentForm.CanFocus == true && parentForm.TSGraphGroup[0] != null)  
            {
                for (int i = 0; i < parentForm.TSGraphGroup.Length; i++)
                {
                    parentForm.TSGraphGroup[i].GraphBuffer();
                }
            }
        }

        private void GraphForm_ResizeBegin(object sender, EventArgs e)
        {
            oldHeight = Size.Height;
            oldWidth = Size.Width;
        }

        private void GraphForm_ResizeEnd(object sender, EventArgs e)
        {
            graphPane.Height = graphPane.Height + (Size.Height - oldHeight);
            graphPane.Width = graphPane.Width + (Size.Width - oldWidth);
            parentForm.TSGraphGroup[0].CreateGraphics();
        }

        private void GraphForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            //parentForm.btnShowGraph_Click(sender, e);
            e.Cancel = true;
        }

        private void keepGraphOnTopToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (this.TopMost == true)
            {
                this.TopMost = false;
                keepGraphOnTopToolStripMenuItem.Text = "Keep Graph On Top";
            }
            else
            {
                this.TopMost = true;
                keepGraphOnTopToolStripMenuItem.Text = "Disable Graph On Top";
            }
        }        
    }
}