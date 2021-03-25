import java.util.*;
import java.lang.*;
import java.lang.Object.*;
import java.io.IOException;
import java.time.*;
import java.time.format.*;

import okhttp3.*;
import static org.junit.Assert.*;
import static org.hamcrest.CoreMatchers.*;
import org.junit.*;
import com.fasterxml.jackson.databind.*;
import com.fasterxml.jackson.core.*;
import com.fasterxml.jackson.databind.node.*;
import com.fasterxml.jackson.core.type.TypeReference;

public class RESTcoms {
	String url = null;

	public RESTcoms(String url) {
		this.url = url;
	}


	public void setTimestamp(String ep) {
		Response response = null;

		try {
			OkHttpClient client = new OkHttpClient();

			DateTimeFormatter formatter = DateTimeFormatter.ofPattern("uuuu-MM-dd\'T\'HH:mm:ss\'Z\'")
			                                               .withZone(ZoneOffset.ofHours(+1));
			ZonedDateTime timestamp = ZonedDateTime.now();
			String json = "{\"id\":13,\"value\":" + formatter.format(timestamp) + "},"
			            + "{\"id\":14,\"value\":" + "\"+01:00\"},"
			            + "{\"id\":15,\"value\":" + "\"Europe/Paris\"}";

			RequestBody body = RequestBody.create(MediaType.parse("application/json"), json);

			Request request = new Request.Builder()
				.url(url + "/api/clients/" + ep + "/3/0")
				.post(body)
				.build();

			Call call = client.newCall(request);
			response = call.execute();

			assertThat(response.code(), equalTo(200));

		} catch (IOException e) {
			System.out.println("Error in setTimestamp: "+ e.toString());
		} finally {
			response.body().close();
		}
	}


	public String logObservation(String ep, int object, int instance, int resource) throws IOException {
		ObjectMapper mapper = new ObjectMapper();
		mapper.configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
		OkHttpClient res = new OkHttpClient();

		Request request = new Request.Builder()
			.url(url + "/api/clients/" + ep + "/" + object + "/" + instance + "/" + resource)
			.build(); // GET observation

		// Decode JSON
		//Airq.insertObservation(json);
		// try {
		Response response = res.newCall(request).execute();
		String json = response.body().string();
		Observation observation = mapper.readValue(json, new TypeReference<Observation>(){});
		readObservation(observation);
		return observation.getValue();
		/*} catch (IOException error) {
			System.out.println("Error in logObservation: " + error.toString());
			return "error";
		}*/
	}


	public List<Client> getCLients() {
		ObjectMapper mapper = new ObjectMapper();
		mapper.configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
		OkHttpClient res = new OkHttpClient();

		Request request = new Request.Builder().url(url + "/api/clients").build(); // defaults to GET

		try {
			Response response = res.newCall(request).execute();
			String json = response.body().string();
			List<Client> clients = mapper.readValue(json, new TypeReference<List<Client>>(){});
			System.out.println("No Clients: " + clients.size() + "\n");
			traverseClients(clients);
			return clients;
		} catch (IOException e) {
			System.out.println("Error in getCLients: " + e.toString());
			return null;
		}
	}


	private void traverseClients(List<Client> clients){
		for(int i = 0; i < clients.size(); i++) {
			System.out.println("ep:        " + clients.get(i).getEndpoint());			
			System.out.println("ipaddr:    " + clients.get(i).getAddress());		   
			System.out.println("regID:     " + clients.get(i).getRegistrationId());
			System.out.println("reg date:  " + clients.get(i).getRegistrationDate());
			
			for(int j = 0; j < clients.get(i).getObjectLinks().size(); j++)
				System.out.println("obj:       " + clients.get(i).getObjectLinks().get(j).getUrl());
		}
		System.out.println("");
	}


	private void readObservation(Observation observation) {
		System.out.println("ID: " + observation.getId() + " Value: " + observation.getValue());
	}
}

/*if id = 5700 {
	"Sensor value"
}*/