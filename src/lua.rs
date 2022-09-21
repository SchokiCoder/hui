/*
	"House Desktop Environment" in short "house_de"
	Copyright (C) 2022	Andy Frank Schoknecht

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

use crate::logger::Logger;

/*
pub fn test(lua: &mlua::Lua) {
	let globals = lua.globals();
	
	globals.set("test", 10);
	let string: Result<String, mlua::Error> = globals.get("test");
}
*/

pub struct LuaGlobal<'a, T: mlua::UserData + Clone + Copy + 'static> {
	lua: &'a mlua::Lua,
	name: &'static str,
	value: T,
}

impl<'a, T: mlua::UserData + Clone + Copy + 'static> LuaGlobal<'a, T> {
	pub fn new(lua: &'a mlua::Lua, lgr: &mut Logger, name: &'static str, value: T) -> LuaGlobal<'a, T> {	
		let mut result = LuaGlobal {
			lua: lua,
			name: name,
			value: value,
		};
		
		result.set(lgr, result.value);
		
		return result;
	}
	
	pub fn get(&mut self, lgr: &mut Logger) -> T {
		let globals = self.lua.globals();
		
		// if could get, update lua, else log
		let gresult = globals.get(self.name);
		
		if gresult.is_ok() {
			self.value = gresult.unwrap();
		}
		else {
			lgr.log(format!("Lua global \"{}\" could not be get", self.name).as_str());
		}
		
		// return current lua value
		return self.value;
	}
	
	pub fn set(&mut self, lgr: &mut Logger, value: T) {
		let globals = self.lua.globals();
		
		// if could set, update value, else log
		let gresult = globals.set(self.name, self.value);
		
		if gresult.is_ok() {
			self.value = value;
		}
		else {
			lgr.log(format!("Lua global \"{}\" could not be set", self.name).as_str());
		}
	}
}