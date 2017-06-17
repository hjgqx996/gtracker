﻿using System.Collections.Generic;

namespace GoodsTracker
{
    class TrackerController
    {
        Tracker         tracker;
        List<Behavior>  behaviors;
        List<Fence>     fences;
        List<Route>     routes;

        internal List<Behavior> Behaviors { get => behaviors; set => behaviors = value; }
        internal List<Fence> Fences { get => fences; set => fences = value; }
        internal List<Route> Routes { get => routes; set => routes = value; }

        internal TrackerController()
        {
            tracker     = new Tracker();
            fences      = new List<Fence>();
            behaviors   = new List<Behavior>();
            routes      = new List<Route>();
        }

        internal Fence createFence()
        {
            Fence fence = new Fence();

            return fence;
        }

        internal Route createRoute()
        {
            Route route = new Route("");

            return route;
        }

        internal void addFence(Fence fence)
        {
            fences.Add(fence);
        }

        internal void removeFenceAt(int index)
        {
            fences.RemoveAt(index);
        }

        internal List<Behavior> getItensNOK()
        {
            List<Behavior> ret=new List<Behavior>();

            foreach(Behavior b in behaviors)
            {
                if (!b.OK())
                {
                    ret.Add(b);
                }
            }

            return ret;
        }

        internal List<Behavior> getItensOK()
        {
            List<Behavior> ret = new List<Behavior>();

            foreach (Behavior b in behaviors)
            {
                if (b.OK())
                {
                    ret.Add(b);
                }
            }

            return ret;
        }

        internal List<Behavior> getBehaviorFiltered(int i)
        {
            List<Behavior> ret = null;

            switch (i)
            {
                case 0: ret = behaviors; break;
                case 1: ret = getItensOK(); break;
                case 2: ret = getItensNOK(); break;
            }

            return ret;
        }

        internal void addRoute(Route r)
        {
            routes.Add(r);
        }

        internal void removeRouteAt(int index)
        {
            routes.RemoveAt(index);
        }

        internal void remove(Route r)
        {
            routes.Remove(r);
        }
    }
}
