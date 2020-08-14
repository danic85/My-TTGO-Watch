/****************************************************************************
 *   Aug 3 12:17:11 2020
 *   Copyright  2020  Dirk Brosswick
 *   Email: dirk.brosswick@googlemail.com
 ****************************************************************************/
 
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "config.h"
#include <TTGO.h>

#include "alarm.h"
#include "alarm_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"

lv_obj_t *alarm_main_tile = NULL;
lv_style_t alarm_main_style;

lv_task_t * _alarm_task;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);
LV_FONT_DECLARE(Ubuntu_72px);

static void exit_alarm_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_alarm_setup_event_cb( lv_obj_t * obj, lv_event_t event );
void alarm_task( lv_task_t * task );

bool rtcIrq = false;
int alarmMax = 10;

void alarm_main_setup( uint32_t tile_num, TTGOClass *ttgo  ) {

    /** ALARM SPECIFIC CODE **/
    ttgo->openBL();
    ttgo->motor_begin();

    //attach touch screen interrupt pin
    pinMode(TP_INT, INPUT); 

    // attach RTC interupt
    pinMode(RTC_INT, INPUT_PULLUP);
    attachInterrupt(RTC_INT, [] {
        rtcIrq = 1;
    }, FALLING);

    // disable alarm
    ttgo->rtc->disableAlarm();

    // ttgo->rtc->setAlarm(27, 20, IGNORE, IGNORE); // can only set one currently I believe

    ttgo->rtc->setDateTime(2019, 8, 12, 0, 0, 0);

    ttgo->rtc->setAlarmByMinutes(1); // or setAlarm() https://playground.arduino.cc/Main/RTC-PCF8563/
    //ttgo->rtc->setAlarmByMinutes(1); // For testing

    

    log_i("Setting alarm");

    //void setAlarm(byte min, byte hour, byte day, byte weekday)
    // ttgo->rtc->setAlarm(0, 7, IGNORE, IGNORE); // can only set one currently I believe
    // @todo allow setting of multiple alarms on repeat.
    // ttgo->rtc->setAlarm(0, 7, IGNORE, MONDAY);
    // ttgo->rtc->setAlarm(0, 7, IGNORE, TUESDAY);
    // ttgo->rtc->setAlarm(0, 7, IGNORE, WEDNESDAY);
    // ttgo->rtc->setAlarm(0, 7, IGNORE, THURSDAY);
    // ttgo->rtc->setAlarm(0, 7, IGNORE, FRIDAY);

    ttgo->rtc->enableAlarm();
    /** END ALARM SPECIFIC CODE **/

    alarm_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &alarm_main_style, mainbar_get_style() );

    lv_obj_t * exit_btn = lv_imgbtn_create( alarm_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &alarm_main_style );
    lv_obj_align(exit_btn, alarm_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_alarm_main_event_cb );

    lv_obj_t * setup_btn = lv_imgbtn_create( alarm_main_tile, NULL);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style(setup_btn, LV_IMGBTN_PART_MAIN, &alarm_main_style );
    lv_obj_align(setup_btn, alarm_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    lv_obj_set_event_cb( setup_btn, enter_alarm_setup_event_cb );

    // uncomment the following block of code to remove the "alarm" label in background
    lv_style_set_text_opa( &alarm_main_style, LV_OBJ_PART_MAIN, LV_OPA_70);
    lv_style_set_text_font( &alarm_main_style, LV_STATE_DEFAULT, &Ubuntu_72px);
    lv_obj_t *app_label = lv_label_create( alarm_main_tile, NULL);
    lv_label_set_text( app_label, "alarm");
    lv_obj_reset_style_list( app_label, LV_OBJ_PART_MAIN );
    lv_obj_add_style( app_label, LV_OBJ_PART_MAIN, &alarm_main_style );
    lv_obj_align( app_label, NULL, LV_ALIGN_CENTER, 0, 0);

    // create an task that runs every secound
    _alarm_task = lv_task_create( alarm_task, 1000, LV_TASK_PRIO_MID, NULL );
}

static void enter_alarm_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( alarm_get_app_setup_tile_num(), LV_ANIM_OFF );
                                        break;
    }
}

static void exit_alarm_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

void alarm_task( lv_task_t * task ) {
    // log_i("Checking alarm");
    // if an alarm has triggered
    if (rtcIrq) {
        log_i("Sounding alarm");
        
        detachInterrupt(RTC_INT);
        TTGOClass *ttgo = TTGOClass::getWatch();
        ttgo->rtc->resetAlarm();

        // Display message
        ttgo->tft->fillScreen(TFT_WHITE);
        ttgo->tft->setTextColor(TFT_BLACK, TFT_WHITE);
        ttgo->tft->drawString("Alarm", 60, 118, 4);

        // Vibrate until screen is touched
        if (ttgo->touch->touched() == false && alarmMax > 0) {
            ttgo->motor->onec();
            delay(2000);
            alarmMax--;
        }
        else {
            rtcIrq = 0;
            alarmMax = 10;
        }

        //@todo Re-set alarm for next time
    }
}