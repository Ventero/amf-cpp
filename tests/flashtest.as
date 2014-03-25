package{
	import flash.display.Sprite;
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

			var b:ByteArray = new ByteArray();
			var v:Vector.<int> = new Vector.<int>();
			for(var z:int = 0; z < 3; ++z)
				v.push(z);
			b.writeObject(v);

			dumpByteArray(b);

			var data:Array = [
				0x0d, 0x07, 0x00,
				0x00, 0x00, 0x00, 0x01,
				0x00, 0x00, 0x00, 0x02,
				0x00, 0x00, 0x00, 0x03
			];
			b = createByteArray(data);

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
