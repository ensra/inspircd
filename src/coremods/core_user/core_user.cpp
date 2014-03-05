/*
 * InspIRCd -- Internet Relay Chat Daemon
 *
 *   Copyright (C) 2014 Attila Molnar <attilamolnar@hush.com>
 *
 * This file is part of InspIRCd.  InspIRCd is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "inspircd.h"
#include "core_user.h"

class CommandMode : public Command
{
 public:
	/** Constructor for mode.
	 */
	CommandMode(Module* parent)
		: Command(parent, "MODE", 1)
	{
		syntax = "<target> <modes> {<mode-parameters>}";
	}

	/** Handle command.
	 * @param parameters The parameters to the command
	 * @param user The user issuing the command
	 * @return A value from CmdResult to indicate command success or failure.
	 */
	CmdResult Handle(const std::vector<std::string>& parameters, User* user)
	{
		ServerInstance->Modes->Process(parameters, user, (IS_LOCAL(user) ? ModeParser::MODE_NONE : ModeParser::MODE_LOCALONLY));
		return CMD_SUCCESS;
	}

	RouteDescriptor GetRouting(User* user, const std::vector<std::string>& parameters)
	{
		return (IS_LOCAL(user) ? ROUTE_LOCALONLY : ROUTE_BROADCAST);
	}
};

/** Handle /PASS.
 */
class CommandPass : public SplitCommand
{
 public:
	/** Constructor for pass.
	 */
	CommandPass(Module* parent)
		: SplitCommand(parent, "PASS", 1, 1)
	{
		works_before_reg = true;
		Penalty = 0;
		syntax = "<password>";
	}

	/** Handle command.
	 * @param parameters The parameters to the command
	 * @param user The user issuing the command
	 * @return A value from CmdResult to indicate command success or failure.
	 */
	CmdResult HandleLocal(const std::vector<std::string>& parameters, LocalUser *user)
	{
		// Check to make sure they haven't registered -- Fix by FCS
		if (user->registered == REG_ALL)
		{
			user->WriteNumeric(ERR_ALREADYREGISTERED, ":You may not reregister");
			return CMD_FAILURE;
		}
		user->password = parameters[0];

		return CMD_SUCCESS;
	}
};

/** Handle /PING.
 */
class CommandPing : public Command
{
 public:
	/** Constructor for ping.
	 */
	CommandPing(Module* parent)
		: Command(parent, "PING", 1, 2)
	{
		Penalty = 0;
		syntax = "<servername> [:<servername>]";
	}

	/** Handle command.
	 * @param parameters The parameters to the command
	 * @param user The user issuing the command
	 * @return A value from CmdResult to indicate command success or failure.
	 */
	CmdResult Handle(const std::vector<std::string>& parameters, User* user)
	{
		user->WriteServ("PONG %s :%s", ServerInstance->Config->ServerName.c_str(), parameters[0].c_str());
		return CMD_SUCCESS;
	}
};

/** Handle /PONG.
 */
class CommandPong : public Command
{
 public:
	/** Constructor for pong.
	 */
	CommandPong(Module* parent)
		: Command(parent, "PONG", 0, 1)
	{
		Penalty = 0;
		syntax = "<ping-text>";
	}

	/** Handle command.
	 * @param parameters The parameters to the command
	 * @param user The user issuing the command
	 * @return A value from CmdResult to indicate command success or failure.
	 */
	CmdResult Handle(const std::vector<std::string>& parameters, User* user)
	{
		// set the user as alive so they survive to next ping
		if (IS_LOCAL(user))
			IS_LOCAL(user)->lastping = 1;
		return CMD_SUCCESS;
	}
};

class CoreModUser : public Module
{
	CommandAway cmdaway;
	CommandMode cmdmode;
	CommandNick cmdnick;
	CommandPart cmdpart;
	CommandPass cmdpass;
	CommandPing cmdping;
	CommandPong cmdpong;
	CommandQuit cmdquit;
	CommandUser cmduser;

 public:
	CoreModUser()
		: cmdaway(this), cmdmode(this), cmdnick(this), cmdpart(this), cmdpass(this), cmdping(this)
		, cmdpong(this), cmdquit(this), cmduser(this)
	{
	}

	Version GetVersion() CXX11_OVERRIDE
	{
		return Version("Provides the AWAY, MODE, NICK, PART, PASS, PING, PONG, QUIT and USER commands", VF_VENDOR|VF_CORE);
	}
};

MODULE_INIT(CoreModUser)
