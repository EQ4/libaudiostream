/*

  Copyright � Grame 2001-2007

  This library is free software; you can redistribute it and modify it under 
  the terms of the GNU Library General Public License as published by the 
  Free Software Foundation version 2 of the License, or any later version.

  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public 
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Grame Research Laboratory, 9, rue du Garet 69001 Lyon - France
  research@grame.fr

*/

#include "lflifo.h"

void lfinit(lifo* lf)
{
	lf->top = 0;
	lf->count.value = 0;
}

void lfpush (lifo * lf, lifocell * cl)
{
	lifocell * volatile top;
	do {
		top = lf->top;
		cl->link = top;
	} while (!CAS (&lf->top, top, cl));
	msAtomicInc (&lf->count);
}

/* 
   on ppc architecture, the ABA problem is catched
   by the memory reservation system
*/
lifocell* lfpop (lifo * lf)
{
	lifocell * volatile top;
	do {
		LWARX(&lf->top);
		top = lf->top;
		if (!top) return 0;
	} while (!STWCX (&lf->top, top, top->link));
	msAtomicDec (&lf->count);
	return top;
}

