using System;
using System.Drawing;
using System.Windows.Forms;

namespace FindFriendApp
{
    public class MainWindow : Form
    {
        public MainWindow()
        {
            Text = "福瑞扩列计划 - Windows";
            Size = new Size(420, 600);
            StartPosition = FormStartPosition.CenterScreen;
            BackColor = Color.White;

            Panel topNav = new Panel
            {
                Dock = DockStyle.Top,
                Height = 60,
                BackColor = Color.White
            };

            Button btnHome = new Button { Text = "首页", Location = new Point(20, 10) };
            Button btnChat = new Button { Text = "聊天", Location = new Point(140, 10) };
            Button btnMine = new Button { Text = "我的", Location = new Point(260, 10) };

            topNav.Controls.Add(btnHome);
            topNav.Controls.Add(btnChat);
            topNav.Controls.Add(btnMine);

            Panel contentPanel = new Panel
            {
                Dock = DockStyle.Fill,
                BackColor = Color.White
            };
            contentPanel.Controls.Add(new PlaceholderLabel("主界面框架就绪"));

            Controls.Add(topNav);
            Controls.Add(contentPanel);
        }
    }
}
