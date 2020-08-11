import Events from 'events'
import SerialPort from 'serialport'
import {createPocket, checkKey} from './protocol.js'
import * as action from './actions.js'
//import {strip} from './actions.js'

export const createArduino = (port = 'com3', baudRate = 115200) => {
	const com = new SerialPort(port, {baudRate})
	const event = new Events
	const state = {
		connect: false,
		checkKey: false,
		start: false
	}

	com.on('error', e => console.error(e))
	com.on('close', e => {
		if (e.disconnected) {
			state.connect = false
			console.log('disconnected')
		} else {
			state.connect = false
			console.log('close', e)
		}
	})
	com.on('open', () => {
		state.connect = true
		console.log('Connected')
	})

	let buf = []
	com.on('data', data => {
		//console.log(data)
		for (const c of data) {
			if (c) buf.push(c)
			else {
				parse(Buffer.from(buf))
				buf = []
			}
		}
	})

	const parse = buf => {
		if (state.checkKey) {
			console.log(buf)
			console.log(`${com.path}> ${buf}\n`)

		} else {
			if (checkKey(buf)) state.checkKey = true
			console.log(`checkKey:`, state.checkKey)
		}
	}

	return {com, state}
}
const createArduinoFake = () => {
	return {
		com: {
			write(buf) {
				console.log(buf)
			}
		}
	}
}



//const avr = createArduino()
////const avr = createArduinoFake()
//setTimeout(() => {
//	avr.com.write(createPocket(Buffer.concat([
//
//		action.play(),
//		action.setLeds()
//		//action.setBrightness(50)
//
//	])))
//}, 3000)



























/*const serial = create_arduino()
setTimeout(() => {
	serial.write(createPayload(
		Buffer.from('abc')
	))
}, 3000)*/



//setTimeout(() => {
//	serial.write(Buffer.from([
//		c.start._byte,
//		c.info._byte,
//	]))
//}, 3000)
//
//setTimeout(() => {
//	serial.write(Buffer.from([
//		c.ping._byte,
//		87,23,78,67,36,43,63,86,55,56,36,
//		c.ping.end._byte
//	]))
//
//	setTimeout(() => {
//		serial.write(Buffer.from([
//		]))
//	}, 60000)
//}, 6000)

//console.log((0xf00f >> 8).toString(16))
//console.log((0xf00f & 0xff).toString(16))
//
//const hex2buf = (hex = 0x0f0f0f) => Buffer.from([
//	hex >> 16 /*& 0xff*/, hex >> 8 & 0xff, hex & 0xff
//])

/*const b1 = 0x00
const b2 = 0x10

//console.log(0xff00)
let data = b1 << 8
data |= b2

console.log(data.toString(16))*/


