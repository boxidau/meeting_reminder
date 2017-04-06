#!/usr/bin/env python
import subprocess
import datetime
import serial
import time
import sys
import glob
import logging

logging.basicConfig(filename='meeting-reminder.log',level=logging.DEBUG)

SEPARATOR = '-:::- '

serial_regex = '/dev/cu.usbmodem*'
SERIAL_DEVICE = None

fields = ["title", "datetime", "location", "attendees"]

icalcmd = 'icalBuddy -ea -n -nc -eed -po "{}" -eep "notes" -df "%Y-%m-%d" -tf "%H:%M" -nrd -b "{}" eventsToday+2'.format(','.join(fields), SEPARATOR)

def setupSerial():
  global SERIAL_DEVICE
  SERIAL_DEVICE = None 
  devices = glob.glob(serial_regex)
  for device in devices:
    try:
      logging.info('Trying serial device: {}'.format(device))
      ser = serial.Serial(device, 9600, timeout=0.5)
      ser.close()
      logging.info('Device OK')
      SERIAL_DEVICE = device
      return device
    except:
      logging.error('Device error')
      pass

def parseOutput(output):
  events = []
  for data in output.split(SEPARATOR):
    event_lines = data.splitlines()

    event = {
      'title': None,
      'start': None,
      'location': None,
      'attendees': None,
    }

    if len(event_lines) > 1:
      start = event_lines[1].strip().split(' at ')
      start_time = start[1].strip().split(':')
      start_date = start[0].strip().split('-')      
      event['title'] = event_lines[0]
      event['start'] = datetime.datetime(int(start_date[0]), int(start_date[1]), int(start_date[2]), int(start_time[0]), int(start_time[1]))

    if len(event_lines) > 2:
      event['location'] = event_lines[2].strip().lstrip('location: ')

    if len(event_lines) > 3:
      event['attendees'] = event_lines[3].strip().lstrip('attendees: ')

    if event['start']:
      events.append(event)

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
  if not SERIAL_DEVICE:
    setupSerial()

  if not mins:
    mins = 255
  
  logging.info('sending {}'.format(mins))
  try:
    ser = serial.Serial(SERIAL_DEVICE, 9600, timeout=0.5)
    ser.write('{}\n'.format(mins).encode())
    ser.close()
  except serial.serialutil.SerialException:
    logging.error("Serial port error")
    setupSerial()

if __name__ == '__main__':
  while (True):

    ical_output = subprocess.Popen(
      icalcmd,
      shell=True,
      stdout=subprocess.PIPE,
    ).stdout.read().decode('utf8')

    events = parseOutput(ical_output)

    sendData(getNextEventMinutes(events))
    time.sleep(5)
