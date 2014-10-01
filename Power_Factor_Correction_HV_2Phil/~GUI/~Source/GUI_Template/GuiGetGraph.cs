using System;
using System.Collections.Generic;
using System.Text;

namespace GUI_Template
{
    public class GuiGetGraph : Object
    {
        public GuiGetArray[] arrayGraph;
        public GuiGetVar[] varGraph;
        public int[] dataBuffer;
        public System.Windows.Forms.CheckBox chbxArray;
        public CommsManager commsMngr;



        #region Constructors

        public GuiGetGraph(GuiGetVar[] variableItems, GuiGetArray[] arrayItems, System.Windows.Forms.CheckBox[] chbxItems)
        {
            varGraph = variableItems;
            arrayGraph = arrayItems;
            //chbxArray = chbxItems;



        }



        #endregion

        public void Update()
        {

        }




    }
}
