import {create_led_strip, create_led_stripV2} from './lib/strip.js'
export const strip = create_led_stripV2(60)

export const play = () => Buffer.from([2])
export const setBrightness = val => Buffer.from([3, val])

export const setLeds = () => {
	return Buffer.concat([
		Buffer.from([4, strip.buf.length / 3]),
		strip.buf
	])
}

//Buffer.from([
//	4, // id
//	60, // leds count
//	0,0,0 // rgb
//])