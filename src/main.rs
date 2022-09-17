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
use config::{UserConfig, Button, ShellCmd};
use desktop_environment::{HouseDe, HouseDeMode};
use std::io::Write;
use termion::event::Key;
use termion::raw::IntoRawMode;
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
	
	// get lua goin, set globals
	let mut lua_globals = std::collections::HashMap::new();
	lua_globals.insert("use_recoverymenu", false);
	lua_globals.insert("app_active", true);
	/*lua_globals.insert("logout", false);
	lua_globals.insert("suspend", false);*/
	let lua = rlua::Lua::new();
	
	let ctxresult = lua.context(|lua_ctx| {
		let globals = lua_ctx.globals();
		
		for (key, value) in lua_globals.iter() {
			if !globals.set(*key, *value).is_ok() {
				return Err(key);
			}
		}
		
		return Ok(());
	});
	
	if !ctxresult.is_ok() {
		let msg = format!(
			"Lua global \"{}\" could not be set",
			ctxresult.err().unwrap());
		
		house_de.log(msg.as_str());
		panic!("{}", msg);
	}
	
	// mainloop		
	'mainloop: loop {
		// update variables exposed to lua globals
		let ctxresult = lua.context(|lua_ctx| {
			let globals = lua_ctx.globals();
			
			for (key, value) in lua_globals.iter_mut() {
				let gresult = globals.get::<_, bool>(*key);
				
				if gresult.is_ok() {
					*value = gresult.unwrap();
				}
				else {
					return Err(key);
				}
			}
			
			return Ok(());
		});
		
		if !ctxresult.is_ok() {
			let msg = format!(
				"Lua global \"{}\" could not be get",
				ctxresult.err().unwrap());
			
			house_de.log(msg.as_str());
			panic!("{}", msg);
		}
		
		// if not lua app_active, quit
		if lua_globals["app_active"] == false {
			break 'mainloop;
		}
		
		// if lua use_recoverymenu
		if lua_globals["use_recoverymenu"] {
			// set mode
			house_de.mode = HouseDeMode::Recovery;
			
			// reset lua use_recoverymenu
			lua_globals["use_recoverymenu"] = false;
			
			let ctxresult = lua.context(|lua_ctx| {
				let globals = lua_ctx.globals();
				
				for (key, value) in lua_globals.iter_mut() {
					let gresult = globals.get::<_, bool>(*key);
					
					if gresult.is_ok() {
						*value = gresult.unwrap();
					}
					else {
						return Err(key);
					}
				}
				
				return Ok(());
			});
			
			if !ctxresult.is_ok() {
				let msg = format!(
					"Lua global \"{}\" could not be set",
					ctxresult.err().unwrap());
				
				house_de.log(msg.as_str());
				panic!("{}", msg);
			}
		}
		
		/*
		// if lua logout, shell exit, quit
		if logout {
			if !std::process::Command::new("exit").spawn().is_ok() {
				lgr.log("Logout could not exit");
			}
			break 'mainloop;
		}
		
		// if lua suspend, shell suspend
		if suspend {
			if !std::process::Command::new("systemctl suspend").spawn().is_ok() {
				lgr.log("Suspend did not work");
			}
		}
		*/
		
		// desk env update and draw
		house_de.update_text();
		house_de.draw();
		
		// input
		let stdin = std::io::stdin();
		
		for key in stdin.keys() {
			// handle keys
			if !key.is_ok() {
				continue;
			}
		
			match key.unwrap() {
				Key::Ctrl('q') => {
					break 'mainloop;
				},
				
				Key::Ctrl('s') => {
					if house_de.mode == HouseDeMode::Sysmenu {
						house_de.mode = HouseDeMode::Normal;
					}
					else {
						house_de.mode = HouseDeMode::Sysmenu;
					}
					
					hover = 0;
					continue 'mainloop;
				},		
				
				Key::Up => {
					// if possible hover up, continue
					if house_de.hover > 0 {
						house_de.hover -= 1;
						continue 'mainloop;
					}
				},
				
				Key::Down => {
					// if possible hover down, continue
					if house_de.hover + 1 < house_de.content.len() {
						house_de.hover -= 1;
						continue 'mainloop;
					}
				},
				
				rework progress here;
				
				
				Key::Right => {
					// if output mode, do nothing
					if house_de.mode == HouseDeMode::Ouput {}
					else {
						if house_de.user_menu[house_de.hover]
						house_de.menu_nav.push(house_de.hover)
						
						// if hovered btn has menu, change menu and clear msg
						if cur_menu.buttons[hover].buttons.len() > 0 {
							menu_path.push(hover);
							hover = 0;
							msg.clear();
							continue 'mainloop;
						}
					}
				},
				
				Key::Left => {
					// if menu suppressed, clear message
					if suppress_menu == true {
						msg.clear();
						continue 'mainloop;
					}
					else {
						// if currently in submenu, go back and clear msg
						if menu_path.len() > 0 {
							menu_path.pop();
							hover = 0;
							msg.clear();
							continue 'mainloop;
						}
					}
				},
				
				Key::Char('\n') => {
					let mut break_input: bool = false;
					// if hovered btn has lua, execute
					if cur_menu.buttons[hover].lua.len() > 0 {
						let execresult = lua.context(|lua_ctx| {
							return lua_ctx.load(&cur_menu.buttons[hover].lua).exec();
						});
						
						if !execresult.is_ok() {
							lgr.log(format!(
								"Lua \"\n{}\n\"\nfailed to execute",
								cur_menu.buttons[hover].lua,).as_str());
						}
						
						break_input = true;
					}
					
					// if hovered btn has shell, execute output mode
					if cur_menu.buttons[hover].shell.exe.len() > 0 {
						let execresult = std::process::Command
							::new(&cur_menu.buttons[hover].shell.exe)
							.args(&cur_menu.buttons[hover].shell.args)
							.current_dir(env!("HOME"))
							.output();

						if !execresult.is_ok() {
							lgr.log("Could not execute command");
							
							// msg = exec error
							msg = "ERR: Couldn't execute command.".to_string();
						}
						else {
							// get output
							let out = execresult.unwrap();
							
							// if exit status isn't ok, use stderr as msg, else stdout
							msg.clear();
							
							if !out.status.success() {
								msg.push_str("ERR: ");
								
								for c in out.stderr {
									msg.push(c as char);
								}
							}
							else {
								for c in out.stdout {
									msg.push(c as char);
								}
							}
						}
						
						break_input = true;
					}
					
					if break_input == true {
						continue 'mainloop;
					}
				},
				
				_ => (),
			}
		}
	}
	
	house_de.show_cursor();	
}