using System.Drawing;
using System.Windows.Forms;

namespace FindFriendApp
{
    public class PlaceholderLabel : Label
    {
        public PlaceholderLabel(string text)
        {
            Text = text;
            Font = new Font("Microsoft YaHei", 14);
            Location = new Point(100, 200);
        }
    }
}
