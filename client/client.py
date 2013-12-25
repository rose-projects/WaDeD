#  WaDeD - Short messages mesh network
#
#  Copyright (C) 2013 WaDeD-ROSE <waded-rose@googlegroups.com>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#!/usr/bin/python3
"""
A client to interract with a WaDeD. Used to send and receive messages.

Due to USB problem on the computer side, lauch this only after the WaDeD
was connected.
"""

from connexion import Connexion_thread
import curses, curses.textpad

class Display:
    def __init__(self):
        self._messages = []

    def add(self, message):
        self._messages.insert(0, message)
        self.print_messages()

    def curses(self, screen):
        self._screen = screen
        line = curses.newwin(1, 0, 0, 0)
        self._body = curses.newwin(0, 0, 1, 0)

        textbox = curses.textpad.Textbox(line)

        while 1:
            txt = textbox.edit()
            line.clear()
            usb.write(txt + "\n")

    def print_messages(self):
        x, _ = self._screen.getmaxyx()
        self._messages = self._messages[:x - 2]
        self._body.clear()
        for i, text in enumerate(self._messages):
            self._screen.addstr(i + 1, 0, text)
        self._screen.refresh()

if __name__ == '__main__':
    print("WaDeD Copyright (C) 2013 WaDeD-ROSE\nThis program comes with AB" \
        "SOLUTELY NO WARRANTY. See README.md\nThis is free software, and y" \
        "ou are welcome to redistribute it\nunder certain conditions. Chec" \
        "k the file LICENSE provided with\nthis program. If not see <http:" \
        "//www.gnu.org/licenses/>.\n")
    d = Display()
    t = Connexion_thread('/dev/ttyACM0', d.add)
    usb = t.connexion
    t.start()
    curses.initscr()
    curses.curs_set(0)
    curses.wrapper(d.curses)
