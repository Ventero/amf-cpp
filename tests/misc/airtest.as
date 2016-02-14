package
{
    import flash.display.Sprite;
    import flash.events.InvokeEvent;
    import flash.filesystem.File;
    import flash.filesystem.FileMode;
    import flash.filesystem.FileStream;
    import flash.desktop.NativeApplication;

    public class airtest extends Sprite
    {
        public function airtest()
        {
            NativeApplication.nativeApplication.addEventListener(InvokeEvent.INVOKE, onInvoke);
        }

        public function onInvoke(invokeEvent:InvokeEvent):void
        {
        	var path:String = invokeEvent.arguments[0];
        	var file:File = invokeEvent.currentDirectory.resolvePath(path);
        	trace(file.nativePath);

        	var stream:FileStream = new FileStream();
        	trace(stream);
        	stream.open(file, FileMode.READ);

        	try {
        		trace(stream.readObject());
        	} catch (e:*) { };
        }
    }
}
