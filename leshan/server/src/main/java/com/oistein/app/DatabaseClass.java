package com.oistein.app;
import java.sql.*;
import java.time.*;
import java.util.*;
import java.nio.ByteBuffer;

import org.eclipse.leshan.core.observation.*;
import org.eclipse.leshan.core.node.*;
import org.eclipse.leshan.core.response.*;
import org.eclipse.leshan.core.request.*;
import org.eclipse.leshan.core.model.ResourceModel.Type;


public class DatabaseClass {
	private static String user;
	private static String pass;

	public DatabaseClass(String user, String pass) {
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
				else
					return null;
			default:
				break;

		}
		return value;
	}

	public static void observation(ObserveResponse response, String ep)
	{
		Observation observation = response.getObservation();
		List<TimestampedLwM2mNode> timestampedNodes = response.getTimestampedLwM2mNode();

		System.out.println(timestampedNodes);


		byte[] id = observation.getId();
		LwM2mPath path = observation.getPath();
		String registrationId = observation.getRegistrationId();
		ContentFormat contentFormat = observation.getContentFormat();
		Map<String, String> context = observation.getContext();

		int obj = path.getObjectId().intValue();
		int inst = path.getResourceId().intValue();
		int res = path.getObjectInstanceId().intValue();

		if (res == 5518) {
			java.util.Date date = (java.util.Date) getValue(response);
			System.out.println("Timestamp: " + date);
		}

		double val = (double) getValue(response);


		Statement stmt = null;
		Connection conn = null;
		System.out.println("Connecting to database...");
		try
		{
			Class.forName("org.postgresql.Driver");
			conn = DriverManager.getConnection(DB_URL, user, pass);
			System.out.println("INSERT data...");
			stmt = conn.createStatement();
			String sql = "INSERT INTO observation(ep, object, instance, resource, value) " + 
			             "VALUES('" + ep + "', " + obj + ", " + inst + ", " + res + ", " + val + ");";
			stmt.executeUpdate(sql);
			System.out.println(sql);
			System.out.println("INSERT data successfully!");
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