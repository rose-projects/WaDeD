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
Handle an USB connexion with a WaDeD.
"""

from serial      import Serial, SerialException
from threading   import Thread
from time        import sleep
from unicodedata import normalize

class Connexion:
    """
    Manage the USB connexion.
    """

    def __init__(self, tty = "/dev/ttyACM0"):
        """
        Open a serial connexion on specified tty, with a 1152000
        baudrate.
        """
        try:
            self.serial = Serial(tty, baudrate = 115200)
        except SerialException:
            print("Can't open", tty)
            exit(1)

    def read(self):
        """ Read a line. """
        try:
            return self.serial.readline().decode("ascii", "ignore")
        except OSError:
            sleep(1)
            return self.read()

    def write(self, msg):
        """ Send a string. """
        self.serial.write(normalize("NFD", msg).encode("ascii", "ignore"))

class Connexion_thread (Thread):
    """
    A tread reading the data through USB. Every time it gets data, it call
    a callback function.
    """

    def __init__(self, tty, callback):
        """
        The constructor, where tty is the input terminal, and callback the
        callback function.
        """

        Thread.__init__(self)

        self.connexion = Connexion(tty)
        self._callback = callback

    def run(self):
        """ Lauch the thread. """

        while 1:
            self._callback(self.connexion.read())
