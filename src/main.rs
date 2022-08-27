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
use config::{UserConfig, Button};
use std::io::Write;
use termion::color;
use termion::event::Key;
use termion::raw::IntoRawMode;
use termion::input::TermRead;

fn main() {
	// read app config
	let cfg_path: String = format!("/etc/{}.json", env!("CARGO_PKG_NAME"));
	
	// login
	/* not yet */
	let username = "generic_guard".to_string();
	let usercfg_path = /* format!("/etc/{}.d/{}.json", env!("CARGO_PKG_NAME"), username).to_string(); override */
		format!("example_etc/house_de.d/{}.json", username);
		
	// user dir
	let userdir = format!("/home/{}/", username);
	
	// read user config
	let usrcfg_result = UserConfig::from_json(usercfg_path.as_str());
	let usercfg: UserConfig;
	
	// if usercfg invalid use recovery cfg, else use read
	if !usrcfg_result.is_ok() {
		usercfg = UserConfig {
			motd: "Userconfig invalid, using recovery mode".to_string(),
			main_menu: Button {
				label: "recovery".to_string(),
				buttons: vec![
					Button {
						label: "Edit userconfig".to_string(),
						cmd: "".to_string(),
						buttons: vec![
							Button {
								label: "(sudo) Edit user config via Vim".to_string(),
								cmd: format!("sudo vim {}", usercfg_path),
								buttons: Vec::<Button>::new(),
							},
							
							Button {
								label: "(sudo) Edit user config via Nano".to_string(),
								cmd: format!("sudo nano {}", usercfg_path),
								buttons: Vec::<Button>::new(),
							},
							
							Button {
								label: "(sudo) Edit user config via Vi".to_string(),
								cmd: format!("sudo vi {}", usercfg_path),
								buttons: Vec::<Button>::new(),
							},
						],
					},
					
					Button {
						label: "Edit config".to_string(),
						cmd: "".to_string(),
						buttons: vec![
							Button {
								label: "(sudo) Edit user config via Vim".to_string(),
								cmd: format!("sudo vim {}", cfg_path),
								buttons: Vec::<Button>::new(),
							},
							
							Button {
								label: "(sudo) Edit user config via Nano".to_string(),
								cmd: format!("sudo nano {}", cfg_path),
								buttons: Vec::<Button>::new(),
							},
							
							Button {
								label: "(sudo) Edit user config via Vi".to_string(),
								cmd: format!("sudo vi {}", cfg_path),
								buttons: Vec::<Button>::new(),
							},
						],
					},
				],
				cmd: String::new(),
			},
		};
	}
	else {
		usercfg = usrcfg_result.unwrap();
	}

	// mainloop
	let mut hover: usize = 0;
	let mut menu_path = Vec::<usize>::new();

	let stdout = std::io::stdout().into_raw_mode();
	
	if !stdout.is_ok() {
		panic!("Stdout raw mode failed");
	}
	
	let mut stdout = stdout.unwrap();
	
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
				// TODO log this later
			}
		}
		
		if !stdout.flush().is_ok() {
			// TODO log this later
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
					// if possible, go up
					if hover > 0 {
						hover -= 1;
						break;
					}
				},
				
				Key::Down => {
					// if possible, go down
					if hover < cur_menu.buttons.len() - 1 {
						hover += 1;
						break;
					}
				},
				
				Key::Right => {
					// if hovered btn has menu, change menu
					if cur_menu.buttons[hover].buttons.len() > 0 {
						menu_path.push(hover);
						hover = 0;
						break;
					}
				},
				
				Key::Left => {
					// if currently in submenu, go back
					if menu_path.len() > 0 {
						menu_path.pop();
						hover = 0;
						break;
					}
				},
				
				Key::Char('\n') => {
					// if hovered btn has cmd, execute
					if cur_menu.buttons[hover].cmd.len() > 0 {
						let execresult = std::process::Command::new(&cur_menu.buttons[hover].cmd)
							.current_dir(&userdir)
							.stdout(std::process::Stdio::null())
							.spawn();
						
						if !execresult.is_ok() {
							// TODO log this
						}
						
						break;
					}
				},
				
				_ => (),
			}
		}
	}
}