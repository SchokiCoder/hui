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
mod logger;
mod desktop_environment;
use desktop_environment::{HouseDe};
use termion::input::TermRead;

/*
#[cfg(not(debug_assertions))]
fn login(lgr: &mut Logger) -> String {
	let mut username = String::new();
	
	loop {
		print!("Login:    ");
		if !std::io::stdout().flush().is_ok() {}
		
		if !std::io::stdin().read_line(&mut username).is_ok() {
			const MSG: &str = "Username could not be read";
			lgr.log(MSG);
			panic!("{}", MSG);
		}
		
		username = username.trim().to_string();
		
		let logpw = rpassword::prompt_password("Password: ");
		
		if !logpw.is_ok() {
			const MSG: &str = "Password could not be read";
			lgr.log(MSG);
			panic!("{}", MSG);
		}
		
		let mut logpw = logpw.unwrap();
		
		// pam auth
		let auth = pam::Authenticator::with_password(env!("CARGO_PKG_NAME"));
		
		if !auth.is_ok() {
			const MSG: &str = "Authentication failed";
			lgr.log(MSG);
			panic!("{}", MSG);
		}
		
		let mut auth = auth.unwrap();

		auth.get_handler().set_credentials(username.as_str(), logpw.as_str());
				
		// if login is correct
		if auth.authenticate().is_ok() {			
			// overwrite pw string, clear
			for i in 0..logpw.len() {
				logpw.replace_range(i..i + 1, "-");
			}
			logpw.clear();
			
			// try open session, break out
			if !auth.open_session().is_ok() {
				const MSG: &str = "Session could not be opened";
				lgr.log(MSG);
				panic!("{}", MSG);
			}
			
			break;
		}
	}
	
	return username;
}
*/

fn main() {
	// use debug name (debug only)
	#[cfg(debug_assertions)]
	let username = "debug".to_string();
	
/*
	// login and get name (release only)
	#[cfg(not(debug_assertions))]
	let username = login(&mut lgr);

	this block is replaced, see below
*/
	// get user name (release only)
	#[cfg(not(debug_assertions))]
	let username = env!("USER");
	
	// desk env
	let house_de = HouseDe::new(username);
	
	house_de.hide_cursor();
	
	// mainloop		
	while house_de.active {
		house_de.update_lua();
		
		if house_de.need_draw {
			house_de.gen_menu_path();
			house_de.draw();
			house_de.need_draw = false;
		}
		
		// input
		let stdin = std::io::stdin();
		
		for key in stdin.keys() {
			if !key.is_ok() {
				continue;
			}
		
			house_de.handle_key(key.unwrap());
		}
	}
	
	house_de.show_cursor();	
}