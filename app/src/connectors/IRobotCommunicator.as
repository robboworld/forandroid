/**
 * Created by Ilya Peresadin on 07.06.2015.
 */
package connectors {
public interface IRobotCommunicator {
    function turnLeft():void;
    function turnRight():void;
    function goForward():void;
    function goBack():void;
    function motorOn():void;
    function motorOff():void;
    function keepAlive():void;

    function getName():String;
    function setActive(isActive:Boolean):void;
    function finishSession():void;
}
}
