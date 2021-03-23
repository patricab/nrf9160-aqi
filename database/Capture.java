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
	public static void main(String args[]) 
	{
		//coms.traverseClients(clients);


		final ScheduledExecutorService executorService = Executors.newSingleThreadScheduledExecutor();
		executorService.scheduleAtFixedRate(Capture::logAll, 0, 30, TimeUnit.SECONDS);
	}


	private static void logAll()
	{
		JavaDBCom airqdb = new JavaDBCom("airq", "airqpass");
		RESTcoms coms = new RESTcoms("http://192.168.0.120:8080");
		List<Client> clients = coms.getCLients();

		for(int i = 0; i < clients.size(); i++) 
		{
			String endpoint = clients.get(i).getEndpoint();
			String ip = clients.get(i).getAddress();
			String regID = clients.get(i).getRegistrationId();

			try {
				coms.setTimestamp(endpoint);
			} catch (IOException e) {
				System.out.println(e.toString());
			}

			for(int j = 0; j < clients.get(i).getObjectLinks().size(); j++)
			{
				switch (clients.get(i).getObjectLinks().get(j).getUrl())
				{
				/*case "/6/0": // Location
					insertDB(airqdb, coms, "observation", endpoint, 6, 0, 1);
					break;*/
				case "/3300/0": // Generic Sensor VOC Ohm
					insertDB(airqdb, coms, "observation", endpoint, 3300, 0, 5700);
					break;
				case "/3303/0": // Temperature Sensor °C
					insertDB(airqdb, coms, "observation", endpoint, 3303, 0, 5700);
					break;
				case "/3304/0": // Humidity Sensor %RH
					insertDB(airqdb, coms, "observation", endpoint, 3304, 0, 5700);
					break;
				case "/3325/0": // Concentration Sensor µg/m3
					insertDB(airqdb, coms, "observation", endpoint, 3325, 0, 5700);
					break;
				case "/3335/0": // Colour sensor
					insertDB(airqdb, coms, "observation", endpoint, 3335, 0, 5706);
					break;
				case "/10314/0": // Particulate sensor µg/m3
					insertDB(airqdb, coms, "observation", endpoint, 10314, 0, 5700);
					break;
				}
			}
		}
	}

	private static void insertDB(JavaDBCom database, RESTcoms rest, String table,
	                             String endpoint, int object, int instance, int resource)
	{
		try {
			String value = rest.logObservation(endpoint, object, instance, resource);
			String time = rest.logObservation(endpoint, object, instance, 5518);
		
			DateTimeFormatter formatter = DateTimeFormatter.ofPattern("uuuu-MM-dd\'T\'HH:mm:ss\'Z\'");
			LocalDateTime timestamp = LocalDateTime.parse(time, formatter);
			timestamp.plusSeconds(Long.parseLong(time));

			if (value.equals("error")) {
			} else if ( time.equals("error")) {
				database.insert(table, endpoint, object, instance, resource, Float.parseFloat(value));
			} else {
				database.insertTS(table, timestamp, endpoint, object, instance, resource, Float.parseFloat(value));
			}
		} catch (Exception e) {
			System.out.println(e.toString());
		}
	}
}

/*String ip, String regID,*/