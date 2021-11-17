using System;
using System.Windows.Forms;
using GeoZonesWrap;
using NVGs.MathC;

namespace GeoZonesTest
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            var gz = new GeoZones(false);

            /*петля*/
            //gz.AddContour(2, 1, 5, new[] { 1, 4, 4, 2f, 2 }, new[] { 2, 2, 4, 4f, 1 });

            /*замкнутый контур в общей точке*/
            //gz.AddContour(2, 1, 5, new[] { 1, 4, 4, 1f, 1 }, new[] { 1, 1, 4, 4f, 1 });
            //gz.AddContour(2, 2, 2, new[] { 1, 4f }, new[] { 2, 2f });

            /*замкнутый контур на отрезке*/
            //gz.AddContour(2, 1, 5, new[] { 1, 5, 5, 2f, 2 }, new[] { 2, 2, 5, 5f, 2 });

            gz.AddContour(2, 1, 6, new[] { 2, 7, 7, 2f, 2, 2 }, new[] { 6, 6, 3, 3f, 1, 6 });

            /*6*/
            //gz.AddContour(2, 1, 5, new[] { 1, 6, 8f, 8, 1 }, new[] { 3, 3f, 5, 7, 7 });
            //gz.AddContour(4, 2, 4, new[] { 2, 2, 2f, 2 }, new[] { 2, 5f, 7, 9 });

            /*10*/
            //gz.AddContour(2, 1, 3, new[] { 2, 4, 2f }, new[] { 1, 3f, 3 });
            //gz.AddContour(2, 2, 3, new[] { 1f, 4, 4 }, new[] { 1, 1f, 4 });

            /*1*/
            //gz.AddContour(2, 1, 4, new[] { 2, 5, 1, 2f }, new[] { 7, 1, 4, 1f });
            //gz.AddContour(4, 2, 6, new[] { 5, 3, 1, 1f, 4, 6 }, new[] { 2, 1, 2, 6f, 7, 5 });

            /*2*/
            //gz.AddContour(2, 1, 5, new[] { 2, 9, 4, 8f, 2 }, new[] { 4, 9, 12, 1f, 8 });
            //gz.AddContour(4, 2, 5, new[] { 4, 10, 10, 1f, 4 }, new[] { 3, 4, 11, 10f, 3 });

            /*3*/
            //gz.AddContour(2, 1, 6, new[] { 8, 4, 4, 4f, 5, 5 }, new[] { 1, 1, 3, 5f, 7, 9 });
            //gz.AddContour(4, 2, 6, new[] { 3, 6, 7, 6f, 3, 1 }, new[] { 1, 2, 4, 7f, 8, 6 });

            /*4*/
            //gz.AddContour(4, 2, 7, new[] { 1f,4,5,7,8,9,7 }, new[] { 3f,3,4,5,6,9,11 });
            //gz.AddContour(2, 1, 9, new[] { 4f,3,3,4,6,8,10,12,14 }, new[] { 1f,4,6,8,9,8,6,3,3 });

            /*5*/
            //gz.AddContour(2, 2, 5, new[] { 4f,6,3,1,4 }, new[] { 2f,7,7,5,2 });
            //gz.AddContour(4, 1, 3, new[] { 2f,2,2 }, new[] { 2f,4,7 });

            gz.InitContours();
            Polygon p;
            gz.GetZone(4, out p);
        }
    }
}
