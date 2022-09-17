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

enum HouseDeMode {
	Normal,
	Recovery,
	Sysmenu,
	Output,
}

struct HouseDe {
	lgr: Logger,
	stdout: termion::raw::RawTerminal,
	term_w: usize,
	term_h: usize,
	
	pub mode: HouseDeMode,
	hover: usize,
	menu_nav: Vec<usize>,
	
	motd: String,
	user_menu: Button,
	sys_menu: Button,
	recovery_menu: Button,

	header: String,
	menu_path: String,
	content: String,
	footer: String,
}

impl HouseDe {
	pub fn new(username: &str) -> HouseDe {
		// get logger
		let lgr: Logger::new();

		// get stdout
		let stdout = std::io::stdout().into_raw_mode();
	
		if !stdout.is_ok() {
			const MSG: &str = "Stdout raw mode failed";
			
			lgr.log(MSG);
			panic!("{}", MSG);
		}
		
		let stdout = stdout.unwrap();
		
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
		let usercfg_path = format!("etc/house_de.d/{}.json", username);
		
		// get usrcfg path (release only)
		#[cfg(not(debug_assertions))]
		let usercfg_path = format!("/etc/{}.d/{}.json", env!("CARGO_PKG_NAME"), username);
	
		// read user config
		let mode: HouseDeMode;
		let usrcfg_result = UserConfig::from_json(usercfg_path.as_str());
		let usercfg: UserConfig;
		
		if !usrcfg_result.is_ok() {
			mode = HouseDeMode::recovery;
			
			let usrcfg_err = usrcfg_result.err().unwrap();
			lgr.log(usrcfg_err);
			
			usercfg = UserConfig {
				motd: format!("{}, using recovery mode", usrcfg_err),
				main_menu: Button::from("", ShellCmd::new(), "", vec![]),
			};
		}
		else {
			mode = HouseDeMode::normal;
			usercfg = usrcfg_result.unwrap();
		}
		
		// return
		return HouseDe {
			lgr: lgr,
			term_w: term_w,
			term_h: term_h,
			
			mode: mode,
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
	
	pub fn hide_cursor(&self) {
		let presult = write!(self.stdout, "{}", termion::cursor::Hide);
	
		if !presult.is_ok() {
			self.lgr.log("Could not hide cursor");
		}
	}
	
	pub fn show_cursor(&self) {
		let presult = write!(self.stdout, "{}", termion::cursor::Show);
		
		if !presult.is_ok() {
			self.lgr.log("Could not unhide the cursor");
		}
	}
	
	pub fn log(&self, msg: &str) {
		self.lgr.log(msg);
	}
	
	pub fn update_text(&mut self, message: &str) {
		self.menu_path.clear();
	
		// if output mode, set menu path string
		if self.mode == HouseDeMode::Output {
			self.menu_path.push_str("Output");
		}
		else
		{
			// find current menu and build menupath string
			let mut sub_menu: &Button;
			
			match self.mode {
				HouseDeMode::Normal => {
					sub_menu = &self.usermenu;
				},
				
				HouseDeMode::Recovery => {
					sub_menu = &self.recoverymenu;
				},
				
				HouseDeMode::Sysmenu => {
					sub_menu = &self.sysmenu;
				},
			}
			
			for i in 0..self.menu_nav.len() {
				self.menu_path.push_str(&sub_menu.label);
				self.menu_path.push_str(" > ");
				sub_menu = &sub_menu.buttons[self.menu_nav[i]];
			}
			
			self.menu_path.push_str(&sub_menu.label);
			self.menu_path.push_str(" > ");
		}
		
		// if menupath string is too long, cut from begin til fit
		let diff = self.menu_path.len() as isize - self.term_w as isize;
		
		if diff > 0 {
			self.menu_path = self.menu_path.split_off(diff as usize + 3);
			self.menu_path.insert_str(0, "...");
		}
		
		// prep msg
		let msg = message.trim().to_string();
		let msg_lines: Vec<&str> = msg.lines().collect();

		// if multi line msg, set output mode
		if msg_lines.len() > 1 {
			self.mode = HouseDeMode::Output;
		}
	}

	pub fn draw(&self) {
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
			self.menupath);
		
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
			let mut fg = color::Fg(color::Rgb(255, 255, 255));
			let mut bg = color::Bg(color::Rgb(0, 0, 0));
				
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
}