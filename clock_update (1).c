#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "clock.h"
// Max Valek (valek016)

//Function to determine and set values for each field of tod_t
//based on the time_of_day_sec
int set_tod_from_secs(int time_of_day_sec, tod_t *tod){
  int temp = time_of_day_sec;
  if(temp < 0 || temp > 86400){ //time_of_day_sec is invalid
    return 1;
  }
  tod->ispm = 0;
  while(temp >= 43200){ //while total time is greater than or equal to twelve hours
    if(tod->ispm == 1){ //This set of conditionals updates the ispm variable
      tod->ispm = 0;
    }
    else{
      tod->ispm = 1;
    }
    temp -= 43200; //decrement temp by 12 hours
  }
  int hour, minute, second;
  hour = temp/3600;
  if(hour == 0){ //12am
    tod->hours = 12;
  }
  else{
    tod->hours = hour;
  }
  minute = (temp - (3600*hour))/60;
  second = (temp - (3600*hour)) - (minute*60);
  tod->minutes = minute;
  tod->seconds = second;
  return 0;
}
//Function used to update the display variable using an array of bit masks
//Returns 1 if any fields of tod are negative or too large
//Displays a one in the tens place for hours if the number of hours
//is greater than 9.
int set_display_bits_from_tod(tod_t tod, int *display){
  if(tod.hours > 12 || tod.hours < 0 || tod.minutes > 59 ||
    tod.minutes < 0 || tod.seconds > 59|| tod.seconds < 0){ //if any fields of tod are negative or too large
    return 1;
  }
  int arr[10] = {0b0111111, 0b0000110, 0b1011011, 0b1001111, 0b1100110,
                 0b1101101, 0b1111101, 0b0000111, 0b1111111, 0b1101111}; //array of masks
  int min_ones = tod.minutes % 10;
  int min_tens = (tod.minutes/10) % 10;
  int hour_ones = tod.hours % 10;
  int hour_tens = (tod.hours/10) % 10;
  *display = 0; //initializing display to all zeros
  //putting all mask values into display variable
  *display |= (arr[min_ones]);
  *display |= (arr[min_tens] << 7);
  *display |= (arr[hour_ones] << 14);
  //if there is a one, display it. Otherwise nothing is added for the tens place of hours
  if(hour_tens >= 1){
    *display |= (arr[1] << 21);
  }
  //set 29th bit to one if time is pm
  if(tod.ispm == 1){
    *display |= (1 << 29);
  }
  //set 28th bit to one if time is am
  else{
    *display |= (1 << 28);
}
  return 0;
}
// Examines the TIME_OF_DAY_SEC global variable to determine hour,
// minute, and am/pm.  Sets the global variable CLOCK_DISPLAY_PORT bits
// to show the proper time.  If TIME_OF_DAY_SEC appears to be in error
// (to large/small) makes no change to CLOCK_DISPLAY_PORT and returns 1
// to indicate an error. Otherwise returns 0 to indicate success.
// Makes use of the set_tod_from_secs() and
// set_display_bits_from_tod() functions.
int clock_update(){
  tod_t tod = {0,0,0,0}; //initializing tod_t variable to be filled in set_tod_from_secs
  int suc = set_tod_from_secs(TIME_OF_DAY_SEC, &tod);
  if(suc == 1){ //if the time of day is invalid
    return 1;
  }
  set_display_bits_from_tod(tod, &CLOCK_DISPLAY_PORT); //update CLOCK_DISPLAY_PORT with the newly filled tod
  return 0;
}
