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

use crate::logger::Logger;
use crate::config::{UserConfig, Button, ShellCmd};
use termion::color;
use termion::raw::IntoRawMode;
use termion::event::Key;
use std::io::Write;

#[derive(PartialEq)]
pub enum HouseDeMode {
	Normal,
	Recovery,
	Sysmenu,
	Output,
}

pub struct HouseDe {
	lgr: Logger,
	stdout: termion::raw::RawTerminal<std::io::Stdout>,
	lua: rlua::Lua,
	lua_globals: std::collections::HashMap<&'static str, bool>,
	term_w: u16,
	term_h: u16,
	
	mode: HouseDeMode,
	pub active: bool,
	pub need_draw: bool,
	hover: usize,
	menu_nav: Vec<usize>,
	
	motd: String,
	user_menu: Button,
	sys_menu: Button,
	recovery_menu: Button,

	header: String,
	menu_path: String,
	content: Vec<String>,
	footer: String,
}

impl HouseDe {
	pub fn new(username: String) -> HouseDe {
		// get logger
		let mut lgr = Logger::new();

		// get stdout
		let stdout = std::io::stdout().into_raw_mode();
	
		if !stdout.is_ok() {
			const MSG: &str = "Stdout raw mode failed";
			
			lgr.log(MSG);
			panic!("{}", MSG);
		}
		
		let stdout = stdout.unwrap();
		
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
		let usercfg_path = format!("etc/self.d/{}.json", username);
		
		// get usrcfg path (release only)
		#[cfg(not(debug_assertions))]
		let usercfg_path = format!("/etc/{}.d/{}.json", env!("CARGO_PKG_NAME"), username);
	
		// read user config
		let mode: HouseDeMode;
		let usrcfg_result = UserConfig::from_json(usercfg_path.as_str());
		let usercfg: UserConfig;
		
		if !usrcfg_result.is_ok() {
			mode = HouseDeMode::Recovery;
			
			let usrcfg_err = usrcfg_result.err().unwrap();
			lgr.log(usrcfg_err);
			
			usercfg = UserConfig {
				motd: format!("{}, using recovery mode", usrcfg_err),
				main_menu: Button::from("", ShellCmd::new(), "", vec![]),
			};
		}
		else {
			mode = HouseDeMode::Normal;
			usercfg = usrcfg_result.unwrap();
		}
		
		// return
		return HouseDe {
			lgr: lgr,
			stdout: stdout,
			lua: lua,
			lua_globals: lua_globals,
			term_w: term_w,
			term_h: term_h,
			
			mode: mode,
			active: true,
			need_draw: true,
			hover: 0,
			menu_nav: Vec::<usize>::new(),
			
			motd: usercfg.motd,
			user_menu: usercfg.main_menu,
			
			sys_menu: Button::from(
				"Sysmenu",
				ShellCmd::new(),
				"",
				vec![
					Button::from(
						"About HouseDE",
						ShellCmd::from(
							"printf",
							format!("{}{}{}.\n{}\n{}\n\n{}\n{}",
								
								env!("CARGO_PKG_NAME"), " is licensed under the ", env!("CARGO_PKG_LICENSE"),
								"You should have received a copy of the license along with this program.",
								"If not see <https://www.gnu.org/licenses/>",
								
								"The source code of this program is available at:",
								env!("CARGO_PKG_REPOSITORY")).as_str()),
						"",
						vec![]),

					Button::from(
						"Enter Recovery Mode",
						ShellCmd::from("", ""),
						"if use_recoverymenu then use_recoverymenu = false else use_recoverymenu = true end",
						vec![]),
						
					Button::from(
						"Quit HouseDE",
						ShellCmd::from("", ""),
						"app_active = false",
						vec![]),
						
					/*Button::from(
						"Logout",
						ShellCmd::from("", ""),
						"logout = true",
						vec![]),

					Button::from(
						"Suspend",
						ShellCmd::from("", ""),
						"suspend = true",
						vec![]),

					Button::from(
						"Shutdown",
						ShellCmd::from("", ""),
						"",
						vec![]),

					Button::from(
						"Reboot",
						ShellCmd::from("", ""),
						"",
						vec![]),*/
				]
			),
			
			recovery_menu: Button::from(
				"Recovery",
				ShellCmd::new(),
				"",
				vec![
					Button::from(
						"Edit userconfig",
						ShellCmd::new(),
						"",
						vec![
							Button::from(
								"(sudo) Edit user config via Vim",
								ShellCmd::from("sudo", format!("vim {}", usercfg_path).as_str()),
								"",
								Vec::<Button>::new(),
							),
							
							Button::from(
								"(sudo) Edit user config via Nano",
								ShellCmd::from("sudo", format!("nano {}", usercfg_path).as_str()),
								"",
								Vec::<Button>::new(),
							),
							
							Button::from(
								"(sudo) Edit user config via Vi",
								ShellCmd::from("sudo", format!("vi {}", usercfg_path).as_str()),
								"",
								Vec::<Button>::new(),
							),
						],
					),
				],
			),
			
			header: String::new(),
			menu_path: String::new(),
			content: Vec::<String>::new(),
			footer: String::new(),
		};
	}
	
	pub fn hide_cursor(&mut self) {
		let presult = write!(self.stdout, "{}", termion::cursor::Hide);
	
		if !presult.is_ok() {
			self.lgr.log("Could not hide cursor");
		}
	}
	
	pub fn show_cursor(&mut self) {
		let presult = write!(self.stdout, "{}", termion::cursor::Show);
		
		if !presult.is_ok() {
			self.lgr.log("Could not unhide the cursor");
		}
	}
	
	pub fn log(&mut self, msg: &str) {
		self.lgr.log(msg);
	}
	
	pub fn gen_menu_path(&mut self) {
		self.menu_path.clear();
	
		// find current menu and build menupath string
		let mut sub_menu: &Button;
		
		match self.mode {
			HouseDeMode::Normal => {
				sub_menu = &self.user_menu;
			},
			
			HouseDeMode::Recovery => {
				sub_menu = &self.recovery_menu;
			},
			
			HouseDeMode::Sysmenu => {
				sub_menu = &self.sys_menu;
			},
			
			HouseDeMode::Output => {
				self.menu_path.push_str("Output");
				return;
			},
		}
		
		for i in 0..self.menu_nav.len() {
			self.menu_path.push_str(&sub_menu.label);
			self.menu_path.push_str(" > ");
			sub_menu = &sub_menu.buttons[self.menu_nav[i]];
		}
		
		self.menu_path.push_str(&sub_menu.label);
		self.menu_path.push_str(" > ");
		
		// if menupath string is too long, cut from begin til fit
		let diff = self.menu_path.len() as isize - self.term_w as isize;
		
		if diff > 0 {
			self.menu_path = self.menu_path.split_off(diff as usize + 3);
			self.menu_path.insert_str(0, "...");
		}
	}

	pub fn draw(&mut self) {
		// clear
		print!("{}", termion::clear::All);
		
		// header
		let presult = write!(self.stdout, "{}{}",
			termion::cursor::Goto(1, 1),
			self.header);
		
		if !presult.is_ok() {
			self.lgr.log("Could not print header");
		}
		
		// menu path
		let presult = write!(self.stdout, "{}{}",
			termion::cursor::Goto(1, 2),
			self.menu_path);
		
		if !presult.is_ok() {
			self.lgr.log("Could not print menu path");
		}
		
		// content
		if self.mode == HouseDeMode::Output {
			for i in 0..self.content.len() {				
				// print
				let presult = write!(self.stdout, "{}{}{}{}{}{}",
					termion::cursor::Goto(1, (i + 4) as u16),
					color::Fg(color::Rgb(255, 255, 255)),
					color::Bg(color::Rgb(0, 0, 0)),
					self.content[i],
					color::Fg(color::Reset),
					color::Bg(color::Reset),
					);
				
				if !presult.is_ok() {
					self.lgr.log(format!("Could not print output line {}", i).as_str());
				}
			}
		}
		else {
			let mut fg: color::Fg<color::Rgb>;
			let mut bg: color::Bg<color::Rgb>;
				
			for i in 0..self.content.len() {
				// if hover on cur button, change colors
				if self.hover == i {
					fg = color::Fg(color::Rgb(0, 0, 0));
					bg = color::Bg(color::Rgb(255, 255, 255));
				}
				else {
					fg = color::Fg(color::Rgb(255, 255, 255));
					bg = color::Bg(color::Rgb(0, 0, 0));
				}
				
				// print
				let presult = write!(self.stdout, "{}{}{}{}{}{}",
					termion::cursor::Goto(1, (i + 4) as u16),
					fg,
					bg,
					self.content[i],
					color::Fg(color::Reset),
					color::Bg(color::Reset),
					);
				
				if !presult.is_ok() {
					self.lgr.log(format!("Could not print menu line {}", i).as_str());
				}
			}
		}
		
		// footer
		let presult = write!(self.stdout, "{}{}",
			termion::cursor::Goto(1, self.term_h),
			self.footer,
			);
		
		if !presult.is_ok() {
			self.lgr.log("Could not print footer");
		}
		
		// flush
		if !self.stdout.flush().is_ok() {
			self.lgr.log("Could not flush stdout");
		}
	}
	
	fn cur_menu(&self) -> &Button {	
		let mut result: &Button;
		
		match self.mode {
			HouseDeMode::Normal | HouseDeMode::Output => {
				result = &self.user_menu;
			},
			
			HouseDeMode::Sysmenu => {
				result = &self.sys_menu;
			},
			
			HouseDeMode::Recovery => {
				result = &self.recovery_menu;
			},
		}
		
		for i in 0..self.menu_nav.len() {
			result = &result.buttons[i];
		}
		
		return result;
	}
	
	pub fn update_lua(&mut self) {
		// update variables exposed to lua globals
		let ctxresult = self.lua.context(|lua_ctx| {
			let globals = lua_ctx.globals();
			
			for (key, value) in self.lua_globals.iter_mut() {
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
			
			self.log(msg.as_str());
			panic!("{}", msg);
		}
		
		// if not lua app_active, quit
		if self.lua_globals["app_active"] == false {
			self.active = false;
		}
		
		// if lua use_recoverymenu
		if self.lua_globals["use_recoverymenu"] {
			// set mode
			self.mode = HouseDeMode::Recovery;
			
			// reset lua use_recoverymenu
			self.lua_globals["use_recoverymenu"] = false;
			
			let ctxresult = self.lua.context(|lua_ctx| {
				let globals = lua_ctx.globals();
				
				for (key, value) in self.lua_globals.iter_mut() {
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
				
				self.log(msg.as_str());
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
	}
	
	pub fn set_output(&mut self, output: &str) {
		// prep msg
		let msg = output.trim().to_string();
		let msg_lines: Vec<&str> = msg.lines().collect();

		// if multi line msg, set output mode
		if msg_lines.len() > 1 {
			self.mode = HouseDeMode::Output;
			self.content.clear();
			
			for i in 0..msg_lines.len() {
				self.content.push(msg_lines[i].to_string());
			}
		}
		else {
			self.footer = msg_lines[0].to_string();
		}
	}
	
	pub fn handle_key(&mut self, key: termion::event::Key) {
		match key {
			Key::Ctrl('q') => {
				self.active = false;
			},
			
			Key::Ctrl('s') => {
				// toggle sysmenu, update
				if self.mode == HouseDeMode::Sysmenu {
					self.mode = HouseDeMode::Normal;
				}
				else {
					self.mode = HouseDeMode::Sysmenu;
				}
				
				self.hover = 0;
				self.need_draw = true;
			},		
			
			Key::Up => {
				// if possible hover up, update
				if self.hover > 0 {
					self.hover -= 1;
					self.need_draw = true;
				}
			},
			
			Key::Down => {
				// if possible hover down, update
				if self.hover + 1 < self.content.len() {
					self.hover -= 1;
					self.need_draw = true;
				}
			},
			
			Key::Right => {
				// if output mode, do nothing
				if self.mode == HouseDeMode::Output {}
				else {
					// if hovered button has buttons, add to menu nav, reset hover, update
					if self.cur_menu().buttons[self.hover].buttons.len() > 0 {
						self.menu_nav.push(self.hover);
						self.hover = 0;
						self.need_draw = true;
					}
				}
			},
			
			Key::Left => {
				// if output mode, go normal mode
				if self.mode == HouseDeMode::Output {
					self.mode = HouseDeMode::Normal;
				}
				else {
					// if nav has anything, pop
					self.menu_nav.pop();
				}
			},
			
			Key::Char('\n') => {
				// if hovered btn has lua, execute
				if self.cur_menu().buttons[self.hover].lua.len() > 0 {
					let execresult = self.lua.context(|lua_ctx| {
						return lua_ctx.load(&self.cur_menu().buttons[self.hover].lua).exec();
					});
					
					if !execresult.is_ok() {
						self.log(format!(
							"Lua \"\n{}\n\"\nfailed to execute",
							self.cur_menu().buttons[self.hover].lua,).as_str());
					}
					
					self.need_draw = true;
				}
				
				// if hovered btn has shell, execute output mode
				else if self.cur_menu().buttons[self.hover].shell.exe.len() > 0 {
					let execresult = std::process::Command
						::new(&self.cur_menu().buttons[self.hover].shell.exe)
						.args(&self.cur_menu().buttons[self.hover].shell.args)
						.current_dir(env!("HOME"))
						.output();

					if !execresult.is_ok() {
						const MSG: &str = "Could not execute command";
						self.log(MSG);
						self.set_output(MSG);
					}
					else {
						// get output
						let out = execresult.unwrap();
						let outmsg = String::new();
						
						// if exit status isn't ok, use stderr as msg, else stdout							
						if !out.status.success() {
							outmsg.push_str("ERR: ");
							
							for c in out.stderr {
								outmsg.push(c as char);
							}
						}
						else {
							for c in out.stdout {
								outmsg.push(c as char);
							}
						}
						
						self.set_output(outmsg.as_str());
					}
					
					self.need_draw = true;
				}
			},
			
			_ => (),
		}
	}
}