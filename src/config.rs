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

use std::vec::Vec;
use std::string::String;
use std::fs::File;
use std::io::Read;
use crate::interface::{Button, ShellCmd};

pub struct UserConfig {
	pub motd: String,
	pub main_menu: Button,
}

impl UserConfig {
	pub fn new() -> UserConfig {
		return UserConfig {
			motd: String::new(),
			main_menu: Button {
				label: String::new(),
				buttons: Vec::<Button>::new(),
				shell: ShellCmd::new(),
				lua: String::new(),
			},
		};
	}

	pub fn from_json(filepath: &str) -> Result<UserConfig, &'static str> {	
		let mut result = UserConfig::new();
	
		// open etc file
		let etcpath = String::from(filepath);
		
		let f = File::open(etcpath.as_str());
		
		if !f.is_ok() {
			return Err("User config could not be opened")
		}
		
		let mut f = f.unwrap();
		
		// read file
		let mut text = String::new();
		if !f.read_to_string(&mut text).is_ok() {
			return Err("User config could not be read")
		}
		
		// parse json
		let mut json = json::parse(text.as_str()).unwrap();
		
		// convert to usable config		
		result.motd = json["motd"].to_string();
		
		let mnu_result = Button::from_json(json["main_menu"].take());
		
		if !mnu_result.is_ok() {
			return Err("Main menu is corrupt");
		}
		
		result.main_menu = mnu_result.unwrap();

		return Ok(result);
	}
}