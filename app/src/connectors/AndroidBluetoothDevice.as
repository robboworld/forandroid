package connectors
{
	import com.as3breeze.air.ane.android.BluetoothDevice;
	import com.as3breeze.air.ane.android.events.BluetoothDataEvent;
	import com.as3breeze.air.ane.android.events.BluetoothDeviceEvent;
	
	import flash.events.Event;
	import flash.utils.ByteArray;

public class AndroidBluetoothDevice implements IDevice
{
	private var device:BluetoothDevice;
	private var conList:Function;
	private var disconList:Function;
	private var errList:Function;
	private var recvList:Function;
	
	public function AndroidBluetoothDevice(dev:BluetoothDevice)
	{
		device = dev;
	}
	
	public function address():String {
		if (device == null)
			return null;
		return device.address;
	}
	
	public function addDeviceConnectedListener(callbackOnDeviceConnected:Function):void {
		conList = callbackOnDeviceConnected;
		device.addEventListener(BluetoothDeviceEvent.BLUETOOTH_DEVICE_CONNECTED, callbackOnDeviceConnected); 
	}
	
	public function addDeviceDisconnectedListener(callbackOnDeviceDisconnected:Function):void {
		disconList = callbackOnDeviceDisconnected;
		device.addEventListener(BluetoothDeviceEvent.BLUETOOTH_DEVICE_DISCONNECTED, callbackOnDeviceDisconnected); 
	}
	
	public function addDeviceConnectErrorListener(callbackOnDeviceConnectError:Function):void {
		errList = callbackOnDeviceConnectError;
		device.addEventListener(BluetoothDeviceEvent.BLUETOOTH_DEVICE_CONNECT_ERROR, callbackOnDeviceConnectError);
	}
	
	public function addReceiveDataListener(callbackOnReceiveData:Function):void {
		recvList = callbackOnReceiveData;
		device.addEventListener(BluetoothDataEvent.BLUETOOTH_RECEIVE_DATA, callbackOnReceiveData);
	}
	
	public function connect():void {
		device.UUID = "00001101-0000-1000-8000-00805F9B34FB"; //"029F9140-EBC6-11E2-91E2-0800200C9A66";
		device.connect();
	}
	
	public function removeAllListners():void {
		if (conList != null) device.removeEventListener(BluetoothDeviceEvent.BLUETOOTH_DEVICE_CONNECTED, conList);
		if (disconList != null) device.removeEventListener(BluetoothDeviceEvent.BLUETOOTH_DEVICE_DISCONNECTED, disconList);
		if (errList != null) device.removeEventListener(BluetoothDeviceEvent.BLUETOOTH_DEVICE_CONNECT_ERROR, errList);
		if (recvList != null) device.removeEventListener(BluetoothDataEvent.BLUETOOTH_RECEIVE_DATA, recvList);
	}
	
	public function disconnect():void {
		device.disconnect();
	}
	
	public function sendData(a:ByteArray):Boolean {
		return device.sendData(a);
	}
	
	public function sendByte(a:int):Boolean
	{
		var ba:ByteArray = new ByteArray();
		ba.writeByte(a);
		ba.position = 0;
		return device.sendData(ba);
	}
	
	public function isConnected():Boolean
	{
		return device.isConnected;
	}
	
	public function getName():String {
		return device.name;
	}
	//var stringToSend:String = "Hello awesome bluetoothdevice!";
	public function deviceEventHandler( b:BluetoothDeviceEvent ):void 
	{
		trace("In device handler");	
		switch( b.type ) { 
			case BluetoothDeviceEvent.BLUETOOTH_DEVICE_CONNECTED:
				
			break; 
			case BluetoothDeviceEvent.BLUETOOTH_DEVICE_DISCONNECTED: 
				
			break; 
			case BluetoothDeviceEvent.BLUETOOTH_DEVICE_CONNECT_ERROR:
				
			break;
		}
	}
}
}