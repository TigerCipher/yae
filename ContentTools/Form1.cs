using System.Diagnostics;

namespace ContentTools
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void testButton_Click(object sender, EventArgs e)
        {
            Debug.WriteLine("This is just a test");
        }
    }
}