//AddExpressionsWindowVars.js

// Remove all Expressions window vars
expRemoveAll();

// Add Expressions window variables
expAdd("EnableFlag");
expAdd("BackTicker");
expAdd("IsrTicker");
expAdd("DutyA", getQValue(24));
expAdd("VbusVcmd", getQValue(24));
expAdd("Gui_Vbus", getQValue(6));
expAdd("Gui_VrectAvg", getQValue(6));
expAdd("Gui_VrectRMS", getQValue(6));
expAdd("Gui_Freq_Vin", getQValue(6));
expAdd("start_flag");
expAdd("run_flag");
expAdd("run_motor");
expAdd("SpeedRef", getQValue(24));
expAdd("Ipfc_fltr", getQValue(24));
