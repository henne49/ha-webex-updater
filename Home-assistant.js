//
// Copyright (c) 2020 Cisco Systems
// Licensed under the MIT License 
//

/*
 * Turns on a binary Sensor when a call is in progress, 
 * and keep off when out of call.
 * 
 * pre-req:
 *   - configure the macro with your Home-Assistant info and you're good to go.
 *   - configure your device for HttpClient
 *
 */

const xapi = require('xapi');

// ACTION: Update for your Home Assistant deployment
const HA = '172.16.3.2:8123';
const HA_LONG_LIVED_ACCESS_TOKEN = 'SOME-TOKEN-HERE';
const BIN_SENSOR_NAME = 'cisco_deskpro';

console.info(`Home-Assistant with ip: ${HA}, binary sensor: ${BIN_SENSOR_NAME}`);
const ha_url = `http://${HA}/api/states/binary_sensor.${BIN_SENSOR_NAME}`;

const COLOR_RED = { state: "on"};
const COLOR_GREEN = { state: "off"};

function changeColor(color) {
   
   // Post message
   xapi.command('HttpClient POST', {
         Header: [`Authorization: Bearer ${HA_LONG_LIVED_ACCESS_TOKEN}`],
         Url: ha_url,
      },
      JSON.stringify(color))
      .catch((err) => {
         console.error('could not contact home-assistant');
         console.error(err);
      });
}

//
// Macro
//

xapi.on('ready', init);

function init() {
   // Green at launch
   changeColor(COLOR_GREEN);
         
   // Listen to active calls and update color accordingly
   xapi.status.on("SystemUnit State NumberOfActiveCalls", (activeCalls) => {
      console.debug(`NumberOfActiveCalls is: ${activeCalls}`)
      
      // Turn red if call in progress
      if (activeCalls > 0) {
         console.info('call in progress');
         changeColor(COLOR_RED);
         return;
      }
      
      // Turn green if out of call
      console.log('no active call');
      changeColor(COLOR_GREEN);
   });
   
   
}