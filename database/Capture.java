//import OkHttpClient.*;
import java.util.*;
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
		RESTcoms coms = new RESTcoms("http://192.168.0.120:8080");

		List<Client> clients = coms.getCLients();
		traverseClients();
		coms.logObservation(clients.get(0).getEndpoint(), 3303, 0, 5700);

		JavaDBCom airq = new JavaDBCom("airq", "airqpass");
		airq.insert(clients.get(0).getEndpoint(), 3303, 0, 5700, 13.37);
	}

	private void traverseClients(List<Client> clients)
	{
		for(int i = 0; i < clients.size(); i++) 
		{
			System.out.println("ep:        " + clients.get(i).getEndpoint());			
			System.out.println("ipaddr:    " + clients.get(i).getAddress());		   
			System.out.println("regID:     " + clients.get(i).getRegistrationId());
			System.out.println("reg date:  " + clients.get(i).getRegistrationDate());
			
			for(int j = 0; j < clients.get(i).getObjectLinks().size(); j++)
				System.out.println("obj:       " + clients.get(i).getObjectLinks().get(j).getUrl());
		}
		System.out.println("");
	}
}
