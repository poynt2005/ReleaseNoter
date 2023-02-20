using System;

namespace ClipboardCopier
{
    public class ClipboardCopier
    {
        [STAThread]
        public static bool Copier(String htmlText) {
            String htmlHeader = @"Version0.9
StartHTML:00000001
EndHTML:00000002
StartFragment:00000003
EndFragment:00000004
";
            System.Text.StringBuilder sb = new System.Text.StringBuilder();

            sb.AppendLine(htmlHeader);
            sb.AppendLine("<!DOCTYPE html>");
            sb.AppendLine("<html>");
            sb.AppendLine("<body>");

            int fragmentStart = System.Text.Encoding.UTF8.GetByteCount(sb.ToString());

            sb.Append(htmlText);

            int fragmentEnd = System.Text.Encoding.UTF8.GetByteCount(sb.ToString());

            sb.AppendLine("<!--EndFragment-->");
            sb.AppendLine("</body>");
            sb.Append("</html>");

            sb.Replace("00000001", htmlHeader.Length.ToString("D8"));
            sb.Replace("00000002", System.Text.Encoding.UTF8.GetByteCount(sb.ToString()).ToString("D8"));
            sb.Replace("00000003", fragmentStart.ToString("D8"));
            sb.Replace("00000004", fragmentEnd.ToString("D8"));

            System.Windows.Forms.IDataObject dataObject = new System.Windows.Forms.DataObject();

            Func<String, String> HtmlTagStrip = inputHtmlText =>
            {
                string pattern = @"<(.|\n)*?>";
                return System.Text.RegularExpressions.Regex.Replace(inputHtmlText, pattern, String.Empty);
            };

            /** ref: https://www.cnblogs.com/08shiyan/p/2078588.html */
            var u8Enc = System.Text.Encoding.UTF8;

            dataObject.SetData(System.Windows.Forms.DataFormats.Html, new System.IO.MemoryStream(u8Enc.GetBytes(sb.ToString())));
            dataObject.SetData(System.Windows.Forms.DataFormats.Text, HtmlTagStrip(htmlText));
            dataObject.SetData(System.Windows.Forms.DataFormats.UnicodeText, HtmlTagStrip(htmlText));

            try
            {
                System.Windows.Forms.Clipboard.SetDataObject(dataObject, true);
            }
            catch (Exception ex) {
                return false;
            }

            return true;
        }
    }
}
