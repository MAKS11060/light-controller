import Events from 'events'

const RGBToHSL = (r, g, b) => {
	// Make r, g, and b fractions of 1
	r /= 255
	g /= 255
	b /= 255
	
	// Find greatest and smallest channel values
	let cmin = Math.min(r,g,b)
	let cmax = Math.max(r,g,b)
	let delta = cmax - cmin
	let h = 0
	let s = 0
	let l = 0
	
	// Calculate hue
	// No difference
	if (delta == 0) h = 0
	// Red is max
	else if (cmax == r)	h = ((g - b) / delta) % 6
	// Green is max
	else if (cmax == g) h = (b - r) / delta + 2
	// Blue is max
	else h = (r - g) / delta + 4
	
	h = Math.round(h * 60)
	
	// Make negative hues positive behind 360°
	if (h < 0) h += 360
	
	// Calculate lightness
	l = (cmax + cmin) / 2
	
	// Calculate saturation
	s = delta == 0 ? 0 : delta / (1 - Math.abs(2 * l - 1))
	
	// Multiply l and s by 100
	//s = +(s * 100).toFixed(1)
	//l = +(l * 100).toFixed(1)
	
	return [h,
		+(s * 100).toFixed(1),
		+(l * 100).toFixed(1)
	]
}
const HSLToRGB = (h, s, l) => {
	// Must be fractions of 1
	s /= 100
	l /= 100
	
	let c = (1 - Math.abs(2 * l - 1)) * s
	let x = c * (1 - Math.abs((h / 60) % 2 - 1))
	let m = l - c / 2
	let r = 0
	let g = 0
	let b = 0
	
	if (0 <= h && h < 60) {
		r = c; g = x; b = 0
	} else if (60 <= h && h < 120) {
		r = x; g = c ;b = 0
	} else if (120 <= h && h < 180) {
		r = 0; g = c; b = x
	} else if (180 <= h && h < 240) {
		r = 0; g = x; b = c
	} else if (240 <= h && h < 300) {
		r = x; g = 0; b = c
	} else if (300 <= h && h < 360) {
		r = c; g = 0; b = x
	}
	r = Math.round((r + m) * 255)
	g = Math.round((g + m) * 255)
	b = Math.round((b + m) * 255)
	
	return [r, g, b]
}

export class Color extends Events {
	constructor() {super()
		this._rgb = [0, 0, 0]
		this._hsl = [0, 0, 0]
	}
	
	RGB(r, g, b) {
		this._rgb = [r, g, b]
		this._hsl = RGBToHSL(r, g, b)
	}
	HSL(h, s, l) {
		this._hsl = [h, s, l]
		this._rgb = HSLToRGB(h, s, l)
	}
}
