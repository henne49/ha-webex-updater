#!/usr/bin/python
# 2020-04-01 Matthew Fugel
# https://github.com/matthewf01/Webex-Teams-Status-Box
# wiring diagram for LED
# https://www.instructables.com/id/Raspberry-Pi-3-RGB-LED-With-Using-PWM/

import os
import time
from webexteamssdk import WebexTeamsAPI


#if not setting personId as an environment varaible,
#then replace the next line with api=WebexTeamsAPI(access_token=putyourtokenhere)
api=WebexTeamsAPI(access_token='MGJmNTM3YjEtZTU0Ny00OGFmLWEwNGMtMjNiMzU2MTU4YmJjYWNiN2FhMjctOGQw_PF84_1eb65fdf-9643-417f-9974-ad72cae0e10f')

#helpful stuff you can run if using your personal access token temporarily to test:
person= api.people.me()
#print (person.status,person.displayName)

#pull the personId from environment variable
#mywebexid=os.environ.get(person)
mywebexid=person.id
api.people.get(personId=mywebexid).status

# Status codes include: active,inactive,DoNotDisturb,meeting,presenting,call

try:
	while True:
		status = api.people.get(personId=mywebexid).status
		print (status)
		if status == "active":
			print ("he's active! GREEN")
			time.sleep (10)
		elif status == "call":
			print ("he's on a call! ORANGE")
			time.sleep (10)
		elif status == "inactive":
			print ("inactive - BLUE")
			time.sleep (180)
		elif status == "DoNotDisturb":
			print ("he's on Do Nor Disturb! RED")
			time.sleep (10)
		elif status == "presenting":
			print ("he's presenting! RED")
			time.sleep (10)
		elif status == "OutOfOffice":
			print ("Out of Office - PURPLE")
			time.sleep (360)
		else:
			print ("don't bug him! RED")
			time.sleep (10)
except KeyboardInterrupt:
    print ("Goodbye")