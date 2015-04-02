/*
IDevice.as
Ilya Peresadin, November 2014

Interface-wrapper around bluetooth-device and com-port.
*/

package connectors {
	import flash.utils.ByteArray;

	public interface IDevice {
		//Dangerous, can added more then one handlers
		function addDeviceConnectedListener(callbackOnDeviceConnected:Function):void;
		function addDeviceDisconnectedListener(callbackOnDeviceDisconnected:Function):void;
		function addDeviceConnectErrorListener(callbackOnDeviceConnectError:Function):void;
		function addReceiveDataListener(callbackOnReceiveData:Function):void;
		function removeAllListners():void;
		
		function connect():void;
		function disconnect():void;
		function sendData(a:ByteArray):Boolean;
		function sendByte(a:int):Boolean;
		function isConnected():Boolean;
		function getName():String;
		function address():String;
	}
}