//import OkHttpClient.*;
import java.util.*;
import java.util.concurrent.*;
import java.lang.*;
import java.io.IOException;

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
		executorService.scheduleAtFixedRate(Capture::logAll, 0, 1, TimeUnit.MINUTES);
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

			for(int j = 0; j < clients.get(i).getObjectLinks().size(); j++)
			{
				switch (clients.get(i).getObjectLinks().get(j).getUrl())
				{
				case "/3303/0":
					insertDB(airqdb, coms, endpoint, ip, regID, 3303, 0, 5700);
					break;
				case "/3304/0":
					insertDB(airqdb, coms, endpoint, ip, regID, 3304, 0, 5700);
					break;
				case "/3325/0":
					insertDB(airqdb, coms, endpoint, ip, regID, 3325, 0, 5700);
					break;
				case "/3335/0":
					insertDB(airqdb, coms, endpoint, ip, regID, 3335, 0, 5700);
					break;
				case "/10314/0":
					insertDB(airqdb, coms, endpoint, ip, regID, 10314, 0, 5700);
					break;
				}
			}
		}
	}

	private static void insertDB(JavaDBCom database, RESTcoms rest, String endpoint,
	                             String ip, String regID, int object, int instance, int resource)
	{
		database.insert(endpoint, ip, regID, object, instance, resource, 
		                rest.logObservation(endpoint, object, instance, resource));
	}
}
