// adapted from http://allmybrain.com/2008/06/10/timing-cc-code-on-linux/
/**************************************************************************
*   Copyright (C) 2010 by Mark Pictor                                     *
*   mpictor@gmail.com                                                     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
**************************************************************************/
#ifndef NANOTIMER_HH
#define NANOTIMER_HH

#include <time.h>
#include <string>

namespace g2m {

class nanotimer {
  private:
    timespec begin;
  public:
    nanotimer() {}
    void start();
    long getElapsed();
    double getElapsedS();
    static QString humanreadable(double s);
};

}
#endif //NANOTIMER_HH
