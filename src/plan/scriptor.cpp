/*
 * Scriptor body
 * Copyright (C) 2007 Petr Kubanek <petr@kubanek.net>
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

#include "../utils/rts2device.h"
#include "rts2execcli.h"
#include "rts2devcliphot.h"
#include "rts2targetscr.h"

#define OPT_EXPAND_PATH       OPT_LOCAL + 101
#define OPT_GEN               OPT_LOCAL + 102

class Rts2Scriptor:public Rts2Device, public Rts2ScriptInterface
{
	private:
		Rts2ValueInteger *scriptCount;
		Rts2ValueString *expandPath;
		Rts2ValueSelection *scriptGen;

		Rts2TargetScr *currentTarget;
	protected:
		virtual int init ();
		virtual int processOption (int in_opt);
		virtual int willConnect (Rts2Address * in_addr);
		virtual Rts2DevClient *createOtherType (Rts2Conn *conn, int other_device_type);
		virtual void deviceReady (Rts2Conn * conn);
		virtual int setValue (Rts2Value * old_value, Rts2Value * new_value);
	public:
		Rts2Scriptor (int argc, char **argv);
		virtual ~Rts2Scriptor (void);

		virtual void postEvent (Rts2Event * event);

		virtual int findScript (std::string in_deviceName, std::string & buf);
		virtual int getPosition (struct ln_equ_posn *posn, double JD);
};

Rts2Scriptor::Rts2Scriptor (int in_argc, char **in_argv)
:Rts2Device (in_argc, in_argv, DEVICE_TYPE_SCRIPTOR, "SCRIPTOR"), Rts2ScriptInterface ()
{
	createValue (scriptCount, "script_count", "number of scripts execuced", false);
	createValue (expandPath, "expand_path", "expand path for new images", false);
	expandPath->setValueString ("%f");

	createValue (scriptGen, "script_generator", "command which gets state and generates next script", false);
	scriptGen->addSelVal ("/etc/rts2/scriptor");

	addOption (OPT_EXPAND_PATH, "expand-path", 1, "path used for filename expansion");
	addOption (OPT_GEN, "script-gen", 1, "script generator");

	currentTarget = NULL;
}


Rts2Scriptor::~Rts2Scriptor (void)
{
}


int
Rts2Scriptor::init ()
{
	int ret;
	ret = Rts2Device::init ();
	if (ret)
		return ret;

	getCentraldConn ()->queCommand (new Rts2Command (this, "priority 20"));

	currentTarget = new Rts2TargetScr (this);
	currentTarget->moveEnded ();

	return 0;
}


int
Rts2Scriptor::processOption (int in_opt)
{
	switch (in_opt)
	{
		case OPT_EXPAND_PATH:
			expandPath->setValueString (optarg);
			break;
		case OPT_GEN:
			scriptGen->addSelVal (optarg);
			break;
		default:
			return Rts2Device::processOption (in_opt);
	}
	return 0;
}


int
Rts2Scriptor::willConnect (Rts2Address * in_addr)
{
	if (in_addr->getType () < getDeviceType ())
		return 1;
	return 0;
}


Rts2DevClient *
Rts2Scriptor::createOtherType (Rts2Conn *conn, int other_device_type)
{
	switch (other_device_type)
	{
		case DEVICE_TYPE_CCD:
			return new Rts2DevClientCameraExec (conn, expandPath);
		default:
			return Rts2Device::createOtherType (conn, other_device_type);
	}
}


void
Rts2Scriptor::deviceReady (Rts2Conn * conn)
{
	std::cout << "conn " << conn->getName () << " prio " << conn->havePriority () << std::endl;
	if (conn->havePriority ())
	{
		conn->postEvent (new Rts2Event (EVENT_SET_TARGET, (void *) currentTarget));
		conn->postEvent (new Rts2Event (EVENT_OBSERVE));
	}
}


int
Rts2Scriptor::setValue (Rts2Value * old_value, Rts2Value * new_value)
{
	if (old_value == expandPath)
		return 0;
	return Rts2Device::setValue (old_value, new_value);
}


void
Rts2Scriptor::postEvent (Rts2Event * event)
{
	switch (event->getType ())
	{
		case EVENT_SCRIPT_ENDED:
			postEvent (new Rts2Event (EVENT_SET_TARGET, (void *) currentTarget));
			postEvent (new Rts2Event (EVENT_OBSERVE));
			break;
	}
	Rts2Device::postEvent (event);
}


int
Rts2Scriptor::findScript (std::string in_deviceName, std::string & buf)
{
	std::string cmd = scriptGen->getSelName ();
	FILE *gen = popen (cmd.c_str (), "r");

	char *filebuf = NULL;
	size_t len;
	ssize_t ret = getline (&filebuf, &len, gen);
	if (ret == -1)
		return -1;
	buf = std::string (filebuf);
	pclose (gen);
	free (filebuf);
	return 0;
}


int
Rts2Scriptor::getPosition (struct ln_equ_posn *posn, double JD)
{
	posn->ra = 20;
	posn->dec = 20;
	return -1;
}


int
main (int argc, char **argv)
{
	Rts2Scriptor scriptor = Rts2Scriptor (argc, argv);
	return scriptor.run ();
}
