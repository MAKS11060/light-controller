import Events from "events"

export const create_led_strip = (count, colorShema = 'RGB') => {
	const events = new Events()
	const buf = Buffer.alloc(
		count * 4,
		0x00
	)
	
	const read = pos => {
		return [
			buf.readUInt8(pos * 4),
			buf.readUInt8(pos * 4 + 1),
			buf.readUInt8(pos * 4 + 2),
			buf.readUInt8(pos * 4 + 3),
		]
	}
	const write = (pos, r, g, b) => {
		buf.writeUInt8(pos,pos * 4)
		buf.writeUInt8(r,pos * 4 + 1)
		buf.writeUInt8(g,pos * 4 + 2)
		buf.writeUInt8(b,pos * 4 + 3)
		events.emit('write', pos, r, g, b)
	}
	
	const set = (pos, r, g, b) => {
		write(pos, r, g, b)
		events.emit('set', pos, r, g, b)
		events.emit('update')
	}
	const fill = (r, g, b) => {
		for (let i = 0; i < count; i++) {
			write(i, r, g, b)
		}
		events.emit('fill')
		events.emit('update')
	}
	const clear = () => {
		for (let i = 0; i < count; i++) {
			set(i, 0, 0, 0)
		}
		events.emit('update')
	}
	
	return {events, buf, read, set, fill, clear}
}

export const create_led_stripV2 = (count, colorShema = 'RGB') => {
	const events = new Events()
	const buf = Buffer.alloc(
		count * 3,
		0x00
	)
	
	const read = pos => {
		return [
			pos,
			buf.readUInt8(pos * 3),     // r
			buf.readUInt8(pos * 3 + 1), // g
			buf.readUInt8(pos * 3 + 2)  // b
		]
	}
	const write = (pos, r, g, b) => {
		buf.writeUInt8(r,pos * 3)     // r
		buf.writeUInt8(g,pos * 3 + 1) // g
		buf.writeUInt8(b,pos * 3 + 2) // b
		events.emit('write', pos, r, g, b)
	}
	
	const set = (pos, r, g, b) => {
		write(pos, r, g, b)
		events.emit('set', pos, r, g, b)
		events.emit('update')
	}
	const fill = (r, g, b) => {
		for (let i = 0; i < count; i++) {
			write(i, r, g, b)
		}
		events.emit('fill', r, g, b)
		events.emit('update')
	}
	const clear = () => {
		for (let i = 0; i < count; i++) {
			set(i, 0, 0, 0)
		}
		events.emit('update')
	}
	
	return {events, buf, read, set, fill, clear}
}
