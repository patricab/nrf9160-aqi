package aqi;
import java.sql.*;
import java.time.*;
import java.util.*;
import java.nio.ByteBuffer;

import org.eclipse.leshan.core.observation.*;
import org.eclipse.leshan.core.node.*;
import org.eclipse.leshan.core.response.*;
import org.eclipse.leshan.core.request.*;
import org.eclipse.leshan.server.registration.*;
import org.eclipse.leshan.core.model.ResourceModel.Type;
import org.eclipse.leshan.server.californium.LeshanServer;


public class Database {
	private static String user;
	private static String pass;

	public Database(String user, String pass) {
		this.user = user;
		this.pass = pass;
	}

	// JDBC driver name and database URL
	static final String JDBC_DRIVER = "org.postgresql.Driver";  
	static final String DB_URL = "jdbc:postgresql://localhost:5432/airqdb";

	private static Object getValue(ObserveResponse response) {
		Type type = ((LwM2mSingleResource)response.getContent()).getType();
		Object value = ((LwM2mSingleResource)response.getContent()).getValue();
		System.out.println("" + type + value.getClass());
		switch (type) {
			case OPAQUE:
				byte[] bytes = (byte []) value;
				if (bytes.length == 4)
					return ByteBuffer.wrap(bytes).getFloat();
				else if (bytes.length == 8)
					return ByteBuffer.wrap(bytes).getDouble();
				else if (bytes.length == 2 || bytes.length == 1)
					return ByteBuffer.wrap(bytes).getInt();
				else
					return null;
			default:
				break;
		}
		return value;
	}



	public static void setState(Registration reg, int status) {
		Statement stmt = null;
		Connection conn = null;
		String endpoint = reg.getEndpoint();
		System.out.println("Connecting to database...");

		String sql = "INSERT INTO devices(endpoint, status) " +
		             "VALUES('" + endpoint + "', " + status + ") " +
		             "ON CONFLICT (endpoint)" +
		             "DO UPDATE SET " +
		             "status = EXCLUDED.status," +
		             "time = now();";
		
		System.out.println(sql);

		try
		{
			Class.forName("org.postgresql.Driver");
			conn = DriverManager.getConnection(DB_URL, user, pass);
			System.out.println("INSERT data...");
			stmt = conn.createStatement();
			stmt.executeUpdate(sql);
			System.out.println("INSERT data successfully! ");
			stmt.close();//finally block used to close resources
		
		}
		catch(SQLException se)
		{
			//Handle errors for JDBC
			se.printStackTrace();
		}
		catch(Exception e)
		{
			//Handle errors for Class.forName
			e.printStackTrace();
		}
	}



	private static String getTimestamp(ReadResponse response) {
		//Type type = ((LwM2mSingleResource)response.getContent()).getType();
		//Object value = ((LwM2mSingleResource)response.getContent()).getValue();

		//java.util.Date timestamp = (java.util.Date) value;

		System.out.println("getTimestamp() " + response.getContent().toString());

		return (String) (((LwM2mSingleResource)response.getContent()).getValue());
	}



	public static void observation(ReadResponse response, String endpoint, 
		LeshanServer server, Registration registration, int obj, int inst, int res)
	{
	/*	Observation observation = response.getObservation();
		List<TimestampedLwM2mNode> timestampedNodes = response.getTimestampedLwM2mNode();

		System.out.println(timestampedNodes);


		byte[] id = observation.getId();
		LwM2mPath path = observation.getPath();
		String registrationId = observation.getRegistrationId();
		ContentFormat contentFormat = observation.getContentFormat();
		Map<String, String> context = observation.getContext();

		int obj  = path.getObjectId().intValue();
		int inst = path.getObjectInstanceId().intValue();
		int res  = path.getResourceId().intValue();
	*/

		//Value
		LwM2mSingleResource resource = ((LwM2mSingleResource) response.getContent());

		// LwM2mResource resource = (LwM2mResource) value;
		// System.out.println("valeur observation : " + resource.getValue().value)

		String timestamp = "0";

		try {
			ReadResponse timeresponse = server.send(registration, new ReadRequest(ContentFormat.TEXT, obj,inst,5518));
			timestamp = getTimestamp(timeresponse);
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("Error read timestamp" + obj + inst);
		}

		double val = (double) resource.getValue();
		//getValue(response);

		Statement stmt = null;
		Connection conn = null;
		System.out.println("Connecting to database...");
		String sql = "INSERT INTO observation(timestamp, endpoint, object, instance, resource, value) " + 
		             "VALUES(" + timestamp + ", '" + endpoint + "', " + obj + ", " + inst + ", " + res + ", " + val + ");";
		System.out.println(sql);
		try
		{
			Class.forName("org.postgresql.Driver");
			conn = DriverManager.getConnection(DB_URL, user, pass);
			System.out.println("INSERT data...");
			stmt = conn.createStatement();
			
			stmt.executeUpdate(sql);
			System.out.println("INSERT data successfully! " + timestamp);
			stmt.close();//finally block used to close resources
		
		}
		catch(SQLException se)
		{
			//Handle errors for JDBC
			se.printStackTrace();
		}
		catch(Exception e)
		{
			//Handle errors for Class.forName
			e.printStackTrace();
		}
	}

	public static void log(String endpoint, String ip, String regId, String message)
	{
		Statement stmt = null;
		Connection conn = null;
		System.out.println("Connecting to database...");
		String sql = "INSERT INTO log(endpoint, ip, regid, message) " + 
		             "VALUES('" + endpoint + "', '" + ip + "', '" + regId + "', '" + message + "');";
		System.out.println(sql);
		try
		{
			Class.forName("org.postgresql.Driver");
			conn = DriverManager.getConnection(DB_URL, user, pass);
			System.out.println("INSERT log...");
			stmt = conn.createStatement();
			
			stmt.executeUpdate(sql);
			System.out.println("Logged successfully!");
			stmt.close();//finally block used to close resources
		
		}
		catch(SQLException se)
		{
			//Handle errors for JDBC
			se.printStackTrace();
		}
		catch(Exception e)
		{
			//Handle errors for Class.forName
			e.printStackTrace();
		}
	}

	public static void logError(String endpoint, String ip, String regId, String error)
	{
		Statement stmt = null;
		Connection conn = null;
		System.out.println("Connecting to database...");
		String sql = "INSERT INTO log(endpoint, ip, regid, error) " + 
		             "VALUES('" + endpoint + "', '" + ip + "', '" + regId + "', '" + error + "');";
		try
		{
			Class.forName("org.postgresql.Driver");
			conn = DriverManager.getConnection(DB_URL, user, pass);
			System.out.println("INSERT error into log...");
			stmt = conn.createStatement();
			
			stmt.executeUpdate(sql);
			System.out.println("Logged error successfully!");
			stmt.close();//finally block used to close resources
		
		}
		catch(SQLException se)
		{
			//Handle errors for JDBC
			se.printStackTrace();
		}
		catch(Exception e)
		{
			//Handle errors for Class.forName
			e.printStackTrace();
		}
	}
}