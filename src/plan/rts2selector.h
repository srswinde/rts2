/*
 * Selector body.
 * Copyright (C) 2003-2008 Petr Kubanek <petr@kubanek.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __RTS2_SELECTOR__
#define __RTS2_SELECTOR__

#include "../utilsdb/rts2appdb.h"
#include "../utilsdb/target.h"

/**
 * Select next target. Traverse list of targets which are enabled and select
 * target with biggest priority.
 *
 * @author Petr Kubanek <petr@kubanek.net>
 */
class Rts2Selector
{
	private:
		std::list < Target * >possibleTargets;
		void considerTarget (int consider_tar_id, double JD);
		std::vector <char> nightDisabledTypes;
		void checkTargetObservability ();
		void checkTargetBonus ();
		void findNewTargets ();
		int selectFlats ();
		int selectDarks ();
		struct ln_lnlat_posn *observer;
		double flat_sun_min;
		double flat_sun_max;

		/**
		 * Checks if type is among types disabled for night selection.
		 *
		 * @param type_id   Type id which will be checked for incursion in nightDisabledTypes.
		 *
		 * @return True if type is among disabled types.
		 */
		bool isInNightDisabledTypes (char target_type)
		{
			return (std::find (nightDisabledTypes.begin (), nightDisabledTypes.end (), target_type) != nightDisabledTypes.end ());
		}

	public:
		Rts2Selector (struct ln_lnlat_posn *in_observer);
		virtual ~ Rts2Selector (void);
								 // return next observation..
		int selectNext (int masterState);
		int selectNextNight (int in_bonusLimit = 0);

		double getFlatSunMin ()
		{
			return flat_sun_min;
		}
		double getFlatSunMax ()
		{
			return flat_sun_max;
		}

		void setFlatSunMin (double in_m)
		{
			flat_sun_min = in_m;
		}
		void setFlatSunMax (double in_m)
		{
			flat_sun_max = in_m;
		}

		/**
		 * Set types of targets which are not permited for selection
		 * during night.
		 *
		 * @param disabledTypes  List of types (separated by space) which are disabled.
		 *
		 * @return -1 if list is incorrect, otherwise 0.
		 */
		int setNightDisabledTypes (const char *types);
};
#endif							 /* !__RTS2_SELECTOR__ */
