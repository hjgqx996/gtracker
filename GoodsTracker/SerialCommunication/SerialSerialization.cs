﻿using System;
using System.Diagnostics;

namespace GoodsTracker
{
    class SerialSerialization : FrameSerialization
    {
        public bool encode(out PayLoad payload, Telemetria b)
        {
            payload     = new PayLoad();

            payload.Append(b.Latitude);
            payload.Append(CONST_CHAR.SEPARATOR);
            payload.Append(b.Longitude);
            payload.Append(CONST_CHAR.SEPARATOR);
            payload.Append(b.AxisX.Acceleration.Val);
            payload.Append(CONST_CHAR.SEPARATOR);
            payload.Append(b.AxisY.Acceleration.Val);
            payload.Append(CONST_CHAR.SEPARATOR);
            payload.Append(b.AxisZ.Acceleration.Val);
            payload.Append(CONST_CHAR.SEPARATOR);
            payload.Append(b.AxisX.Rotation.Val);
            payload.Append(CONST_CHAR.SEPARATOR);
            payload.Append(b.AxisY.Rotation.Val);
            payload.Append(CONST_CHAR.SEPARATOR);
            payload.Append(b.AxisZ.Rotation.Val);
            payload.Append(CONST_CHAR.SEPARATOR);
            payload.Append(b.Speed.Val);
            payload.Append(CONST_CHAR.SEPARATOR);
            payload.Append(b.Level.Val);
            payload.Append(CONST_CHAR.SEPARATOR);
            payload.Append(b.StatusLock);
            payload.Append(CONST_CHAR.SEPARATOR);
            payload.Append(Convert.ToString(b.getTimeStamp()));

            return true;
        }

        /**
         * 
         * Codifica o cabeçalho
         */
        public bool encode(HeaderFrame header)
        {
            header.Append(header.Address.ToString("D5") + CONST_CHAR.SEPARATOR);
            header.Append(header.Dest.ToString("D5") + CONST_CHAR.SEPARATOR);
            header.Append(header.Count.ToString("D5") + CONST_CHAR.SEPARATOR);
            header.Append(header.Operation.ToString() + CONST_CHAR.SEPARATOR);
            header.Append(header.Resource);

            return true;
        }

        /*
         * 
         * Decodifica uma resposta completa cabeçalho+payload
         * 
         */
        public bool decode(out AnsCmd ans, CommunicationFrame frame)
        {
            bool ret    = false;
            ans         = new AnsCmd();

            string[] list = frame.Data.Split(CONST_CHAR.SEPARATOR);

            if (list != null && list.Length >= 8)
            {
                // Exclui CheckSum
                frame.Data = frame.Data.Substring(0, frame.Data.Length - 2);
                byte cheksumRx = (byte)Convert.ToInt16(list[list.Length - 1], 16);

                if (frame.checkSum() == cheksumRx)
                {
                    ans.Header = decoderHeader(list);

                    if (ans.Header.Resource.Equals(RESOURCE.TLM) &&
                        ans.Header.Operation.Equals(Operation.AN) && list.Length >= 9)
                    {
                        ans.Telemetria = decoderTLM(list);

                        ret = true;
                    }
                    else
                    {
                        ret = false;
                        throw new Exception("Erro no payload");
                    }
                }
                else
                {
                    ret = false;
                    throw new Exception("Erro de CheckSum");
                }
            }
            else
            {
                ret = false;
                throw new ArgumentException("Error ao fazer o split do payload: " + frame.Data, "frame");
            }

            return ret;
        }

        /*
         * Decodifica um payload do tipo TLM fazendo a conversão de uma lista de string para seu respectivo formato original
         */
        private Telemetria decoderTLM(string[] list)
        {
            Telemetria telemetria = new Telemetria();

            FieldFrame<double> lat = new FieldFrame<double>(INDEX.LAT, list, UNIT_FIELD.GRAU_SEXAGESIMAL);
            FieldFrame<double> lng = new FieldFrame<double>(INDEX.LNG, list, UNIT_FIELD.GRAU_SEXAGESIMAL);
            FieldFrame<int> a_x = new FieldFrame<int>(INDEX.ACCEL_X, list);
            FieldFrame<int> a_y = new FieldFrame<int>(INDEX.ACCEL_Y, list);
            FieldFrame<int> a_z = new FieldFrame<int>(INDEX.ACCEL_Z, list);
            FieldFrame<double> g_x = new FieldFrame<double>(INDEX.ACCEL_XG, list);
            FieldFrame<double> g_y = new FieldFrame<double>(INDEX.ACCEL_YG, list);
            FieldFrame<double> g_z = new FieldFrame<double>(INDEX.ACCEL_ZG, list);
            FieldFrame<int> speed = new FieldFrame<int>(INDEX.SPEED, list);
            FieldFrame<int> level = new FieldFrame<int>(INDEX.LEVEL, list);
            FieldFrame<bool> bLCk = new FieldFrame<bool>(INDEX.TRAVA, list);
            FieldFrame<DateTime> dt = new FieldFrame<DateTime>(INDEX.TIME_STAMP, list);

            telemetria.setPosition(lat.getVal(), lng.getVal());
            telemetria.setAcceleration(a_x.getVal(), a_y.getVal(), a_z.getVal());
            telemetria.setRotation(g_x.getVal(), g_y.getVal(), g_z.getVal());

            telemetria.Speed.Val = speed.getVal();
            telemetria.Level.Val = level.getVal();
            telemetria.StatusLock = bLCk.getVal();
            telemetria.DateTime = dt.getVal();

            return telemetria;
        }
    
        /*
         * Decodifica um header fazendo a conversão de uma lista de string para seu respectivo formato original
         */
        private HeaderFrame decoderHeader(string[] list)
        {
            HeaderFrame header = new HeaderFrame();

            FieldFrame<int> addr = new FieldFrame<int>(INDEX.ADDRESS, list);
            FieldFrame<int> dest = new FieldFrame<int>(INDEX.DEST, list);
            FieldFrame<int> count = new FieldFrame<int>(INDEX.COUNT, list);
            FieldFrame<Operation> op = new FieldFrame<Operation>(INDEX.OPERACAO, list);
            FieldFrame<string> res = new FieldFrame<string>(INDEX.RESOURCE, list);
            FieldFrame<int> s_pl = new FieldFrame<int>(INDEX.SIZE_PAYLOAD, list);

            header.Address = addr.getVal();
            header.Dest = dest.getVal();
            header.Count = count.getVal();
            header.Operation = op.getVal();
            header.Resource = res.getVal();
            header.SizePayLoad = s_pl.getVal();

            return header;
        }
    }
}