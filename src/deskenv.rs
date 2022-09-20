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
use crate::interface::{Button};
use termion::color;
use termion::event::Key;
use std::io::Write;

#[derive(Clone, Copy, PartialEq)]
pub enum HouseDeMode {
	Normal,
	Recovery,
	Sysmenu,
	Output,
}

pub fn draw(
	lgr: &mut Logger,
	stdout: &mut termion::raw::RawTerminal<std::io::Stdout>,
	mode: HouseDeMode,
	hover: usize,
	term_h: u16,
	header: & String,
	menu_path: String,
	content: &Vec<String>,
	footer: &String)
{
	// clear
	print!("{}", termion::clear::All);
	
	// header
	let presult = write!(stdout, "{}{}",
		termion::cursor::Goto(1, 1),
		header);
	
	if !presult.is_ok() {
		lgr.log("Could not print header");
	}
	
	// menu path
	let presult = write!(stdout, "{}{}",
		termion::cursor::Goto(1, 2),
		menu_path);
	
	if !presult.is_ok() {
		lgr.log("Could not print menu path");
	}
	
	// content
	if mode == HouseDeMode::Output {
		for i in 0..content.len() {				
			// print
			let presult = write!(stdout, "{}{}{}{}{}{}",
				termion::cursor::Goto(1, (i + 4) as u16),
				color::Fg(color::Rgb(255, 255, 255)),
				color::Bg(color::Rgb(0, 0, 0)),
				content[i],
				color::Fg(color::Reset),
				color::Bg(color::Reset),
				);
			
			if !presult.is_ok() {
				lgr.log(format!("Could not print output line {}", i).as_str());
			}
		}
	}
	else {
		let mut fg: color::Fg<color::Rgb>;
		let mut bg: color::Bg<color::Rgb>;
			
		for i in 0..content.len() {
			// if hover on cur button, change colors
			if hover == i {
				fg = color::Fg(color::Rgb(0, 0, 0));
				bg = color::Bg(color::Rgb(255, 255, 255));
			}
			else {
				fg = color::Fg(color::Rgb(255, 255, 255));
				bg = color::Bg(color::Rgb(0, 0, 0));
			}
			
			// print
			let presult = write!(stdout, "{}{}{}{}{}{}",
				termion::cursor::Goto(1, (i + 4) as u16),
				fg,
				bg,
				content[i],
				color::Fg(color::Reset),
				color::Bg(color::Reset),
				);
			
			if !presult.is_ok() {
				lgr.log(format!("Could not print menu line {}", i).as_str());
			}
		}
	}
	
	// footer
	let presult = write!(stdout, "{}{}",
		termion::cursor::Goto(1, term_h),
		footer);
	
	if !presult.is_ok() {
		lgr.log("Could not print footer");
	}
	
	// flush
	if !stdout.flush().is_ok() {
		lgr.log("Could not flush stdout");
	}
}
	
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

pub fn set_output(
	mode: &mut HouseDeMode,
	message: &str,
	content: &mut Vec<String>,
	footer: &mut String)
{
	// prep msg
	let msg = message.trim().to_string();
	let msg_lines: Vec<&str> = msg.lines().collect();

	// if multi line msg, set output mode
	if msg_lines.len() > 1 {
		*mode = HouseDeMode::Output;
		content.clear();
		
		for i in 0..msg_lines.len() {
			content.push(msg_lines[i].to_string());
		}
	}
	else {
		*footer = msg_lines[0].to_string();
	}
}

pub fn handle_key(
	lgr: &mut Logger,
	lua: &rlua::Lua,
	mode: &mut HouseDeMode,
	hover: &mut usize,
	cur_menu: &mut Menu,
	active: &mut bool,
	need_draw: &mut bool,
	content: &mut Vec<String>,
	footer: &mut String,
	key: termion::event::Key)
{
	// get current button from menu
	let mut cur_btn = &cur_menu.entry;
	
	for i in 0..cur_menu.nav.len() {
		cur_btn = &cur_btn.buttons[i];
	}
	
	// key	
	match key {
		Key::Ctrl('q') => {
			*active = false;
			*need_draw = true;
		},
		
		Key::Ctrl('s') => {
			// toggle sysmenu, flag draw
			if *mode == HouseDeMode::Sysmenu {
				*mode = HouseDeMode::Normal;
			}
			else {
				*mode = HouseDeMode::Sysmenu;
			}
			
			*hover = 0;
			*need_draw = true;
		},		
		
		Key::Up => {
			// if possible hover up, flag draw
			if *hover > 0 {
				*hover -= 1;
				*need_draw = true;
			}
		},
		
		Key::Down => {
			// if possible hover down, flag draw
			if *hover + 1 < content.len() {
				*hover += 1;
				*need_draw = true;
			}
		},
		
		Key::Right => {
			// if output mode, do nothing
			if *mode == HouseDeMode::Output {}
			else {
				// if hovered button has buttons, add to menu nav, reset hover, flag draw
				if cur_btn.buttons[*hover].buttons.len() > 0 {
					cur_menu.nav.push(*hover);
					*hover = 0;
					*need_draw = true;
				}
			}
		},
		
		Key::Left => {
			// if output mode, go normal mode
			if *mode == HouseDeMode::Output {
				*mode = HouseDeMode::Normal;
				*need_draw = true;
			}
			else {
				// if nav has anything, pop, reset hover, flag draw
				cur_menu.nav.pop();
				*hover = 0;
				*need_draw = true;
			}
		},
		
		Key::Char('\n') => {
			// if hovered btn has lua, execute
			if cur_btn.buttons[*hover].lua.len() > 0 {
				let execresult = lua.context(|lua_ctx| {
					return lua_ctx.load(&cur_btn.buttons[*hover].lua).exec();
				});
				
				if !execresult.is_ok() {
					lgr.log(format!(
						"Lua \"\n{}\n\"\nfailed to execute",
						cur_btn.buttons[*hover].lua,).as_str());
				}
				
				*need_draw = true;
			}
			
			// if hovered btn has shell, execute output mode
			if cur_btn.buttons[*hover].shell.exe.len() > 0 {
				let execresult = std::process::Command
					::new(&cur_btn.buttons[*hover].shell.exe)
					.args(&cur_btn.buttons[*hover].shell.args)
					.current_dir(env!("HOME"))
					.output();

				if !execresult.is_ok() {
					const MSG: &str = "Could not execute command";
					lgr.log(MSG);
					set_output(mode, MSG, content, footer);
				}
				else {
					// get output
					let out = execresult.unwrap();
					let mut outmsg = String::new();
					
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
					
					set_output(mode, outmsg.as_str(), content, footer);
				}
				
				*need_draw = true;
			}
		},
		
		_ => (),
	}
}

pub struct Menu {
	pub entry: Button,
	pub nav: Vec<usize>,
}

impl Menu {
	pub fn new(entry: Button) -> Menu {
		return Menu {
			entry: entry,
			nav: Vec::<usize>::new(),
		};
	}
	
	pub fn gen_path(&self, term_w: u16) -> String {
		let mut result = String::new();

		// build menupath string
		let mut sub_menu = &self.entry;
		
		for i in 0..self.nav.len() {
			result.push_str(&sub_menu.label);
			result.push_str(" > ");
			sub_menu = &sub_menu.buttons[self.nav[i]];
		}
		
		result.push_str(&sub_menu.label);
		result.push_str(" > ");
		
		// if menupath string is too long, cut from begin til fit
		let diff: isize = result.len() as isize - term_w as isize;
		
		if diff > 0 {
			result = result.split_off(diff as usize + 3);
			result.insert_str(0, "...");
		}
		
		return result;
	}
	
	pub fn set_content(&self, content: &mut Vec<String>) {
		// get current button
		let mut cur_btn = &self.entry;
		
		for i in 0..self.nav.len() {
			cur_btn = &cur_btn.buttons[i];
		}
		
		// set content
		content.clear();
		
		for i in 0..cur_btn.buttons.len() {
			content.push(cur_btn.buttons[i].label.clone());
		}
	}
}