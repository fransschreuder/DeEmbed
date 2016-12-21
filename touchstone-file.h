/*
 * DeEmbed
 * Copyright (C) Frans Schreuder 2016 <info@schreuderelectronics.com>
 *
software is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _TOUCHSTONE_FILE_H_
#define _TOUCHSTONE_FILE_H_

#include <vector>
#include "typedefs.h"
using namespace std;

typedef enum {GHz, MHz, KHz, Hz}FUnit;
typedef enum {DB, MA, RI}SFormat;

class TouchstoneFile
{
public:
    TouchstoneFile();
    ~TouchstoneFile();
    ///! can load s1p and s2p files
    spar_t Load2P(const char* filename);

    ///! can load s3p files
    spar_t Load3P(const char* filename);

    ///! can load s4p files
    spar_t Load4P(const char* filename);

    ///! Saves s1p, s2p, s3p and s4p touchstone files.
    bool Save(spar_t& spar, const char* filename);

};

#endif // _TOUCHSTONE_FILE_H_
