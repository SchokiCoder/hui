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

use std::collections::HashMap;
use std::vec::Vec;
use std::string::String;
use std::fs::File;
use std::io::Read;
use serde_derive::Deserialize;

struct Command {
	label: String,
	command: String,
}

struct Menu {
	label: String,
	buttons: Vec<Button>,
}

enum Button {
	Command(Command),
	Menu(Menu),
}

struct UserConfig {
	main_menu: Menu,
}

impl UserConfig {
	pub fn new(username: &str) -> Result<UserConfig, String> {
		#[derive(Deserialize)]
		struct ButtonRaw {
			label: String,
			menu: Vec<ButtonRaw>,
			command: String,
		}
		
		#[derive(Deserialize)]
		struct MenuRaw {
			label: String,
			buttons: Vec<ButtonRaw>,
		}
		
		#[derive(Deserialize)]
		struct UserConfigRaw {
			motd: String,
			main_menu: MenuRaw,
		}
		
		let mut result = UserConfig {
			main_menu: Menu {
				label: String::from("Main"),
				buttons: Vec::<Button>::new(),
			},
		};
	
		// open etc file
		/*let mut etcpath = String::from("/etc/");
		etcpath.push_str(env!("CARGO_PKG_NAME"));
		etcpath.push_str(".d/");
		etcpath.push_str(username);*/
		let etcpath = String::from(username); // temp override
		
		let f = File::open(etcpath.as_str());
		
		if !f.is_ok() {
			return Err(format!("User config \"{}\" could not be opened", etcpath))
		}
		
		let mut f = f.unwrap();
		
		// read file
		let mut text = String::new();
		if !f.read_to_string(&mut text).is_ok() {
			return Err(format!("User config \"{}\" could not be read", etcpath))
		}
		
		// parse toml
		let rawcfg: UserConfigRaw = toml::from_str(text.as_str()).unwrap();
		
		// convert to usable config
		
/*
		// read lines
		let mut i: usize = 0;
		let lines: Vec<&str> = text.split('\n').collect();
		
		while i < lines.len() {
			// read words
			let words: Vec<&str> = lines[i].trim().split(' ').collect();
			
			match words[0] {
				"buttons" => {
					i += 1;
					
					while i < lines.len() {
						// count indentation
						let mut indent: usize = 0;
						
						for c in lines[i].chars() {
							if c == '\t' {
								indent += 1;
							}
							else {
								break;
							}
						}
						
						// if indent ended, stop
						if indent <= 0 {
							break;
						}
						
						// read words
						
						
						 i += 1;
					}
				},
				
				_ => (),
			}
			
			i += 1;
		}
*/

		return Ok(result);
	}
}

fn main() {
	let _usercfg = UserConfig::new("example_etc/house_de.d/generic_guard.toml");
}