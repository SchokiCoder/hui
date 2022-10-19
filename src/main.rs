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

mod lua;
mod interface;
mod config;
mod menus;
mod logger;
mod deskenv;
use config::UserConfig;
use logger::Logger;
use deskenv::HouseDeMode;
use termion::input::TermRead;
use termion::raw::IntoRawMode;
use std::io::Write;

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

	// desk env data	
	let mut mode = HouseDeMode::Normal;
	let mut active = true;
	let mut need_draw = true;
	let mut hover: usize = 0;
	
	// get logger, stdout
	let mut lgr = Logger::new();
	
	let stdout = std::io::stdout().into_raw_mode();
	if !stdout.is_ok() {
		const MSG: &str = "Stdout raw mode failed";
		
		lgr.log(MSG);
		panic!("{}", MSG);
	}
	
	let mut stdout = stdout.unwrap();
	
	// get lua
	/*let mut lua_bools = std::collections::HashMap::<&str, bool>::new();
	lua_bools.insert("use_recoverymenu", false);
	lua_bools.insert("app_active", true);
	lua_bools.insert("logout", false);
	lua_bools.insert("suspend", false);
	
	let mut lua_strings = std::collections::HashMap::<&str, String>::new();
	lua_strings.insert("output", String::new());*/
	
	let lua = mlua::Lua::new();
	
	/*
	// set lua globals
	let ctxresult = lua.context(|lua_ctx| {
		let globals = lua_ctx.globals();
		
		for (key, value) in lua_bools.iter() {
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
		
		lgr.log(msg.as_str());
		panic!("{}", msg);
	}
	
	let ctxresult = lua.context(|lua_ctx| {
		let globals = lua_ctx.globals();
		
		for (key, value) in lua_strings.iter() {
			if !globals.set(*key, value.as_str()).is_ok() {
				return Err(key);
			}
		}
		
		return Ok(());
	});
	
	if !ctxresult.is_ok() {
		let msg = format!(
			"Lua global \"{}\" could not be set",
			ctxresult.err().unwrap());
		
		lgr.log(msg.as_str());
		panic!("{}", msg);
	}*/
		
	// get term size
	let term_size = termion::terminal_size();
	
	if !term_size.is_ok() {
		const MSG: &str = "Terminal size could not be determined";
		
		lgr.log(MSG);
		panic!("{}", MSG);
	}
	
	let (term_w, term_h) = term_size.unwrap();
	
	// get usrcfg path (debug only)
	#[cfg(debug_assertions)]
	let usrcfg_path = format!("etc/{}.d/{}.json", env!("CARGO_PKG_NAME"), username);
	
	// get usrcfg path (release only)
	#[cfg(not(debug_assertions))]
	let usrcfg_path = format!("/etc/{}.d/{}.json", env!("CARGO_PKG_NAME"), username);

	// read user config
	let usrcfg_result = UserConfig::from_json(usrcfg_path.as_str());
	let usrcfg: UserConfig;
	
	if !usrcfg_result.is_ok() {	
		let usrcfg_err = usrcfg_result.err().unwrap();
		lgr.log(usrcfg_err);
		
		usrcfg = UserConfig {
			motd: format!("{}, using recovery mode", usrcfg_err),
			main_menu: menus::new_recovery_menu(usrcfg_path.as_str()),
		};
	}
	else {
		usrcfg = usrcfg_result.unwrap();
	}
	
	let motd = usrcfg.motd;
	let mut user_menu = deskenv::Menu::new(usrcfg.main_menu);
	let mut sys_menu = deskenv::Menu::new(menus::new_sys_menu());
	let mut recovery_menu = deskenv::Menu::new(menus::new_recovery_menu(usrcfg_path.as_str()));

	let mut header: String;
	let mut content = Vec::<String>::new();
	let mut footer = String::new();
	
	// hide cursor
	if write!(stdout, "{}", termion::cursor::Hide).is_ok() == false {
		lgr.log("Could not hide cursor");
	}
	
	// mainloop
	while active {	
		// get current menu
		let cur_menu: &mut deskenv::Menu;
		
		match mode {
			HouseDeMode::Normal | HouseDeMode::Output => {
				cur_menu = &mut user_menu;
			},
			
			HouseDeMode::Sysmenu => {
				cur_menu = &mut sys_menu;
			},
			
			HouseDeMode::Recovery => {
				cur_menu = &mut recovery_menu;
			},
		}
		
		// if draw needed
		if need_draw {
			// generate strings
			if mode != HouseDeMode::Output {
				cur_menu.set_content(&mut content);
			}
			
			match mode {
				HouseDeMode::Normal => {
					header = motd.clone();
				},
				
				HouseDeMode::Sysmenu => {
					header = "HouseDE Sysmenu".to_string();
				},
				
				HouseDeMode::Recovery => {
					header = "HouseDE Recovery".to_string();
				},
				
				HouseDeMode::Output => {
					header = "HouseDE Output".to_string();
				},
			}
			
			// draw
			deskenv::draw(
				&mut  lgr,
				&mut stdout,
				mode,
				hover,
				term_h,
				&header,
				cur_menu.gen_path(term_w),
				&content,
				&footer);
			need_draw = false;
		}
		
		// input loop
		let stdin = std::io::stdin();
		
		for key in stdin.keys() {
			// handle key
			if !key.is_ok() {
				continue;
			}
		
			deskenv::handle_key(
				&mut lgr,
				&lua,
				&mut mode,
				&mut hover,
				cur_menu,
				&mut active,
				&mut need_draw,
				&mut content,
				&mut footer,
				key.unwrap());
			
			/*
			// update variables exposed to lua globals
			let ctxresult = lua.context(|lua_ctx| {
				let globals = lua_ctx.globals();
				
				for (key, value) in lua_bools.iter_mut() {
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
				
				lgr.log(msg.as_str());
				panic!("{}", msg);
			}
			
			let ctxresult = lua.context(|lua_ctx| {
				let globals = lua_ctx.globals();
				
				for (key, value) in lua_strings.iter_mut() {
					let gresult = globals.get::<_, String>(*key);
					
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
				
				lgr.log(msg.as_str());
				panic!("{}", msg);
			}
			*/
			
			/*
			// if not lua app_active, quit
			if lua_bools["app_active"] == false {
				active = false;
				break;
			}
			
			// if lua use_recoverymenu
			if lua_bools["use_recoverymenu"] {
				// set mode, reset hover
				mode = HouseDeMode::Recovery;
				hover = 0;
				
				// reset lua use_recoverymenu
				if let Some(x) = lua_bools.get_mut("use_recoverymenu") {
					*x = false;
				}
				
				let ctxresult = lua.context(|lua_ctx| {
					let globals = lua_ctx.globals();
					
					for (key, value) in lua_bools.iter() {
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
					
					lgr.log(msg.as_str());
					panic!("{}", msg);
				}
				
				// break input loop
				break;
			}
			
			// if lua_output is filled
			if lua_strings["output"].len() > 0 {
				// set output
				deskenv::set_output(
					&mut mode,
					lua_strings["output"].as_str(),
					&mut content,
					&mut footer);
			
				// reset lua_output
				if let Some(x) = lua_strings.get_mut("output") {
					x.clear();
				}
				
				let ctxresult = lua.context(|lua_ctx| {
					let globals = lua_ctx.globals();
					
					for (key, value) in lua_strings.iter() {
						if !globals.set(*key, value.as_str()).is_ok() {
							return Err(key);
						}
					}
					
					return Ok(());
				});
				
				if !ctxresult.is_ok() {
					let msg = format!(
						"Lua global \"{}\" could not be set",
						ctxresult.err().unwrap());
					
					lgr.log(msg.as_str());
					panic!("{}", msg);
				}
			}
			*/
			
			if need_draw {
				break;
			}
		}
	}
	
	// show cursor
	if write!(stdout, "{}", termion::cursor::Show).is_ok() == false {
		lgr.log("Could not unhide the cursor");
	}
}