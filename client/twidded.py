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
from twitter   import *
from os        import path
from connexion import Connexion_thread
from time      import sleep

print("WaDeD Copyright (C) 2013 WaDeD-ROSE\nThis program comes with AB" \
    "SOLUTELY NO WARRANTY. See README.md\nThis is free software, and y" \
    "ou are welcome to redistribute it\nunder certain conditions. Chec" \
    "k the file LICENSE provided with\nthis program. If not see <http:" \
    "//www.gnu.org/licenses/>.\n")

MY_TWITTER_CREDS = path.expanduser('~/.auth')
CONSUMER_KEY = #TODO
CONSUMER_SECRET = #TODO
if not path.exists(MY_TWITTER_CREDS):
    oauth_dance("TwiDDeD", CONSUMER_KEY, CONSUMER_SECRET,
                MY_TWITTER_CREDS)

oauth_token, oauth_secret = read_token_file(MY_TWITTER_CREDS)

twitter = Twitter(auth=OAuth(
    oauth_token, oauth_secret, CONSUMER_KEY, CONSUMER_SECRET))

def handle(message):
    """ Function called each time a message come through USB. """
    if message.split()[0] == "rec_txt":
        tweet = " ".join(message.split()[2:])
        print("Tweeting (from {}): {}".format(message.split()[1], tweet))
        twitter.statuses.update(status = tweet)

if __name__ == "__main__":
    # TODO work also on non-initialized WaDeD
    t = Connexion_thread('/dev/ttyACM0', handle)
    t.start()

    last = twitter.search.tweets(q = "#rose2013", result_type = "recent",
            count = 1)["statuses"][0]["id"]

    while 1:
        try:
            messages = twitter.search.tweets(q = "#rose2013", since_id = last,
                    result_type = "recent")["statuses"]
        except:
            print("Can not connect to Twitter...")
            messages = []
        for message in messages:
            if message["id"] == messages:
                break
            t.connexion.write("send_txt 24616 " + message["text"] + "\n")
            print("Add \"{}\" to the network.".format(message["text"]))
        if messages:
            last = messages[0]["id"]
        sleep(6)
