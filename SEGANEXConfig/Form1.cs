using System;
using System.IO;
using System.Web.Script.Serialization;
using System.Windows.Forms;

namespace SEGANEXConfig
{
    public partial class Form1 : Form
    {
        private string originalScreenType;
        private bool originalFullscreen;

        private readonly string[] validScreenTypes =
        {
            "16:9 (720p - 1280x720)",
            "4:3 (480i - 640x480 interlaced)"
        };

        public Form1()
        {
            InitializeComponent();
            LoadSettings();
        }

        public class ConfigSettings
        {
            public string screenType { get; set; }
            public bool fullscreen { get; set; }
        }

        private string GetConfigFolder()
        {
            // Artık üst klasörlere gitmek yok! 
            // Uygulama hangi klasördeyse, hemen yanındaki "config" klasörünü hedef alır.
            return Path.Combine(Application.StartupPath, "config");
        }

        private string GetConfigPath()
        {
            return Path.Combine(GetConfigFolder(), "config.json");
        }

        private ConfigSettings GetDefaultSettings()
        {
            return new ConfigSettings
            {
                screenType = "16:9 (720p - 1280x720)",
                fullscreen = false
            };
        }

        private void SaveSettingsToFile(ConfigSettings settings)
        {
            string configFolder = GetConfigFolder();
            string configPath = GetConfigPath();

            Directory.CreateDirectory(configFolder);

            string json =
        $@"{{
  ""screenType"": ""{settings.screenType}"",
  ""fullscreen"": {settings.fullscreen.ToString().ToLower()}
}}";

            File.WriteAllText(configPath, json);
        }

        private bool IsValidScreenType(string screenType)
        {
            foreach (string validType in validScreenTypes)
            {
                if (screenType == validType)
                    return true;
            }

            return false;
        }

        private void LoadSettings()
        {
            string configPath = GetConfigPath();
            ConfigSettings settings;

            if (!File.Exists(configPath))
            {
                settings = GetDefaultSettings();
                SaveSettingsToFile(settings);
            }
            else
            {
                try
                {
                    string json = File.ReadAllText(configPath);

                    JavaScriptSerializer serializer = new JavaScriptSerializer();
                    settings = serializer.Deserialize<ConfigSettings>(json);

                    if (!IsValidConfig(settings, json))
                        throw new Exception("Invalid config.");
                }
                catch
                {
                    MessageBox.Show(
                        "The config file is invalid or corrupted.\n\n" +
                        "Please edit the file manually with correct values,\n" +
                        "or delete it so the app can recreate it automatically.",
                        "SEGA NEX Config Error",
                        MessageBoxButtons.OK,
                        MessageBoxIcon.Error);

                    settings = GetDefaultSettings();
                }
            }

            if (comboScreenType.Items.Contains(settings.screenType))
                comboScreenType.SelectedItem = settings.screenType;
            else
                comboScreenType.SelectedIndex = 0;

            checkFullscreen.Checked = settings.fullscreen;

            originalScreenType = comboScreenType.SelectedItem?.ToString() ?? validScreenTypes[0];
            originalFullscreen = checkFullscreen.Checked;

            buttonSave.Enabled = false;
        }

        private void CheckForChanges()
        {
            string currentScreenType = comboScreenType.SelectedItem?.ToString() ?? "";
            bool currentFullscreen = checkFullscreen.Checked;

            buttonSave.Enabled =
                currentScreenType != originalScreenType ||
                currentFullscreen != originalFullscreen;
        }

        private bool HasChanges()
        {
            string currentScreenType = comboScreenType.SelectedItem?.ToString() ?? "";
            bool currentFullscreen = checkFullscreen.Checked;

            return currentScreenType != originalScreenType ||
                   currentFullscreen != originalFullscreen;
        }

        private void SaveCurrentSettings(bool showMessage)
        {
            ConfigSettings settings = new ConfigSettings
            {
                screenType = comboScreenType.SelectedItem?.ToString() ?? validScreenTypes[0],
                fullscreen = checkFullscreen.Checked
            };

            SaveSettingsToFile(settings);

            originalScreenType = settings.screenType;
            originalFullscreen = settings.fullscreen;

            buttonSave.Enabled = false;

            if (showMessage)
            {
                MessageBox.Show(
                    "Settings saved successfully.",
                    "SEGA NEX Config",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Information);
            }
        }

        private void buttonSave_Click(object sender, EventArgs e)
        {
            SaveCurrentSettings(true);
        }

        private void buttonExit_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            if (HasChanges())
            {
                var result = MessageBox.Show(
                    "You have unsaved changes. Do you want to save before exiting?",
                    "SEGA NEX Config",
                    MessageBoxButtons.YesNoCancel,
                    MessageBoxIcon.Warning);

                if (result == DialogResult.Yes)
                {
                    SaveCurrentSettings(false);
                }
                else if (result == DialogResult.Cancel)
                {
                    e.Cancel = true;
                }
            }

            base.OnFormClosing(e);
        }

        private bool IsValidConfig(ConfigSettings settings, string json)
        {
            if (settings == null)
                return false;

            if (string.IsNullOrWhiteSpace(settings.screenType))
                return false;

            if (!json.Contains(@"""fullscreen"""))
                return false;

            if (!IsValidScreenType(settings.screenType))
                return false;

            return true;
        }

        private void label1_Click(object sender, EventArgs e)
        {
        }

        private void comboScreenType_SelectedIndexChanged(object sender, EventArgs e)
        {
            CheckForChanges();
        }

        private void checkFullscreen_CheckedChanged(object sender, EventArgs e)
        {
            CheckForChanges();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }
    }
}