﻿using GMap.NET;
using GMap.NET.MapProviders;
using GMap.NET.WindowsForms;
using System.Collections.Generic;
using System.Data;


namespace GoodsTracker
{
    class Route
    {
        List<Behavior>  behaviors;
        GDirections     direction;
        GMapRoute       mapRoute;

        DataTable           dt;
        List<PointLatLng>   points;
        string              name;

        public DataTable Dt { get => dt; set => dt = value; }
        public List<PointLatLng> Points { get => points; set => points = value; }
        public string Name { get => name; set => name = value; }
        public GDirections Direction { get => direction; set => direction = value; }
        public GMapRoute MapRoute { get => mapRoute; set => mapRoute = value; }
        internal List<Behavior> Behaviors { get => behaviors; set => behaviors = value; }

        internal Route(string n)
        {
            name = n;

            points = new List<PointLatLng>();

            dt = new DataTable();

            dt.Columns.Add(new DataColumn("Loc.", typeof(string)));
            dt.Columns.Add(new DataColumn("Latitude", typeof(double)));
            dt.Columns.Add(new DataColumn("Longitude", typeof(double)));

            behaviors = new List<Behavior>();
        }

        internal void startTrip(PointLatLng point)
        {
            points.Add(point);
            dt.Rows.Add(string.Format("Start{0}", dt.Rows.Count), point.Lat, point.Lng);
        }

        internal void stopTrip(PointLatLng point)
        {
            points.Add(point);
            dt.Rows.Add(string.Format("Stop{0}", dt.Rows.Count), point.Lat, point.Lng);
        }

        internal void clear()
        {
            points.Clear();
            dt.Clear();
            behaviors.Clear();
        }

        internal void createRoute(PointLatLng start, PointLatLng stop)
        {
            startTrip(start);

            stopTrip(stop);

            var routedirection = GMapProviders.GoogleMap.GetDirections(out direction, start, stop, false, false, false, false, false);

            mapRoute = new GMapRoute(direction.Route, name);
        }

        internal void createRoute()
        {
            createRoute(points[0], points[points.Count - 1]);
        }

        internal List<Behavior> getBehaviorFiltered(int i)
        {
            List<Behavior> ret = new List<Behavior>(behaviors);

            // i=0 nao filtra nada
            // OK (i=1) entao remove os NOK
            // NOK(i=2) entao remove os OK
            if (i != 0)
            {
                foreach (Behavior b in behaviors)
                {
                    if ((i == 1 && !b.OK()) || i == 2 && b.OK())
                    {
                        ret.Remove(b);
                    }
                }
            }

            return ret;
        }

        internal void registerBehavior(Behavior b)
        {
            if (b != null)
            {
                behaviors.Add(b);
            }
        }
    }
}