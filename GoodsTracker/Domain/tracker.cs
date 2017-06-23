﻿using System;

namespace GoodsTracker
{
    class Tracker : CommunicationUnit,InterfaceTracker
    {
        TelemetriaData    telemetriaData;

        internal TelemetriaData TelemetriaData { get => telemetriaData; set => telemetriaData = value; }

        internal Tracker(int val)
        {
            address = val;
        }

        public double getLevel()
        {
            return telemetriaData.Level.Val;
        }

        public void requestBehavior(onAnswerCmd on_ans)
        {
            sendCMD(2,Operation.RD,RESOURCE.TELEMETRIA).EventAnswerCmd = on_ans;
        }

        public void lockVehicle(onAnswerCmd on_ans)
        {
            sendCMD(2, Operation.WR, RESOURCE.LOCK).EventAnswerCmd = on_ans;
        }

        public void unLockVehicle(onAnswerCmd on_ans)
        {
            sendCMD(2, Operation.WR, RESOURCE.LOCK).EventAnswerCmd = on_ans;
        }

        /*
         * 
         * Hook para processamento de comandos respondidos
         * 
         */
        protected override void onReceiveAnswer(AnsCmd ans)
        {
            if(ans.Resource.Equals(RESOURCE.TELEMETRIA))
            {
                updateDataTelemetria(ans);
            }
            else if(ans.Resource.Equals(RESOURCE.LOCK)){
                // Nao fazer nada pq o status da trava esta na telemetria
            }
        }

        void updateDataTelemetria(AnsCmd ans)
        {
            telemetriaData = ans.Info;
        }

        public TelemetriaData getTelemetria()
        {
            return telemetriaData;
        }
    }
}
