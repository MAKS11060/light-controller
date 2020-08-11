import './arduino.js'

setInterval(()=>{}, 1000)














//class Protocol {
//	start   = Buffer.from([0xf0, 0x0f])
//	payload = Buffer.alloc(2)
//}
//
//class Pocket extends Protocol {
//	write(buf) {
//		if (!Buffer.isBuffer(buf)) throw new TypeError('is not a Buffer')
//		else {
//			this.payload.writeUInt16BE(buf.length)
//			this.data = Buffer.concat([
//				this.start,
//				this.payload,
//				buf,
//			])
//		}
//	}
//}
//
//const p = new Pocket()
//
//p.write(Buffer.alloc(5))
//
//console.log(p)
