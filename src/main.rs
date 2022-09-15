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
use config::{UserConfig, Button, ShellCmd};
use std::io::Write;
use termion::color;
use termion::event::Key;
use termion::raw::IntoRawMode;
use termion::input::TermRead;

struct Logger {
	file: std::fs::File,
}

impl Logger {
	pub fn new() -> Logger {
		let logpath = format!("{}/{}", env!("HOME"), "house_de.log");
		let fresult: std::io::Result<std::fs::File>;
		
		// if log file doesn't exist
		if std::fs::metadata(logpath.as_str()).is_ok() == false {
			// create
			fresult = std::fs::File::create(logpath);
		}
		else {
			// open log file
			fresult = std::fs::File::options().append(true).open(logpath);
		}
		
		if !fresult.is_ok() {
			panic!("Log file could not be opened");
		}
		
		return Logger {
			file: fresult.unwrap(),
		};
	}
	
	pub fn log(&mut self, msg: &str) {
		let ts = chrono::Local
			::now()
			.format("%Y-%m-%d_%H-%M")
			.to_string();
			
		if writeln!(self.file, "[{}] {}", ts, msg).is_ok() == false {
			panic!("Could not write to logfile");
		}
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

fn main() {
	// open logfile
	let mut lgr = Logger::new();
	
	// read app config
	let cfg_path: String = format!("/etc/{}.json", env!("CARGO_PKG_NAME"));
	
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

	// get usrcfg path (debug only)
	#[cfg(debug_assertions)]
	let usercfg_path = format!("etc/house_de.d/{}.json", username);
	
	// get usrcfg path (release only)
	#[cfg(not(debug_assertions))]
	let usercfg_path = format!("/etc/{}.d/{}.json", env!("CARGO_PKG_NAME"), username);
	
	let sysmenu = Button::from(
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
				"Toggle Recovery Mode",
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
	);
	
	let recoverymenu = Button::from(
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
			
			Button::from(
				"Edit config",
				ShellCmd::new(),
				"",
				vec![
					Button::from(
						"(sudo) Edit user config via Vim",
						ShellCmd::from("sudo", format!("vim {}", cfg_path).as_str()),
						"",
						Vec::<Button>::new(),
					),
					
					Button::from(
						"(sudo) Edit user config via Nano",
						ShellCmd::from("sudo", format!("nano {}", cfg_path).as_str()),
						"",
						Vec::<Button>::new(),
					),
					
					Button::from(
						"(sudo) Edit user config via Vi",
						ShellCmd::from("sudo", format!("vi {}", cfg_path).as_str()),
						"",
						Vec::<Button>::new(),
					),
				],
			),
		],
	);
	
	// read user config
	let usrcfg_result = UserConfig::from_json(usercfg_path.as_str());
	let usercfg: UserConfig;
	
	// if usercfg invalid use recovery cfg, else use read
	let force_recovery: bool;
	
	if !usrcfg_result.is_ok() {
		force_recovery = true;
		
		let usrcfg_err = usrcfg_result.err().unwrap();
		lgr.log(usrcfg_err);
		
		usercfg = UserConfig {
			motd: format!("{}, using recovery mode", usrcfg_err),
			main_menu: Button::from("", ShellCmd::new(), "", vec![]),
		};
	}
	else {
		force_recovery = false;
		usercfg = usrcfg_result.unwrap();
	}

	// get term size, get stdout, hide cursor
	let mut use_sysmenu = false;
	let mut hover: usize = 0;
	let mut menu_path = Vec::<usize>::new();
	let mut msg = String::new();
	let term_size = termion::terminal_size();
	
	if !term_size.is_ok() {
		const MSG: &str = "Terminal size could not be determined";
		
		lgr.log(MSG);
		panic!("{}", MSG);
	}
	
	let (term_w, term_h) = term_size.unwrap();

	let stdout = std::io::stdout().into_raw_mode();
	
	if !stdout.is_ok() {
		const MSG: &str = "Stdout raw mode failed";
		
		lgr.log(MSG);
		panic!("{}", MSG);
	}
	
	let mut stdout = stdout.unwrap();
	
	let presult = write!(stdout, "{}", termion::cursor::Hide);
	
	if !presult.is_ok() {
		lgr.log("Could not hide cursor");
	}
	
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
	
	// mainloop
	let mut app_active: bool;
	let mut use_recoverymenu: bool = false;
	/*let mut logout: bool;
	let mut suspend: bool;*/
			
	'mainloop: loop {
		let mut menupath_str = String::new();
		let cur_menu: &Button;
		
		// force recovery
		if force_recovery == true {
				use_recoverymenu = true;
		}
		
		// find current menu and build menupath string
		let mut submenu: &Button;
		
		// set as first menu: sysmenu, recovery menu or main menu
		if use_sysmenu {
			submenu = &sysmenu;
		}
		else if use_recoverymenu {
			submenu = &recoverymenu;
		}
		else {				
			submenu = &usercfg.main_menu;
		}
		
		for i in 0..menu_path.len() {
			menupath_str.push_str(&submenu.label);
			menupath_str.push_str(" > ");
			submenu = &submenu.buttons[menu_path[i]];
		}
		
		cur_menu = submenu;
		menupath_str.push_str(&cur_menu.label);
		menupath_str.push_str(" > ");
		
		// trim msg
		msg = msg.trim().to_string();
		
		// if msg
		let msg_lines: Vec<&str> = msg.lines().collect();
		let suppress_menu: bool;
		
		if msg_lines.len() > 1 {
			// msg is not single line, suppress menu, append to menupath
			suppress_menu = true;
			menupath_str.push_str("Output:");
		}
		// if msg is single line, print directly		
		else if msg_lines.len() == 1 {
			suppress_menu = false;
			
			let presult = write!(stdout, "{}{}",
				termion::cursor::Goto(1, term_h),
				msg,
				);
			
			if !presult.is_ok() {
				lgr.log("Could not print message");
			}
		}
		else {
			suppress_menu = false;
		}
		
		// if menupath string is too long, cut from begin til fit
		let diff = menupath_str.len() as isize - term_w as isize;
		
		if diff > 0 {
			menupath_str = menupath_str.split_off(diff as usize + 3);
			menupath_str.insert_str(0, "...");
		}
		
		// clear
		print!("{}", termion::clear::All);
		
		// if force revocery, display recovery motd, else normal motd
		let cur_motd: &str;
		
		if force_recovery {
			cur_motd = "Forced recovery mode, fix your config!";
		}
		else {
			cur_motd = &usercfg.motd;
		}
		
		let presult = write!(stdout, "{}{}",
			termion::cursor::Goto(1, 1),
			cur_motd);
		
		if !presult.is_ok() {
			lgr.log("Could not print motd");
		}
		
		// display menu path
		let presult = write!(stdout, "{}{}",
			termion::cursor::Goto(1, 2),
			menupath_str);
		
		if !presult.is_ok() {
			lgr.log("Could not print menu path");
		}
		
		// if menu suppressed, display msg, else menu
		if suppress_menu {
			for i in 0..msg_lines.len() {
				// print
				let presult = write!(stdout, "{}{}{}{}{}{}",
					termion::cursor::Goto(1, (i + 4) as u16),
					color::Fg(color::Rgb(255, 255, 255)),
					color::Bg(color::Rgb(0, 0, 0)),
					msg_lines[i],
					color::Fg(color::Reset),
					color::Bg(color::Reset),
					);
				
				if !presult.is_ok() {
					lgr.log("Could not print button");
				}
			}
		}
		else {
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
					termion::cursor::Goto(1, (i + 4) as u16),
					fg,
					bg,
					br[0], cur_menu.buttons[i].label, br[1],
					color::Fg(color::Reset),
					color::Bg(color::Reset),
					);
				
				if !presult.is_ok() {
					lgr.log("Could not print button");
				}
			}
		}
		
		if !stdout.flush().is_ok() {
			lgr.log("Could not flush stdout");
		}

		'input: loop {
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
			
			// update each lua global's variable
			app_active = lua_globals["app_active"];
			use_recoverymenu = lua_globals["use_recoverymenu"];
			/*logout = lua_globals["logout"];
			suspend = lua_globals["suspend"];*/
			
			// if not app_active, quit
			if app_active == false {
				break 'mainloop;
			}
			/*
			// if logout, shell exit, quit
			if logout {
				if !std::process::Command::new("exit").spawn().is_ok() {
					lgr.log("Logout could not exit");
				}
				break 'mainloop;
			}
			
			// if suspend, shell suspend
			if suspend {
				if !std::process::Command::new("systemctl suspend").spawn().is_ok() {
					lgr.log("Suspend did not work");
				}
			}
			*/
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
						use_sysmenu = !use_sysmenu;
						hover = 0;
						break 'input;
					},
					
					Key::Up => {
						// if possible, go up and clear msg
						if hover > 0 {
							hover -= 1;
							msg.clear();
							break 'input;
						}
					},
					
					Key::Down => {
						// if possible, go down and clear msg
						if hover < cur_menu.buttons.len() - 1 {
							hover += 1;
							msg.clear();
							break 'input;
						}
					},
					
					Key::Right => {
						// if hovered btn has menu, change menu and clear msg
						if cur_menu.buttons[hover].buttons.len() > 0 {
							menu_path.push(hover);
							hover = 0;
							msg.clear();
							break 'input;
						}
					},
					
					Key::Left => {
						// if currently in submenu, go back and clear msg
						if menu_path.len() > 0 {
							menu_path.pop();
							hover = 0;
							msg.clear();
							break 'input;
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
							break 'input;
						}
					},
					
					_ => (),
				}
			}
		}
	}
	
	// show cursor
	let presult = write!(stdout, "{}", termion::cursor::Show);
	
	if !presult.is_ok() {
		lgr.log("Could not unhide the cursor");
	}
}