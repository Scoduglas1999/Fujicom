﻿using ASCOM;
using ASCOM.DeviceInterface;

namespace ASCOM.ScdouglasFujifilm.Camera
{
    internal interface ICameraV3
    {
        // Dummy interface just to stop compile errors during development.
        // This file is not needed and is deleted by the setup wizard.
    }
}

//Dummy implementation to stop compile errors in the Driver template solution
internal class AxisRates : IAxisRates
{
    public AxisRates(TelescopeAxes Axis)
    {
    }

    public int Count
    {
        get { throw new PropertyNotImplementedException(); }
    }

    public void Dispose()
    {
        throw new MethodNotImplementedException();
    }

    public System.Collections.IEnumerator GetEnumerator()
    {
        throw new MethodNotImplementedException();
    }

    public IRate this[int index]
    {
        get { throw new PropertyNotImplementedException(); }
    }
}

//Dummy implementation to stop compile errors in the Driver template solution
internal class TrackingRates : ITrackingRates
{
    public int Count
    {
        get { throw new PropertyNotImplementedException(); }
    }

    public void Dispose()
    {
        throw new MethodNotImplementedException();
    }

    public System.Collections.IEnumerator GetEnumerator()
    {
        throw new MethodNotImplementedException();
    }

    public DriveRates this[int index]
    {
        get { throw new PropertyNotImplementedException(); }
    }
}
