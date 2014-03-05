/*
 * InspIRCd -- Internet Relay Chat Daemon
 *
 *   Copyright (C) 2009 Daniel De Graaf <danieldg@inspircd.org>
 *   Copyright (C) 2007-2008 Robin Burchell <robin+git@viroteck.net>
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

CommandPart::CommandPart(Module* parent)
	: Command(parent, "PART", 1, 2)
{
	Penalty = 5;
	syntax = "<channel>{,<channel>} [<reason>]";
}

CmdResult CommandPart::Handle (const std::vector<std::string>& parameters, User *user)
{
	std::string reason;

	if (IS_LOCAL(user))
	{
		if (!ServerInstance->Config->FixedPart.empty())
			reason = ServerInstance->Config->FixedPart;
		else if (parameters.size() > 1)
			reason = ServerInstance->Config->PrefixPart + parameters[1] + ServerInstance->Config->SuffixPart;
	}
	else
	{
		if (parameters.size() > 1)
			reason = parameters[1];
	}

	if (CommandParser::LoopCall(user, this, parameters, 0))
		return CMD_SUCCESS;

	Channel* c = ServerInstance->FindChan(parameters[0]);

	if (c)
	{
		c->PartUser(user, reason);
	}
	else
	{
		user->WriteNumeric(ERR_NOSUCHNICK, "%s :No such nick/channel", parameters[0].c_str());
		return CMD_FAILURE;
	}

	return CMD_SUCCESS;
}

RouteDescriptor CommandPart::GetRouting(User* user, const std::vector<std::string>& parameters)
{
	return (IS_LOCAL(user) ? ROUTE_LOCALONLY : ROUTE_BROADCAST);
}
