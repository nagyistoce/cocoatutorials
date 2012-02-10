//
//  IO.h
//  This file is part of ix4
// 
//  ix4 is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  ix is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with Favorites.  If not, see <http://www.gnu.org/licenses/>.
//
//  This file is original work by Robin Mills, San Jose, CA 95112, USA
//  Created 2012 http://clanmills.com robin@clanmills.com
//

#ifndef ix4_IO_h
#define ix4_IO_h

void setVerbose(boolean_t verbose);
boolean_t getVerbose();

// report => stdout error => stderr warn => stderr IF verbose
void warn    (const char* msg);
void errors  (const char* msg);
void report  (const char* msg);

void warns   (NSString* msg);
void errorns (NSString* msg);
void reportns(NSString* msg);

#endif
