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

use crate::interface::{Button, ShellCmd};

pub fn new_sys_menu() -> Button {
	return Button::from(
		"Sysmenu",
		ShellCmd::new(),
		"",
		vec![
			Button::from(
				"About HouseDE",
				ShellCmd::from("", ""),
				format!("output = \"{} {} is licensed under the {}.\\nYou should have received a copy of the license along with this program.\\nIf not see <https://www.gnu.org/licenses/>\\n\\nThe source code of this program is available at:\\n{}\"", env!("CARGO_PKG_NAME"), env!("CARGO_PKG_VERSION"), env!("CARGO_PKG_LICENSE"), env!("CARGO_PKG_REPOSITORY")).as_str(),
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
	);
}

pub fn new_recovery_menu(user_cfg_path: &str) -> Button {
	return Button::from(
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
						ShellCmd::from("sudo", format!("vim {}", user_cfg_path).as_str()),
						"",
						Vec::<Button>::new(),
					),
					
					Button::from(
						"(sudo) Edit user config via Nano",
						ShellCmd::from("sudo", format!("nano {}", user_cfg_path).as_str()),
						"",
						Vec::<Button>::new(),
					),
					
					Button::from(
						"(sudo) Edit user config via Vi",
						ShellCmd::from("sudo", format!("vi {}", user_cfg_path).as_str()),
						"",
						Vec::<Button>::new(),
					),
				],
			),
		],
	);
}