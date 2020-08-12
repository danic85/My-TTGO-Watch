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
#include "alarm_setup.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"

uint32_t alarm_main_tile_num;
uint32_t alarm_setup_tile_num;

// app icon container
lv_obj_t *alarm_icon_cont = NULL;
lv_obj_t *alarm_icon = NULL;
lv_obj_t *alarm_icon_info = NULL;

// widget icon container
lv_obj_t *alarm_widget_cont = NULL;
lv_obj_t *alarm_widget_icon = NULL;
lv_obj_t *alarm_widget_icon_info = NULL;
lv_obj_t *alarm_widget_label = NULL;

// declare you images or fonts you need
LV_IMG_DECLARE(alarm_64px);
LV_IMG_DECLARE(alarm_48px);
LV_IMG_DECLARE(info_1_16px);
// declare callback functions
static void enter_alarm_event_cb( lv_obj_t * obj, lv_event_t event );

// setup routine for example app
void alarm_setup(TTGOClass *ttgo) {
    // register 2 vertical tiles and get the first tile number and save it for later use
    alarm_main_tile_num = mainbar_add_app_tile( 1, 2 );
    alarm_setup_tile_num = alarm_main_tile_num + 1;

    // create an app icon, label it and get the lv_obj_t icon container
    alarm_icon_cont = app_tile_register_app( "myapp");
    // set your own icon and register her callback to activate by an click
    // remember, an app icon must have an size of 64x64 pixel with an alpha channel
    // use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
    // the resulting c-file can put in /app/examples/images/
    alarm_icon = lv_imgbtn_create( alarm_icon_cont, NULL );
    lv_imgbtn_set_src( alarm_icon, LV_BTN_STATE_RELEASED, &alarm_64px);
    lv_imgbtn_set_src( alarm_icon, LV_BTN_STATE_PRESSED, &alarm_64px);
    lv_imgbtn_set_src( alarm_icon, LV_BTN_STATE_CHECKED_RELEASED, &alarm_64px);
    lv_imgbtn_set_src( alarm_icon, LV_BTN_STATE_CHECKED_PRESSED, &alarm_64px);
    lv_obj_reset_style_list( alarm_icon, LV_OBJ_PART_MAIN );
    lv_obj_align( alarm_icon , alarm_icon_cont, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_set_event_cb( alarm_icon, enter_alarm_event_cb );

    // set an small info indicator at your app icon to inform the user about the state or news
    alarm_icon_info = lv_img_create( alarm_icon_cont, NULL );
    lv_img_set_src( alarm_icon_info, &info_1_16px );
    lv_obj_align( alarm_icon_info, alarm_icon_cont, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );
    lv_obj_set_hidden( alarm_icon_info, false );

    // init main and setup tile, see alarm_main.cpp and alarm_setup.cpp
    alarm_main_setup( alarm_main_tile_num, ttgo );
    alarm_setup_setup( alarm_setup_tile_num);

#ifdef EXAMPLE_WIDGET
    // get an widget container from main_tile
    // remember, an widget icon must have an size of 64x64 pixel
    // total size of the container is 64x80 pixel, the bottom 16 pixel is for your label
    alarm_widget_cont = main_tile_register_widget();
    alarm_widget_icon = lv_imgbtn_create( alarm_widget_cont, NULL );
    lv_imgbtn_set_src( alarm_widget_icon, LV_BTN_STATE_RELEASED, &alarm_48px);
    lv_imgbtn_set_src( alarm_widget_icon, LV_BTN_STATE_PRESSED, &alarm_48px);
    lv_imgbtn_set_src( alarm_widget_icon, LV_BTN_STATE_CHECKED_RELEASED, &alarm_48px);
    lv_imgbtn_set_src( alarm_widget_icon, LV_BTN_STATE_CHECKED_PRESSED, &alarm_48px);
    lv_obj_reset_style_list( alarm_widget_icon, LV_OBJ_PART_MAIN );
    lv_obj_align( alarm_widget_icon , alarm_widget_cont, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_set_event_cb( alarm_widget_icon, enter_alarm_event_cb );

    // set an small info icon at your widget icon to inform the user about the state or news
    alarm_widget_icon_info = lv_img_create( alarm_widget_cont, NULL );
    lv_img_set_src( alarm_widget_icon_info, &info_1_16px );
    lv_obj_align( alarm_widget_icon_info, alarm_widget_cont, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );
    lv_obj_set_hidden( alarm_widget_icon_info, false );

    // label your widget
    alarm_widget_label = lv_label_create( alarm_widget_cont , NULL);
    lv_label_set_text( alarm_widget_label, "myapp");
    lv_obj_reset_style_list( alarm_widget_label, LV_OBJ_PART_MAIN );
    lv_obj_align( alarm_widget_label, alarm_widget_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
#endif // EXAMPLE_WIDGET
}

uint32_t alarm_get_app_main_tile_num( void ) {
    return( alarm_main_tile_num );
}

uint32_t alarm_get_app_setup_tile_num( void ) {
    return( alarm_setup_tile_num );
}

/*
 *
 */
static void enter_alarm_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       alarm_hide_app_icon_info( true );
                                        #ifdef EXAMPLE_WIDGET
                                            alarm_hide_widget_icon_info( true );
                                        #endif
                                        mainbar_jump_to_tilenumber( alarm_main_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}

/*
 *
 */
void alarm_hide_app_icon_info( bool show ) {
    if ( alarm_icon_info == NULL )
        return;
    
    lv_obj_set_hidden( alarm_icon_info, show );
    lv_obj_invalidate( lv_scr_act() );
}

/*
 *
 */
void alarm_hide_widget_icon_info( bool show ) {
    if ( alarm_widget_icon_info == NULL )
        return;

    lv_obj_set_hidden( alarm_widget_icon_info, show );
    lv_obj_invalidate( lv_scr_act() );
}