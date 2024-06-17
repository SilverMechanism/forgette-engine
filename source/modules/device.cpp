module;
module device;

Device::Device()
{
	uses[Command::Fallback] = &Device::report_target_invalid;
}

void Device::use(Command command, Unit* user, Target& target)
{
	if (uses.find(command) == uses.end())
	{
		uses[Command::Fallback](user, target);
	}
	else
	{
		uses[command](user, target);
	}
}

void Device::report_target_invalid(Unit* user, Target& target)
{
	std::cout << "Invalid target!" << std::endl;
}