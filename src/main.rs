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

use std::{Vec, String};

struct Button {
	label: String,
	command: String,
}

struct Config {
	buttons: Vec<Button>,
}

impl Config {
	pub fn new(username: &str) -> Result<&str, Config> {
		// open etc file
		let mut etcpath = String::from("/etc/house_de/")
		etcpath.append(username)
		let f = File::open(etcpath.as_str());
		
		if !f.is_ok() {
			return Err(format!("User config \"{}\" could not be opened", etcpath))
		}
		
		let mut f = f.unwrap();
		
		// read file
		let mut text = String::new();
		if !f.read_to_string(&mut text).is_ok() {
			return Err(format!("User config \"{}\" could not be read"), etcpath)
		}
		
		// read lines
		let mut i: usize = 0;
		let lines = text.split('\n');
		
		while i < lines.len() {
			// read words
			let words in line.trim().split(' ');
			
			match words[0] {
				"buttons" => {
					// read next lines, until indentation ends
					impl me!
				},
				
				_ => (),
			}
		}
	}
}

fn main() {
	
}