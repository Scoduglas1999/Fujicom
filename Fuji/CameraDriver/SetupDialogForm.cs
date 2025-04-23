using ASCOM.Utilities;
using System;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace ASCOM.ScdouglasFujifilm.Camera
{
    [ComVisible(false)] // Form not registered for COM!
    public partial class SetupDialogForm : Form
    {
        // Removed COM port constant: const string NO_PORTS_MESSAGE = "No COM ports found";
        TraceLogger tl; // Holder for a reference to the driver's trace logger

        public SetupDialogForm(TraceLogger tlDriver)
        {
            InitializeComponent();

            // Save the provided trace logger for use within the setup dialogue
            tl = tlDriver;

            // Initialise current values of user settings from the ASCOM Profile
            InitUI();
        }

        private void CmdOK_Click(object sender, EventArgs e) // OK button event handler
        {
            // Place any validation constraint checks here

            // Save the trace logger state
            tl.Enabled = chkTrace.Checked;
            tl.LogMessage("Setup OK", $"Trace logger state set to: {tl.Enabled}");

            // Removed code that accessed CameraHardware.comPort and saved COM port setting
        }

        private void CmdCancel_Click(object sender, EventArgs e) // Cancel button event handler
        {
            Close();
        }

        private void BrowseToAscom(object sender, EventArgs e) // Click on ASCOM logo event handler
        {
            try
            {
                System.Diagnostics.Process.Start("https://ascom-standards.org/");
            }
            catch (Win32Exception noBrowser)
            {
                if (noBrowser.ErrorCode == -2147467259)
                    MessageBox.Show(noBrowser.Message);
            }
            catch (Exception other)
            {
                MessageBox.Show(other.Message);
            }
        }

        private void InitUI()
        {
            // Set the trace checkbox based on the current state from the profile
            chkTrace.Checked = tl.Enabled;
            tl.LogMessage("InitUI", $"Set UI controls to Trace: {chkTrace.Checked}");

            // Removed code that populated and selected COM ports in comboBoxComPort

            // TODO: Add code here later to detect connected Fujifilm cameras
            // and populate a ListBox or ComboBox for camera selection.
            // For now, the form only handles the Trace checkbox.
        }

        private void SetupDialogForm_Load(object sender, EventArgs e)
        {
            // Bring the setup dialogue to the front of the screen
            if (WindowState == FormWindowState.Minimized)
                WindowState = FormWindowState.Normal;
            else
            {
                TopMost = true;
                Focus();
                BringToFront();
                TopMost = false;
            }
        }
    }
}