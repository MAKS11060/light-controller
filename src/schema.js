export const schema = {
	[(1 << 0)]: {name: 'start',
		[(1 << 0)]: {name: 'end'},
	},
	[(1 << 1)]: {name: 'msg',
		[(1 << 0)]: {name: 'end'},
		[(1 << 1)]: {name: 'power'},
		[(1 << 2)]: {name: 'play'},
		[(1 << 3)]: {name: 'brightness',
			[(1 << 0)]: {name: 'end'},
			[(1 << 1)]: {name: 'set'},
			[(1 << 2)]: {name: 'get'},
		},
		[(1 << 4)]: {name: 'effect',
			[(1 << 0)]: {name: 'end'},
			[(1 << 1)]: {name: 'set'},
			[(1 << 2)]: {name: 'get'},
			[(1 << 3)]: {name: 'next'},
			[(1 << 4)]: {name: 'back'},
		},
	},
	
	[(1 << 6)]: {name: 'info'},
	[(1 << 7)]: {name: 'ping',
		[(1 << 0)]: {name: 'end'},
	}
}


export const parseSchema = (schema, prevKey) => {
	const {name=null, ...keys} = schema
	const result = {}
	if (name) {
		for (const key of Object.keys(keys)) {
			result[keys[key].name || key] = {_byte: +key,
				...parseSchema(keys[key], key)
			}
		}
	} else {
		for (const key of Object.keys(keys)) {
			result[keys[key].name] = {_byte: +key,
				...parseSchema(keys[key], key)
			}
		}
	}
	return result
}

//export const parseSchema = (schema, prevKey) => {
//	const {name=null, ...keys} = schema
//	const result = {}
//	if (name) {
//		for (const key of Object.keys(keys)) {
//			result[keys[key].name || key] = {_byte: [...prevKey, key],
//				...parseSchema(keys[key], [...prevKey, key])
//			}
//		}
//	} else {
//		for (const key of Object.keys(keys)) {
//			result[keys[key].name] = {_byte: [key],
//				...parseSchema(keys[key], [key])
//			}
//		}
//	}
//	return result
//}
