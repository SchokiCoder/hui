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

mod config;
use config::UserConfig;

fn main() {
	let etcpath = String::from("example_etc/house_de.d/generic_guard.json");/*let mut etcpath = String::from("/etc/");
	etcpath.push_str(env!("CARGO_PKG_NAME"));
	etcpath.push_str(".d/");
	etcpath.push_str(username);*/
	// temp override
	
	let _usercfg = UserConfig::from_json(etcpath.as_str());
}