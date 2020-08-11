const PROTOCOL_key   = Buffer.from([0x92, 0x4c, 0x32, 0x1c])
const PTOTOCOL_start = Buffer.from([0xf0, 0x0f])

export const checkKey = buf => {
	return Buffer.compare(PROTOCOL_key, buf) === 0
}
export const createPocket = buf => {
	const contentLength = Buffer.alloc(2)
	
	contentLength.writeUInt16BE(buf.length)
	return Buffer.concat([
		PTOTOCOL_start,
		contentLength,
		buf,
	])
}
