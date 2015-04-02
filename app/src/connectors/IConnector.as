/*
IConnector.as
Ilya Peresadin, November 2014

Interface-wrapper around bluetooth and com-port.
Interface provide common methods for connection with robot.
In the code you should use this interface intstead of classes for bluetooth and com-ports previously
wrap corresponding classes in this interface. 
*/

package connectors {
	public interface IConnector {
		function scanForVisibleDevices(callbackForDiscoveryStarted:Function, callbackForDiscoveryFinished:Function):int;
	}
}