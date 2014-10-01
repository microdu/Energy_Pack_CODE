namespace GUI_Template
{
    partial class GraphForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(GraphForm));
            this.graphPane = new System.Windows.Forms.Panel();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.keepGraphOnTopToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.contextMenuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // graphPane
            // 
            this.graphPane.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.graphPane.ContextMenuStrip = this.contextMenuStrip1;
            this.graphPane.Location = new System.Drawing.Point(12, 12);
            this.graphPane.Name = "graphPane";
            this.graphPane.Size = new System.Drawing.Size(618, 392);
            this.graphPane.TabIndex = 0;
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.keepGraphOnTopToolStripMenuItem});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.ShowImageMargin = false;
            this.contextMenuStrip1.Size = new System.Drawing.Size(144, 26);
            // 
            // keepGraphOnTopToolStripMenuItem
            // 
            this.keepGraphOnTopToolStripMenuItem.Name = "keepGraphOnTopToolStripMenuItem";
            this.keepGraphOnTopToolStripMenuItem.Size = new System.Drawing.Size(143, 22);
            this.keepGraphOnTopToolStripMenuItem.Text = "Keep Graph On Top";
            this.keepGraphOnTopToolStripMenuItem.Click += new System.EventHandler(this.keepGraphOnTopToolStripMenuItem_Click);
            // 
            // GraphForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(642, 416);
            this.Controls.Add(this.graphPane);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "GraphForm";
            this.Text = "Two-Channel Graph";
            this.ResizeBegin += new System.EventHandler(this.GraphForm_ResizeBegin);
            this.Activated += new System.EventHandler(this.GraphForm_Activated);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.GraphForm_FormClosing);
            this.ResizeEnd += new System.EventHandler(this.GraphForm_ResizeEnd);
            this.contextMenuStrip1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        public System.Windows.Forms.Panel graphPane;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem keepGraphOnTopToolStripMenuItem;


    }
}