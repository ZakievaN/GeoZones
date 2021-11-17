using System;
using System.Runtime.InteropServices;
using NVGs.MathC;

namespace GeoZonesWrap
{

    internal struct gpc_vertex
    {
        public float x;
        public float y;
        public override string ToString()
        {
            return string.Format("x={0};y={1}", x, y);
        }
    }

    internal unsafe struct gpc_vertex_list
    {
        public int numvertices;
        public gpc_vertex* vertex;
    }

    internal unsafe struct gpc_polygon
    {
        public int numcontours;
        public int* hole;
        public gpc_vertex_list* contour;
    }

    public class GeoZones : IDisposable
    {
        IntPtr pGeoZones = IntPtr.Zero;

        [DllImport("GeoZones.dll")]
        static unsafe extern void InitGZ(void** pGeoZones, bool invetrXY);

        [DllImport("GeoZones.dll")]
        static unsafe extern void FreeGZ(void** pGeoZones);

        [DllImport("GeoZones.dll")]
        static unsafe extern void AddContour(void** pGeoZones, int typeCont, int num, gpc_vertex_list* cont);

        [DllImport("GeoZones.dll")]
        static unsafe extern void AddContourXY(void** pGeoZones, int typeCont, int num, int nPoint, float* x, float* y);

        [DllImport("GeoZones.dll")]
        static unsafe extern void InitContours(void** pGeoZones);

        [DllImport("GeoZones.dll")]
        static unsafe extern void GetZone(void** pGeoZones, int typeZone, out gpc_polygon* zone);

        public GeoZones(bool invetrXY)
        {
            unsafe
            {
                fixed (void* ppGeoZones = &pGeoZones)
                {
                    InitGZ((void**) ppGeoZones, invetrXY);
                }
            }
        }

        public GeoZones()
        {
            unsafe
            {
                fixed (void* ppGeoZones = &pGeoZones)
                {
                    InitGZ((void**)ppGeoZones, true);
                }
            }
        }
        // про освобождение unmanage ресурсов  http://msdn.microsoft.com/ru-ru/library/b1yfkh5e.aspx
        ~GeoZones()
        {
            unsafe
            {
                fixed (void* ppGeoZones = &pGeoZones)
                {
                    FreeGZ((void**)ppGeoZones);
                }
            }
        }

        unsafe public void AddContour(int typeCont, int num, int nPoint, float[] x, float[] y)
        {
            fixed (void* ppGeoZones = &pGeoZones)
            {
                fixed (float* px = &x[0], py = &y[0])
                {
                    AddContourXY((void**) ppGeoZones, typeCont, num, nPoint, px, py);
                }
            }
        }

        unsafe public void InitContours()
        {
            fixed (void* ppGeoZones = &pGeoZones)
            {
                InitContours((void**)ppGeoZones);
            }
        }

        unsafe public void GetZone(int typeZone, out Polygon poly)
        {
            poly = null;
            fixed (void* ppGeoZones = &pGeoZones)
            {
                gpc_polygon* p;
                GetZone((void**)ppGeoZones, typeZone, out p);
                if (p != null)
                {
                    poly = new Polygon();
                    for (int i = 0; i < p->numcontours; i++)
                    {
                        gpc_vertex_list* pvl = &p->contour[i];
                        float[] x = new float[pvl->numvertices];
                        float[] y = new float[pvl->numvertices];
                        for (int j = 0; j < pvl->numvertices; j++)
                        {
                            x[j] = pvl->vertex[j].x;
                            y[j] = pvl->vertex[j].y;
                        }
                        VertexList vl = new VertexList(x, y);
                        poly.AddContour(vl, p->hole[i] != 0);
                    }
                }
            }
        }

        #region IDisposable Support

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                unsafe
                {
                    fixed (void* ppGeoZones = &pGeoZones)
                    {
                        FreeGZ((void**)ppGeoZones);
                    }
                }
            }
        }
        #endregion
    }
}
