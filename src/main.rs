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
use config::{UserConfig, Button, Command};
use std::io::Write;
use termion::color;
use termion::event::Key;
use termion::raw::IntoRawMode;
use termion::input::TermRead;

fn log(logfile: &mut std::fs::File, msg: &str) {
	if writeln!(logfile, "{}", msg).is_ok() == false {
		panic!("Could not write to logfile");
	}
}

fn main() {
	// open log file
	let logpath = format!("{}/{}", env!("HOME"), "house_de_log");
	let logfile = std::fs::File::options().append(true).open(logpath);
	
	if !logfile.is_ok() {
		panic!("Log file could not be opened");
	}
	
	let mut logfile = logfile.unwrap();
	
	// read app config
	let cfg_path: String = format!("/etc/{}.json", env!("CARGO_PKG_NAME"));
	
	// login
	// TODO login
	
	let username = "generic_guard".to_string();
	let usercfg_path = /* format!("/etc/{}.d/{}.json", env!("CARGO_PKG_NAME"), username).to_string(); override */
		format!("example_etc/house_de.d/{}.json", username);
	
	// read user config
	let usrcfg_result = UserConfig::from_json(usercfg_path.as_str());
	let usercfg: UserConfig;
	
	// if usercfg invalid use recovery cfg, else use read
	if !usrcfg_result.is_ok() {
		usercfg = UserConfig {
			motd: "Userconfig invalid, using recovery mode".to_string(),
			main_menu: Button::from(
				"recovery",
				Command::new(),
				vec![
					Button::from(
						"Edit userconfig",
						Command::new(),
						vec![
							Button::from(
								"(sudo) Edit user config via Vim",
								Command::from("sudo", format!("vim {}", usercfg_path).as_str()),
								Vec::<Button>::new(),
							),
							
							Button::from(
								"(sudo) Edit user config via Nano",
								Command::from("sudo", format!("nano {}", usercfg_path).as_str()),
								Vec::<Button>::new(),
							),
							
							Button::from(
								"(sudo) Edit user config via Vi",
								Command::from("sudo", format!("vi {}", usercfg_path).as_str()),
								Vec::<Button>::new(),
							),
						],
					),
					
					Button::from(
						"Edit config",
						Command::new(),
						vec![
							Button::from(
								"(sudo) Edit user config via Vim",
								Command::from("sudo", format!("vim {}", cfg_path).as_str()),
								Vec::<Button>::new(),
							),
							
							Button::from(
								"(sudo) Edit user config via Nano",
								Command::from("sudo", format!("nano {}", cfg_path).as_str()),
								Vec::<Button>::new(),
							),
							
							Button::from(
								"(sudo) Edit user config via Vi",
								Command::from("sudo", format!("vi {}", cfg_path).as_str()),
								Vec::<Button>::new(),
							),
						],
					),
					
					/*Button::from(
						"View log",
						Command::from(
							"",
							"",
						),
						Vec::<Button>::new(),
					),*/
				],
			),
		};
	}
	else {
		usercfg = usrcfg_result.unwrap();
	}

	// get term size, get stdout, hide cursor
	let mut hover: usize = 0;
	let mut menu_path = Vec::<usize>::new();
	let mut msg = String::new();
	let term_size = termion::terminal_size();
	
	if !term_size.is_ok() {
		const MSG: &str = "Terminal size could not be determined";
		
		log(&mut logfile, MSG);
		panic!("{}", MSG);
	}
	
	let (_term_w, term_h) = term_size.unwrap();

	let stdout = std::io::stdout().into_raw_mode();
	
	if !stdout.is_ok() {
		panic!("Stdout raw mode failed");
	}
	
	let mut stdout = stdout.unwrap();
	
	let presult = write!(stdout, "{}", termion::cursor::Hide);
	
	if !presult.is_ok() {
		log(&mut logfile, "Could not hide cursor");
	}
	
	// mainloop
	'mainloop: loop {
		// find current menu
		let cur_menu: &Button;
		
		if menu_path.len() > 0 {
			let mut submenu = &usercfg.main_menu;
			
			for i in 0..menu_path.len() {
				submenu = &submenu.buttons[menu_path[i]];
			}
			
			cur_menu = submenu;
		}
		else {
			cur_menu = &usercfg.main_menu;
		}
		
		// clear
		print!("{}", termion::clear::All);
		
		// display menu
		for i in 0..cur_menu.buttons.len() {
			// if button is submenu, add brackets
			let mut br: [&str; 2] = [
				"",
				""
			];
			
			if cur_menu.buttons[i].buttons.len() > 0 {
				br[0] = "[";
				br[1] = "]";
			}
			
			// if cursor on cur button, change colors
			let mut fg = color::Fg(color::Rgb(255, 255, 255));
			let mut bg = color::Bg(color::Rgb(0, 0, 0));
			
			if hover == i {
				fg = color::Fg(color::Rgb(0, 0, 0));
				bg = color::Bg(color::Rgb(255, 255, 255));
			}
			
			// print
			let presult = write!(stdout, "{}{}{}{}{}{}{}{}",
				termion::cursor::Goto(1, (i + 1) as u16),
				fg,
				bg,
				br[0], cur_menu.buttons[i].label, br[1],
				color::Fg(color::Reset),
				color::Bg(color::Reset),
			    );
			
			if !presult.is_ok() {
				log(&mut logfile, "Could not print button");
			}
		}
		
		// trim msg
		msg = msg.trim().to_string();
		
		// if msg, display msg
		if msg.len() > 0 {
			let presult = write!(stdout, "{}{}",
				termion::cursor::Goto(1, term_h),
				msg,
				);
			
			if !presult.is_ok() {
				log(&mut logfile, "Could not print message");
			}
		}
		
		if !stdout.flush().is_ok() {
			log(&mut logfile, "Could not flush stdout");
		}

		// input
		let stdin = std::io::stdin();
		
		for key in stdin.keys() {
			if !key.is_ok() {
				continue;
			}
		
			match key.unwrap() {
				Key::Ctrl('q') => {
					break 'mainloop;
				},
				
				Key::Up => {
					// if possible, go up and clear msg
					if hover > 0 {
						hover -= 1;
						msg.clear();
						break;
					}
				},
				
				Key::Down => {
					// if possible, go down and clear msg
					if hover < cur_menu.buttons.len() - 1 {
						hover += 1;
						msg.clear();
						break;
					}
				},
				
				Key::Right => {
					// if hovered btn has menu, change menu and clear msg
					if cur_menu.buttons[hover].buttons.len() > 0 {
						menu_path.push(hover);
						hover = 0;
						msg.clear();
						break;
					}
				},
				
				Key::Left => {
					// if currently in submenu, go back and clear msg
					if menu_path.len() > 0 {
						menu_path.pop();
						hover = 0;
						msg.clear();
						break;
					}
				},
				
				Key::Char('\n') => {
					// if hovered btn has cmd, execute output mode
					if cur_menu.buttons[hover].cmd.exe.len() > 0 {
						let execresult = std::process::Command
							::new(&cur_menu.buttons[hover].cmd.exe)
							.args(&cur_menu.buttons[hover].cmd.args)
							.current_dir(env!("HOME"))
							.output();

						if !execresult.is_ok() {
							log(&mut logfile, "Could not execute command");
							
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
						
						break;
					}
				},
				
				_ => (),
			}
		}
	}
	
	// show cursor
	let presult = write!(stdout, "{}", termion::cursor::Show);
	
	if !presult.is_ok() {
		log(&mut logfile, "Could not unhide the cursor");
	}
}