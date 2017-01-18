#!/usr/bin/env python
import subprocess
import datetime
import serial
import time

SEPARATOR = '-:::- '

SERIAL_DEVICE = '/dev/cu.usbmodemfd111'

icalcmd = 'icalBuddy -b "{}" -tf "%H:%M" -n -eed eventsToday'.format(SEPARATOR)

def parseOutput(output):
  now = datetime.datetime.now()
  events = []
  for event in output.split(SEPARATOR):
    
    event_lines = event.splitlines()
    if len(event_lines) == 1:
      print 'Ignoring all day event: ' + event_lines[0]

    if len(event_lines) == 2:
      start_time = event_lines[1].strip().split(':')
      events.append({
        'name': event_lines[0],
        'start': datetime.datetime(now.year, now.month, now.day, int(start_time[0]), int(start_time[1]))
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
  if mins:
    print 'sending {}'.format(mins)
    ser = serial.Serial(SERIAL_DEVICE, 9600, timeout=0.5)
    ser.write('{}\n'.format(mins))
    ser.close()

if __name__ == '__main__':
  while (True):

    ical_output = subprocess.Popen(
      icalcmd,
      shell=True,
      stdout=subprocess.PIPE,
    ).stdout.read()

    events = parseOutput(ical_output)
    print repr(events)

    sendData(getNextEventMinutes(events))
    time.sleep(5)
