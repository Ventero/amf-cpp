package{
	import flash.display.Sprite;
	import flash.net.registerClassAlias;
	import flash.text.TextField;
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;

	[SWF(width="450", height="400")]
	public class flashtest extends Sprite {
		private var tf:TextField;
		public function flashtest() {
			tf = new TextField();
			tf.width = tf.height = 450;
			tf.wordWrap = true;
			addChild(tf);

			dumpValues();
			readValues();
		}

		private function dumpValues():void {
			tf.text = "";
			var b:ByteArray = new ByteArray();
			var a:Dictionary = new Dictionary();
			var o:Object = {"bar": 1};
			a[o] = "foo";
			a["qux"] = o;
			b.writeObject(a);
			dumpByteArray(b);
		}

		private function readValues():void {
			/*
			var data:Array = [
				0x11, 0x07, 0x00,
					0x0a, 0x0b, 0x01,
						0x07, 0x62, 0x61, 0x72,
						0x04, 0x01,
						0x01,
					0x06, 0x07, 0x66, 0x6f, 0x6f,
					0x0a, 0x0b, 0x01,
						0x07, 0x62, 0x61, 0x72,
						0x04, 0x02,
						0x01,
					0x06, 0x07, 0x66, 0x6f, 0x6f,
					0x06, 0x07, 0x71, 0x75, 0x78,
					0x0a, 0x00
			];
			*/
			var data:Array = [
				0x0a, 0x00
			];
			var b:ByteArray = createByteArray(data);

			var o:* = b.readObject();
			tf.appendText(o + "\n");
			tf.appendText(o.f + "\n");
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
			tf.appendText("\tv8 data {\n\t\t");
			var cnt:int = 0;
			while(b.bytesAvailable) {
				var c:String = b.readUnsignedByte().toString(16);
				if(c.length < 2) c = "0" + c;
				tf.appendText("0x" + c);
				if (b.bytesAvailable == 0) break;
				if (++cnt == 12) {
					cnt = 0;
					tf.appendText(",\n\t\t");
				} else {
					tf.appendText(", ");
				}
			}
			tf.appendText("\n\t};\n");
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
