namespace SEGANEXConfig
{
    partial class Form1
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.label1 = new System.Windows.Forms.Label();
            this.checkFullscreen = new System.Windows.Forms.CheckBox();
            this.comboScreenType = new System.Windows.Forms.ComboBox();
            this.buttonExit = new System.Windows.Forms.Button();
            this.buttonSave = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(24, 44);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(68, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Screen Type";
            this.label1.Click += new System.EventHandler(this.label1_Click);
            // 
            // checkFullscreen
            // 
            this.checkFullscreen.AutoSize = true;
            this.checkFullscreen.Location = new System.Drawing.Point(150, 79);
            this.checkFullscreen.Name = "checkFullscreen";
            this.checkFullscreen.Size = new System.Drawing.Size(74, 17);
            this.checkFullscreen.TabIndex = 1;
            this.checkFullscreen.Text = "Fullscreen";
            this.checkFullscreen.UseVisualStyleBackColor = true;
            this.checkFullscreen.CheckedChanged += new System.EventHandler(this.checkFullscreen_CheckedChanged);
            // 
            // comboScreenType
            // 
            this.comboScreenType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboScreenType.FormattingEnabled = true;
            this.comboScreenType.Items.AddRange(new object[] {
            "16:9 (720p - 1280x720)",
            "4:3 (480i - 640x480 interlaced)"});
            this.comboScreenType.Location = new System.Drawing.Point(98, 41);
            this.comboScreenType.Name = "comboScreenType";
            this.comboScreenType.Size = new System.Drawing.Size(258, 21);
            this.comboScreenType.TabIndex = 2;
            this.comboScreenType.SelectedIndexChanged += new System.EventHandler(this.comboScreenType_SelectedIndexChanged);
            // 
            // buttonExit
            // 
            this.buttonExit.Location = new System.Drawing.Point(69, 126);
            this.buttonExit.Name = "buttonExit";
            this.buttonExit.Size = new System.Drawing.Size(75, 23);
            this.buttonExit.TabIndex = 3;
            this.buttonExit.Text = "Exit";
            this.buttonExit.UseVisualStyleBackColor = true;
            this.buttonExit.Click += new System.EventHandler(this.buttonExit_Click);
            // 
            // buttonSave
            // 
            this.buttonSave.Location = new System.Drawing.Point(224, 126);
            this.buttonSave.Name = "buttonSave";
            this.buttonSave.Size = new System.Drawing.Size(95, 23);
            this.buttonSave.TabIndex = 4;
            this.buttonSave.Text = "Save Settings";
            this.buttonSave.UseVisualStyleBackColor = true;
            this.buttonSave.Click += new System.EventHandler(this.buttonSave_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(384, 161);
            this.Controls.Add(this.buttonSave);
            this.Controls.Add(this.buttonExit);
            this.Controls.Add(this.comboScreenType);
            this.Controls.Add(this.checkFullscreen);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "SEGA NEX Config";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox checkFullscreen;
        private System.Windows.Forms.ComboBox comboScreenType;
        private System.Windows.Forms.Button buttonExit;
        private System.Windows.Forms.Button buttonSave;
    }
}

