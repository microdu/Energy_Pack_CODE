using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace GUI_Template
{
    public partial class HexConverter : Form
    {
        string hex2000;
        string coffFile;
        string destination;
        System.Windows.Forms.TextBox LoadHexTxt;


        public HexConverter(System.Windows.Forms.TextBox pathTextbox)
        {
            InitializeComponent();
            LoadHexTxt = pathTextbox;

            //**Override Designer Code
            openFileDialog1.InitialDirectory = Properties.FileLocationSettings.Default.Hex2000_Location;
            openFileDialog2.InitialDirectory = Properties.FileLocationSettings.Default.Coff_Location;
            //folderBrowserDialog1.SelectedPath = Properties.FileLocationSettings.Default.A00_Destination;

            textBox1.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::GUI_Template.Properties.FileLocationSettings.Default, "Hex2000_Location", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            textBox1.Text = global::GUI_Template.Properties.FileLocationSettings.Default.Hex2000_Location;
            textBox2.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::GUI_Template.Properties.FileLocationSettings.Default, "Coff_Location", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            textBox2.Text = global::GUI_Template.Properties.FileLocationSettings.Default.Coff_Location;
            //textBox3.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::GUI_Template.Properties.FileLocationSettings.Default, "A00_Destination", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            //textBox3.Text = global::GUI_Template.Properties.FileLocationSettings.Default.A00_Destination;
            //**
        }

        private void button1_Click(object sender, EventArgs e)
        {
            openFileDialog1.ShowDialog();
        }

        private void openFileDialog1_FileOk(object sender, CancelEventArgs e)
        {
            textBox1.Text = openFileDialog1.FileName;
            hex2000 = textBox1.Text;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            openFileDialog2.ShowDialog();
        }

        private void openFileDialog2_FileOk(object sender, CancelEventArgs e)
        {
            textBox2.Text = openFileDialog2.FileName;
            coffFile = textBox2.Text;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (textBox1.Text != "" && textBox2.Text != "")
            {
                string k = textBox2.Text;
                int l = k.LastIndexOf('\\');
                string coffFile;
                string workingDir;
                coffFile = k.Remove(0, l+1);
                workingDir = k.Remove(l);
                System.Diagnostics.Process myProcess = new System.Diagnostics.Process();
                myProcess.StartInfo.FileName = textBox1.Text;
                myProcess.StartInfo.Arguments = coffFile + " -boot -sci8 -a";
                myProcess.StartInfo.WorkingDirectory = workingDir;
                myProcess.StartInfo.CreateNoWindow = true;
                myProcess.StartInfo.LoadUserProfile = true;
                //myProcess.StartInfo.UseShellExecute = false;
                //myProcess.StartInfo.WorkingDirectory = @"C:\tidcs\";

                //myProcess.StartInfo.Arguments = "calc.exe";

                int lastPeriodIndex = k.LastIndexOf('.');
                string HexPath = k.Remove(lastPeriodIndex);
                HexPath = HexPath + ".a00";
                LoadHexTxt.Text = HexPath;
                myProcess.Start();
                

            }
        }


        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            Properties.FileLocationSettings.Default.Save();
            this.Hide();
            e.Cancel = true;
        }
    }
}