#!/usr/bin/env python
import subprocess
import datetime
import serial
import time

SEPARATOR = b'-:::- '

SERIAL_DEVICE = '/dev/cu.usbmodem1411'

icalcmd = 'icalBuddy -ea -n -nc -eed -po "title,datetime,location,attendees,notes" -eep "notes,attendees" -df "%Y-%m-%d" -tf "%H:%M" -nrd -b "{}" eventsToday+2'.format(SEPARATOR)

def parseOutput(output):
  events = []
  for event in output.split(SEPARATOR):
    
    event_lines = event.splitlines()

    if len(event_lines) > 1:
      start = event_lines[1].strip().split(b' at ')
      start_time = start[1].strip().split(b':')
      start_date = start[0].strip().split(b'-')
      
      events.append({
        'title': event_lines[0],
        'start': datetime.datetime(int(start_date[0]), int(start_date[1]), int(start_date[2]), int(start_time[0]), int(start_time[1])),
        'location': event_lines[2]
      })
  return events

def getNextEventMinutes(events):
  nextEvent = None
  now = datetime.datetime.now()
  for event in events:
    time_until = event['start'] - now
    mins = int((time_until.total_seconds()+59)/60)
    if mins < 1:
      continue
    if nextEvent == None or mins < nextEvent:
      nextEvent = mins
  return nextEvent

def sendData(mins):
  if not mins:
    mins = 255
  
  print('sending {}'.format(mins).encode())
  ser = serial.Serial(SERIAL_DEVICE, 9600, timeout=0.5)
  ser.write('{}\n'.format(mins).encode())
  ser.close()

if __name__ == '__main__':
  while (True):

    ical_output = subprocess.Popen(
      icalcmd,
      shell=True,
      stdout=subprocess.PIPE,
    ).stdout.read()

    events = parseOutput(ical_output)

    sendData(getNextEventMinutes(events))
    time.sleep(5)
