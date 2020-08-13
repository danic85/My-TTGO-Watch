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
#ifndef _alarm_MAIN_H
    #define _alarm_MAIN_H

    #define IGNORE PCF8563_NO_ALARM
    #define SUNDAY 1
    #define MONDAY 2
    #define TUESDAY 3
    #define WEDNESDAY 4
    #define THURSDAY 5
    #define FRIDAY 6
    #define SATURDAY 7

    #include <TTGO.h>

    void alarm_main_setup( uint32_t tile_num, TTGOClass *ttgo);

#endif // _alarm_MAIN_H