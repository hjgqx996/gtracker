﻿using System.Collections.Generic;

namespace GoodsTracker
{
    class TrackerController :ThreadRun
    {
        onUpdate onDataTelemetria, onStatusLock;

        private static TrackerController singleton=null;

        Tracker             tracker;
        List<Fence>         fences;
        List<Route>         routes;

        internal List<Fence> Fences { get => fences; set => fences = value; }
        internal List<Route> Routes { get => routes; set => routes = value; }

        //Singleton
        public static TrackerController TrackerCtrl
        {
            get
            {
                if (singleton == null)
                {
                    singleton = new TrackerController();
                }

                return singleton;
            }
        }

        internal Tracker Tracker { get => tracker; set => tracker = value; }
        public onUpdate OnDataTelemetria { get => onDataTelemetria; set => onDataTelemetria = value; }
        public onUpdate OnStatusLock { get => onStatusLock; set => onStatusLock = value; }

        private TrackerController()
        {
            tracker     = new Tracker(1);
            fences      = new List<Fence>();
            routes      = new List<Route>();
        }

        /*
         * 
         * Metodo chamdo na Thread do Domain
         * 
         */
        public override void run()
        {
            requestBehavior();
        }

        internal Fence createFence()
        {
            Fence fence = new Fence();

            return fence;
        }

        internal Route createRoute(string name)
        {
            Route route = new Route(name);

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

        internal List<TelemetriaData> getBehaviorFiltered(int i)
        {
            List<TelemetriaData> ret = null;

            if (anyRoute())
            {
                ret = routes[0].getBehaviorFiltered(i);
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

        internal void requestBehavior()
        {
            tracker.requestBehavior(onReceiveBehavior);
        }

        internal void unLockVehicle()
        {
            tracker.unLockVehicle(onLockStatus);
        }

        internal void lockVehicle()
        {
            tracker.lockVehicle(onLockStatus);
        }

        internal void registerBehavior(TelemetriaData b)
        {
            if (anyRoute() && b != null)
            {
                routes[0].registerBehavior(b);
            }
        }

        internal bool anyRoute()
        {
            return routes.Count > 0 && routes[0].MapRoute != null;
        }

        internal int getCountRegisters()
        {
            return anyRoute() ? routes[0].Behaviors.Count : 0;
        }

        // CallBacks

        private ResultExec onLockStatus(AnsCmd ans)
        {
            registerBehavior(tracker.getTelemetria());

            onStatusLock?.Invoke();

            return ResultExec.EXEC_SUCCESS;
        }

        private ResultExec onReceiveBehavior(AnsCmd ans)
        {
            registerBehavior(tracker.getTelemetria());

            onDataTelemetria?.Invoke();

            return ResultExec.EXEC_SUCCESS;
        }
    }
}
