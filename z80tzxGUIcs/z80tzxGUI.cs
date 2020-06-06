using System;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Security;
using System.Windows.Forms;

public class OpenFileDialogForm : Form
{
    [STAThread]
    public static void Main()
    {
        Application.SetCompatibleTextRenderingDefault(false);
        Application.EnableVisualStyles();
        Application.Run(new OpenFileDialogForm());
    }

    private Label label0;
    private Label label1;
    private Button selectButton;
    private Button convertButton;
    private RadioButton radioButton0;
    private RadioButton radioButton1;
    private RadioButton radioButton2;
    private RadioButton radioButton3;
    private RadioButton radioButton4;
    private RadioButton radioButton5;
    private RadioButton radioButton6;
    private RadioButton radioButton7;
    private RadioButton radioButton8;
    private RadioButton radioButton9;
    private RadioButton radioButton10;
    private RadioButton radioButtonA;
   

    private OpenFileDialog openFileDialog1;
    private int[] speed= new int[11];
    private String strFilePath;

    public OpenFileDialogForm()
    {
        openFileDialog1 = new OpenFileDialog()
        {
            FileName = "Select file",
            Filter = "zx Snapshot files(*.sna;*.z80)|*.sna;*.z80|All files (*.*)|*.*",
            Title = "Open text file"
        };

	label0 = new Label()
	{
	    Size = new Size(140, 20),
 	    Location = new Point(10, 65),
 	    Text = "https://github/rcmolina"	    
	};

	label1 = new Label()
	{
	    Size = new Size(200, 20),
 	    Location = new Point(215, 65),
 	    Text = "Maxduino project companion help tool"
	};

        selectButton = new Button()
        {
            Size = new Size(80, 20),
            Location = new Point(440, 60),
            Text = "Select file"
        };
        selectButton.Click += new EventHandler(selectButton_Click);

        convertButton = new Button()
        {
            Size = new Size(80, 20),
            Location = new Point(520, 60),
            Text = "Convert"
        };
        convertButton.Click += new EventHandler(convertButton_Click);

	radioButton0 = new System.Windows.Forms.RadioButton()
	{
	    Size = new Size(50, 20),
            Location = new Point(16, 20),
	    Text = "1364"
	};
	radioButton0.CheckedChanged += new System.EventHandler(radioButton0_CheckedChanged);

	radioButton1 = new System.Windows.Forms.RadioButton()
	{
	    Size = new Size(50, 20),
            Location = new Point(66, 20),
	    Text = "2250"
	};
	radioButton1.CheckedChanged += new System.EventHandler(radioButton1_CheckedChanged);

	radioButton2 = new System.Windows.Forms.RadioButton()
	{
	    Size = new Size(50, 20),
            Location = new Point(116, 20),
	    Text = "3000"
	};
	radioButton2.CheckedChanged += new System.EventHandler(radioButton2_CheckedChanged);

	radioButton3 = new System.Windows.Forms.RadioButton()
	{
	    Size = new Size(50, 20),
            Location = new Point(166, 20),
	    Text = "3230"
	};
	radioButton3.CheckedChanged += new System.EventHandler(radioButton3_CheckedChanged);

	radioButton4 = new System.Windows.Forms.RadioButton()
	{
	    Size = new Size(50, 20),
            Location = new Point(216, 20),
	    Text = "3500"
	};
	radioButton4.CheckedChanged += new System.EventHandler(radioButton4_CheckedChanged);

	radioButton5 = new System.Windows.Forms.RadioButton()
	{
	    Size = new Size(50, 20),
            Location = new Point(266, 20),
	    Text = "4500"
	};
	radioButton5.CheckedChanged += new System.EventHandler(radioButton5_CheckedChanged);

	radioButton6 = new System.Windows.Forms.RadioButton()
	{
	    Size = new Size(50, 20),
            Location = new Point(316, 20),
	    Text = "5000"
	};
	radioButton6.CheckedChanged += new System.EventHandler(radioButton6_CheckedChanged);

	radioButton7 = new System.Windows.Forms.RadioButton()
	{
	    Size = new Size(50, 20),
            Location = new Point(366, 20),
	    Text = "5100"
	};
	radioButton7.CheckedChanged += new System.EventHandler(radioButton7_CheckedChanged);	

	radioButton8 = new System.Windows.Forms.RadioButton()
	{
	    Size = new Size(50, 20),
            Location = new Point(416, 20),
	    Text = "5500"
	};
	radioButton8.CheckedChanged += new System.EventHandler(radioButton8_CheckedChanged);

	radioButton9 = new System.Windows.Forms.RadioButton()
	{
	    Size = new Size(50, 20),
            Location = new Point(466, 20),
	    Text = "5800"
	};
	radioButton9.CheckedChanged += new System.EventHandler(radioButton9_CheckedChanged);

	radioButton10 = new System.Windows.Forms.RadioButton()
	{
	    Size = new Size(50, 20),
            Location = new Point(516, 20),
	    Text = "6000"
	};
	radioButton10.CheckedChanged += new System.EventHandler(radioButton10_CheckedChanged);

	radioButtonA = new System.Windows.Forms.RadioButton()
	{
	    Size = new Size(50, 20),
            Location = new Point(566, 20),
	    Text = "ALL"
	};
	radioButtonA.CheckedChanged += new System.EventHandler(radioButtonA_CheckedChanged);

        // 
        // Form1
        // 

        AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
        AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
        ClientSize = new System.Drawing.Size(619, 90);

        Controls.Add(label0);
        Controls.Add(label1);
        Controls.Add(selectButton);
        Controls.Add(convertButton);
        Controls.Add(radioButton0);
        Controls.Add(radioButton1);
        Controls.Add(radioButton2);
        Controls.Add(radioButton3);
        Controls.Add(radioButton4);
        Controls.Add(radioButton5);
        Controls.Add(radioButton6);
        Controls.Add(radioButton7);
        Controls.Add(radioButton8);
        Controls.Add(radioButton9);
        Controls.Add(radioButton10);
        Controls.Add(radioButtonA);

        Name = "Form1";
        Text = "z80tzxGUI";
        ResumeLayout(false);
        PerformLayout();

	///////////////////////
	// Default RadioButton
	radioButton7.Checked = true; speed[7] = 1;

    	// Enable drag and drop for this form
    	// (this can also be applied to any controls)
    	this.AllowDrop = true;
 
    	// Add event handlers for the drag & drop functionality
    	this.DragEnter += new DragEventHandler(Form_DragEnter);
    	this.DragDrop += new DragEventHandler(Form_DragDrop);
    }

    // This event occurs when the user drags over the form with 
    // the mouse during a drag drop operation 
    void Form_DragEnter(object sender, DragEventArgs e)
    {
    	// Check if the Dataformat of the data can be accepted
    	// (we only accept file drops from Explorer, etc.)
    	if (e.Data.GetDataPresent(DataFormats.FileDrop))
        	e.Effect = DragDropEffects.Copy; // Okay
    	else
        	e.Effect = DragDropEffects.None; // Unknown data, ignore it
 
    }
 
    // Occurs when the user releases the mouse over the drop target 
    void Form_DragDrop(object sender, DragEventArgs e)
    {
    	// Extract the data from the DataObject-Container into a string list
    	string[] FileList = (string[])e.Data.GetData(DataFormats.FileDrop, false);
 
    	// Do something with the data...
 
    	// For example add all files into a simple label control:
   	foreach (string File in FileList)
        	//this.label.Text += File + "\n";
		strFilePath = File;
    }

    private void selectButton_Click(object sender, EventArgs e)
    {
        if (openFileDialog1.ShowDialog() == DialogResult.OK)
        {
            try
            {
                var filePath = openFileDialog1.FileName;
                using (Stream str = openFileDialog1.OpenFile())
                {		
			// do something
			strFilePath =  filePath;
		}

            }
            catch (SecurityException ex)
            {
                MessageBox.Show("Security error.\n\nError message: {ex.Message}\n\n" +
                "Details:\n\n{ex.StackTrace}");
            }
        }
    }

    private void convertButton_Click(object sender, EventArgs e)
    {
	int i;		
	for (i=0; i<11; i++) {
		if (speed[i] == 1) Process.Start("Z802TZX3.exe", strFilePath + " -s "+ i.ToString());
	}
		
    }

    private void radioButton0_CheckedChanged (object sender, EventArgs e)
    {
	int i;	
        if (radioButton0.Checked == true) {
		for (i=0;i<11;i++) speed[i]=0;
		speed[0] = 1;	/* 1364 */
	}	
    }
    private void radioButton1_CheckedChanged (object sender, EventArgs e)
    {
	int i;	
        if (radioButton1.Checked == true) {
		for (i=0;i<11;i++) speed[i]=0;
		speed[1] = 1;	/* 2250 */
	}	
    }
    private void radioButton2_CheckedChanged (object sender, EventArgs e)
    {
	int i;	
        if (radioButton2.Checked == true) {
		for (i=0;i<11;i++) speed[i]=0;
		speed[2] = 1;	/* 3000 */
	}	
    }
    private void radioButton3_CheckedChanged (object sender, EventArgs e)
    {
	int i;	
        if (radioButton3.Checked == true) {
		for (i=0;i<11;i++) speed[i]=0;
		speed[3] = 1;	/* 3230 */
	}	
    }
    private void radioButton4_CheckedChanged (object sender, EventArgs e)
    {
	int i;	
        if (radioButton4.Checked == true) {
		for (i=0;i<11;i++) speed[i]=0;
		speed[4] = 1;	/* 3500 */
	}	
    }
    private void radioButton5_CheckedChanged (object sender, EventArgs e)
    {
	int i;	
        if (radioButton5.Checked == true) {
		for (i=0;i<11;i++) speed[i]=0;
		speed[5] = 1;	/* 4500 */
	}	
    }
    private void radioButton6_CheckedChanged (object sender, EventArgs e)
    {
	int i;	
        if (radioButton6.Checked == true) {
		for (i=0;i<11;i++) speed[i]=0;
		speed[6] = 1;	/* 5000 */
	}	
    }
    private void radioButton7_CheckedChanged (object sender, EventArgs e)
    {
	int i;	
        if (radioButton7.Checked == true) {
		for (i=0;i<11;i++) speed[i]=0;
		speed[7] = 1;	/* 5100 */
	}	
    }
    private void radioButton8_CheckedChanged (object sender, EventArgs e)
    {
	int i;	
        if (radioButton8.Checked == true) {
		for (i=0;i<11;i++) speed[i]=0;
		speed[8] = 1; /* 5500 */
	}	
    }
    private void radioButton9_CheckedChanged (object sender, EventArgs e)
    {
	int i;	
        if (radioButton9.Checked == true) {
		for (i=0;i<11;i++) speed[i]=0;
		speed[9] = 1; /* 5800 */
	}	
    }
    private void radioButton10_CheckedChanged (object sender, EventArgs e)
    {
	int i;	
        if (radioButton10.Checked == true) {
		for (i=0;i<11;i++) speed[i]=0;
		speed[9] = 1; /* 6000 */
	}	 
    }
    private void radioButtonA_CheckedChanged (object sender, EventArgs e)
    {
	int i;	
        if (radioButtonA.Checked == true) { 
	   for (i=0;i<11;i++) speed[i]=1;
        }
    }

}