package{
	import flash.display.Sprite;
	import flash.net.registerClassAlias;
	import flash.text.TextField;
	import flash.utils.ByteArray;

	[SWF(width="400", height="400")]
	public class flashtest extends Sprite {
		private var tf:TextField;
		public function flashtest() {
			tf = new TextField();
			tf.width = tf.height = 400;
			tf.wordWrap = true;
			addChild(tf);

			dumpValues();
		}

		private function dumpValues():void {
			tf.text = "";
			var b:ByteArray = new ByteArray();
			var a:Array = [undefined, null, [], "foo"];
			a.foo = "qux";
			a.bar = [];
			b.writeObject(a);
			dumpByteArray(b);
		}

		private function readValues():void {
			var data:Array = [
				0x0d, 0x07, 0x00,
				0x00, 0x00, 0x00, 0x01,
				0x00, 0x00, 0x00, 0x02,
				0x00, 0x00, 0x00, 0x03
			];
			var b:ByteArray = createByteArray(data);

			var o:* = b.readObject();
			tf.appendText(o + "\n");
			var a:Vector.<int> =  o as Vector.<int>;
			if(a != null) {
				tf.appendText("Vector with " + a.length + " elements\n");
				for(var j:int = 0; j < a.length; ++j)
					tf.appendText(j + ": " + a[j] + "\n");
			} else {
				tf.appendText("Vector is null\n");
			}
		}

		private function testExternalizable():void {
			tf.text = "";
			registerClassAlias("Foo", Foo);
			var b:ByteArray = new ByteArray();
			var f:Foo = new Foo("asd");
			var g:Foo = new Foo("bar");
			var a:Array = [f, f, g];
			b.writeObject(a);
			dumpByteArray(b);
			b.position = 0;
			var a2:Array = b.readObject();
			tf.appendText((a[0] as Foo).foo);
			tf.appendText((a[1] as Foo).foo);
			tf.appendText((a[2] as Foo).foo);
		}

		private function dumpByteArray(b:ByteArray):void {
			b.position = 0;
			tf.appendText(b.bytesAvailable + "\n");
			var cnt:int = 0;
			while(b.bytesAvailable) {
				var c:String = b.readUnsignedByte().toString(16);
				if(c.length < 2) c = "0" + c;
				tf.appendText("0x" + c);
				if (b.bytesAvailable == 0) break;
				if (++cnt == 12) {
					cnt = 0;
					tf.appendText(",\n");
				} else {
					tf.appendText(", ");
				}
			}
			tf.appendText("\n");
		}

		private function createByteArray(data:Array):ByteArray {
			var b:ByteArray = new ByteArray();

			for(var i:int = 0; i < data.length; ++i)
				b.writeByte(data[i]);

			b.position = 0;
			return b;
		}
	}
}

import flash.utils.IDataInput;
import flash.utils.IDataOutput;
import flash.utils.IExternalizable;

class Foo implements IExternalizable {
	public var foo:String;

	public function Foo(s:String = "default") {
		foo = s;
	}

	public function writeExternal(output:IDataOutput):void {
		output.writeUTF(foo);
	}

	public function readExternal(input:IDataInput):void {
		foo = input.readUTF();
	}
}
