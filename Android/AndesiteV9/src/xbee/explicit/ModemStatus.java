package xbee.explicit;

import java.util.ArrayList;

public class ModemStatus {
	
	public static final String MODEM_HARDWARE_RESET = "00", MODEM_WATCHDOG_RESET = "01",
			MODEM_JOINED_NETWORK = "02", MODEM_DISASSOCIATED = "03", MODEM_COORD_STARTED = "06", MODEM_KEY_UPDATED = "07",
			MODEM_VOLT_EXCEEDED = "0D", MODEM_CONFIG_CHANGED = "11", MODEM_STACK_ERROR = "80";
	
	public static String getStatus(ArrayList<Byte> packet, long modemTime)
	{
		String status = Common.byteToHex(packet.get(4));
		
		if(status.equalsIgnoreCase(MODEM_HARDWARE_RESET))
		{
			return "Hardware reset at " + new Common().millisToDate(modemTime);
		}
		else if(status.equalsIgnoreCase(MODEM_WATCHDOG_RESET))
		{
			return "Watchdog timer reset at " + new Common().millisToDate(modemTime);
		}
		else if(status.equalsIgnoreCase(MODEM_JOINED_NETWORK))
		{
			return "Joined Network at " + new Common().millisToDate(modemTime);
		}
		else if(status.equalsIgnoreCase(MODEM_DISASSOCIATED))
		{
			return "Disassociated at " + new Common().millisToDate(modemTime);
		}
		else if(status.equalsIgnoreCase(MODEM_COORD_STARTED))
		{
			return "Coordinator started at " + new Common().millisToDate(modemTime);
		}
		else if(status.equalsIgnoreCase(MODEM_KEY_UPDATED))
		{
			return "Network security key updated at " + new Common().millisToDate(modemTime);
		}
		else if(status.equalsIgnoreCase(MODEM_VOLT_EXCEEDED))
		{
			return "Voltage supply limit exceeded at " + new Common().millisToDate(modemTime);
		}
		else if(status.equalsIgnoreCase(MODEM_CONFIG_CHANGED))
		{
			return "Modem configuration changed while join in progress at " + new Common().millisToDate(modemTime);
		}
		else
		{
			return "Stack error at " + new Common().millisToDate(modemTime);
		}
	}

}
