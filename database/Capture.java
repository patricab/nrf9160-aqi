//import OkHttpClient.*;
import java.util.*;
import java.util.concurrent.*;
import java.text.*;
import java.lang.*;
import java.io.IOException;
import java.sql.*;
import java.time.*;
import java.time.format.*;

import okhttp3.*;
import com.fasterxml.jackson.databind.*;
import com.fasterxml.jackson.core.*;
import com.fasterxml.jackson.databind.node.*;

public class Capture 
{
	private static JavaDBCom airqdb = new JavaDBCom("airq", "airqpass");
	private static RESTcoms coms = new RESTcoms("http://192.168.0.120:8080");
	private static List<Client> clients = coms.getCLients();
	private static List<Client> prevClients = clients;

	public static void main(String args[]) 
	{
		final ScheduledExecutorService executorService = Executors.newSingleThreadScheduledExecutor();
		executorService.scheduleAtFixedRate(Capture::logAll, 0, 60, TimeUnit.SECONDS);
	}


	private static void logAll()
	{

		for(int i = 0; i < clients.size(); i++) 
		{
			boolean justRegistered = false;
			String endpoint = clients.get(i).getEndpoint();
			String regDate = clients.get(i).getRegistrationDate();
			String regId = clients.get(i).getRegistrationId();
			String ip = clients.get(i).getAddress();

			if (regId != prevClients.get(i).getRegistrationId())
			{
				prevClients.set(i, clients.get(i));
				justRegistered = true;
				airqdb.log(endpoint, ip, regId);
				coms.setTimestamp(endpoint);
			}


			for(int j = 0; j < clients.get(i).getObjectLinks().size(); j++)
			{
				if (justRegistered) {
					switch (clients.get(i).getObjectLinks().get(j).getUrl())
					{
					/*case "/6/0": // Location
						observe(endpoint, ip, regId, 6, 0, 1);
						break;*/
					case "/3300/0": // Generic Sensor VOC Ohm
						observe(endpoint, ip, regId, 3300, 0, 5700);
						break;
					case "/3303/0": // Temperature Sensor °C
						observe(endpoint, ip, regId, 3303, 0, 5700);
						break;
					case "/3304/0": // Humidity Sensor %RH
						observe(endpoint, ip, regId, 3304, 0, 5700);
						break;
					case "/3325/0": // Concentration Sensor µg/m3
						observe(endpoint, ip, regId, 3325, 0, 5700);
						break;
					case "/3335/0": // Colour sensor
						observe(endpoint, ip, regId, 3335, 0, 5706);
						break;
					case "/10314/0": // Particulate sensor µg/m3
						observe(endpoint, ip, regId, 10314, 0, 5700);
						break;
					case "/10314/1": // Particulate sensor µg/m3
						observe(endpoint, ip, regId, 10314, 1, 5700);
						break;
					case "/10314/2": // Particulate sensor µg/m3
						observe(endpoint, ip, regId, 10314, 2, 5700);
						break;
					}
				}
				switch (clients.get(i).getObjectLinks().get(j).getUrl())
				{
				/*case "/6/0": // Location
					insertDB(endpoint, regDate, ip, regId, 6, 0, 1);
					break;*/
				case "/3300/0": // Generic Sensor VOC Ohm
					insertDB(endpoint, regDate, ip, regId, 3300, 0, 5700);
					break;
				case "/3303/0": // Temperature Sensor °C
					insertDB(endpoint, regDate, ip, regId, 3303, 0, 5700);
					break;
				case "/3304/0": // Humidity Sensor %RH
					insertDB(endpoint, regDate, ip, regId, 3304, 0, 5700);
					break;
				case "/3325/0": // Concentration Sensor µg/m3
					insertDB(endpoint, regDate, ip, regId, 3325, 0, 5700);
					break;
				case "/3335/0": // Colour sensor
					insertDB(endpoint, regDate, ip, regId, 3335, 0, 5706);
					break;
				case "/10314/0": // Particulate sensor µg/m3
					insertDB(endpoint, regDate, ip, regId, 10314, 0, 5700);
					break;
				case "/10314/1": // Particulate sensor µg/m3
					insertDB(endpoint, regDate, ip, regId, 10314, 1, 5700);
					break;
				case "/10314/2": // Particulate sensor µg/m3
					insertDB(endpoint, regDate, ip, regId, 10314, 2, 5700);
					break;
				}
			}
		}
	}

	private static void insertDB(String endpoint, String regDate, String ip,
	                             String regId, int object, int instance, int resource)
	{
		try {
			String value = null;
			String time = null;
		
			DateTimeFormatter formatter = DateTimeFormatter.ofPattern("uuuu-MM-dd\'T\'HH:mm:ss\'Z\'")
			                                               .withZone(ZoneOffset.ofHours(0));
			//ZonedDateTime timestamp = ZonedDateTime.parse(regDate, formatter);
			ZonedDateTime timestamp = ZonedDateTime.parse(time, formatter);
			//timestamp = timestamp.plusSeconds(Long.parseLong(time, 16));

			if (value.equals("error")) {
			} else if ( time.equals("error")) {
				airqdb.insert(endpoint, object, instance, resource, Float.parseFloat(value));
			} else {
				airqdb.insertTS(timestamp, endpoint, object, instance, resource, Float.parseFloat(value));
			}
		} catch (Exception e) {
			System.out.println("Error in insertDB: " + e.toString());
			airqdb.logError(endpoint, ip, regId, e.toString());
		}
	}


	private static void observe(String endpoint, String ip, String regId,
	                            int object, int instance, int resource)
	{
		try
		{
			coms.observeResourceStart(endpoint, object, instance, resource);
			coms.observeResourceStart(endpoint, object, instance, 5518);
		}
		catch (IOException e) {
			System.out.println("Error in observe: " + e.toString());
			airqdb.logError(endpoint, ip, regId,  e.toString());
		}
	}
}


//registered check
//start observe
//observe values

