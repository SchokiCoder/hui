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

use json::JsonValue;

pub struct ShellCmd {
	pub exe: String,
	pub args: Vec<String>,
}

impl ShellCmd {
	pub fn new() -> ShellCmd {
		return ShellCmd {
			exe: String::new(),
			args: Vec::<String>::new(),
		};
	}
	
	pub fn from(exe: &str, args: &str) -> ShellCmd {
		let mut result = ShellCmd {
			exe: exe.to_string(),
			args: Vec::<String>::new(),
		};
		
		for arg in args.split(' ') {
			result.args.push(arg.to_string());
		}
		
		return result;
	}
	
	pub fn from_json(mut json: JsonValue) -> ShellCmd {
		let mut result = ShellCmd {
			exe: json["exe"].to_string(),
			args: Vec::<String>::new(),
		};
		
		loop {
			let arg = json["args"].array_remove(0);
			if arg == JsonValue::Null {
				break;
			}
			
			result.args.push(arg.to_string());
		}
		
		return result;
	}
}

pub struct Button {
	pub label: String,
	pub shell: ShellCmd,
	pub lua: String,
	pub buttons: Vec<Button>,
}

impl Button {
	pub fn from(label: &str, shell: ShellCmd, lua: &str, buttons: Vec<Button>) -> Button {
		return Button {
			label: label.to_string(),
			shell: shell,
			lua: lua.to_string(),
			buttons: buttons,
		};
	}

	pub fn from_json(mut json: JsonValue) -> Result<Button, &'static str> {
		let mut result = Button {
			label: json["label"].to_string(),
			buttons: Vec::<Button>::new(),
			shell: ShellCmd::from_json(json["shell"].take()),
			lua: json["lua"].to_string(),
		};
		
		loop {
			let button = json["buttons"].array_remove(0);
			if button == JsonValue::Null {
				break;
			}
			
			let btn_result = Button::from_json(button);
			
			if !btn_result.is_ok() {
				return Err("Button in submenu corrupt");
			}
			
			result.buttons.push(btn_result.unwrap());
		}
		
		return Ok(result);
	}
}