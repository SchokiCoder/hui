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
mod menus;
mod logger;
mod deskenv;
use config::{UserConfig, Button, ShellCmd};
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
	let mut mode: HouseDeMode;
	let mut active = true;
	let mut need_draw = false;
	let mut hover: usize = 0;
	let mut menu_nav = Vec::<usize>::new();
	
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
		
		lgr.log(msg.as_str());
		panic!("{}", msg);
	}
		
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
	let usrcfg_path = format!("etc/d/{}.json", username);
	
	// get usrcfg path (release only)
	#[cfg(not(debug_assertions))]
	let usrcfg_path = format!("/etc/{}.d/{}.json", env!("CARGO_PKG_NAME"), username);

	// read user config
	let usrcfg_result = UserConfig::from_json(usrcfg_path.as_str());
	let usrcfg: UserConfig;
	
	if !usrcfg_result.is_ok() {
		mode = HouseDeMode::Recovery;
		
		let usrcfg_err = usrcfg_result.err().unwrap();
		lgr.log(usrcfg_err);
		
		usrcfg = UserConfig {
			motd: format!("{}, using recovery mode", usrcfg_err),
			main_menu: Button::from("", ShellCmd::new(), "", vec![]),
		};
	}
	else {
		mode = HouseDeMode::Normal;
		usrcfg = usrcfg_result.unwrap();
	}
	
	let motd = usrcfg.motd;
	let user_menu = usrcfg.main_menu;
	let sys_menu = menus::new_sys_menu();
	let recovery_menu = menus::new_recovery_menu(usrcfg_path.as_str());

	let header = String::new();
	let mut menu_path: String;
	let mut content = Vec::<String>::new();
	let mut footer = String::new();
	
	// hide cursor
	if write!(stdout, "{}", termion::cursor::Hide).is_ok() == false {
		lgr.log("Could not hide cursor");
	}
	
	// mainloop
	while active {
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
			
			lgr.log(msg.as_str());
			panic!("{}", msg);
		}
		
		// if not lua app_active, quit
		if lua_globals["app_active"] == false {
			active = false;
		}
		
		// if lua use_recoverymenu
		if lua_globals["use_recoverymenu"] {
			// set mode
			mode = HouseDeMode::Recovery;
			
			// reset lua use_recoverymenu
			if let Some(x) = lua_globals.get_mut("use_recoverymenu") {
				*x = false;
			}
			
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
				
				lgr.log(msg.as_str());
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
		
		// draw if needed
		if need_draw {
			menu_path = deskenv::gen_menu_path(
				mode,
				&menu_nav,
				term_w,
				&user_menu,
				&sys_menu,
				&recovery_menu);
			deskenv::draw(
				&mut  lgr,
				&mut stdout,
				mode,
				hover,
				term_h,
				&header,
				&menu_path,
				&content,
				&footer);
			need_draw = false;
		}
		
		// get current menu
		let mut cur_menu: &Button;
		
		match mode {
			HouseDeMode::Normal | HouseDeMode::Output => {
				cur_menu = &user_menu;
			},
			
			HouseDeMode::Sysmenu => {
				cur_menu = &sys_menu;
			},
			
			HouseDeMode::Recovery => {
				cur_menu = &recovery_menu;
			},
		}
		
		for i in 0..menu_nav.len() {
			cur_menu = &cur_menu.buttons[i];
		}
		
		// input
		let stdin = std::io::stdin();
		
		for key in stdin.keys() {
			if !key.is_ok() {
				continue;
			}
		
			deskenv::handle_key(
				&mut lgr,
				&lua,
				&mut mode,
				&mut hover,
				&mut menu_nav,
				&mut active,
				&mut need_draw,
				&mut content,
				&mut footer,
				&cur_menu,
				key.unwrap());
			
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