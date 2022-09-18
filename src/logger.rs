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

use std::io::Write;

pub struct Logger {
	file: std::fs::File,
}

impl Logger {
	pub fn new() -> Logger {
		let logpath = format!("{}/{}", env!("HOME"), "house_de.log");
		let fresult: std::io::Result<std::fs::File>;
		
		// if log file doesn't exist
		if std::fs::metadata(logpath.as_str()).is_ok() == false {
			// create
			fresult = std::fs::File::create(logpath);
		}
		else {
			// open log file
			fresult = std::fs::File::options().append(true).open(logpath);
		}
		
		if !fresult.is_ok() {
			panic!("Log file could not be opened");
		}
		
		return Logger {
			file: fresult.unwrap(),
		};
	}
	
	pub fn log(&mut self, msg: &str) {
		let ts = chrono::Local
			::now()
			.format("%Y-%m-%d_%H-%M")
			.to_string();
			
		if writeln!(self.file, "[{}] {}", ts, msg).is_ok() == false {
			panic!("Could not write to logfile");
		}
	}
}